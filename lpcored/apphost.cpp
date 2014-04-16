// apphost.cpp
//
// Host an LPCore Internal Application
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: apphost.cpp,v 1.4 2013/12/31 19:31:05 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <signal.h>

#include "apphost.h"

AppHost::AppHost(int id,LPConfig *config,QObject *parent)
  : QObject(parent)
{
  app_id=id;
  app_config=config;
  app_start_interval=0;
  app_fail_count=0;

  //
  // Start Timer
  //
  app_start_timer=new QTimer(this);
  app_start_timer->setSingleShot(true);
  connect(app_start_timer,SIGNAL(timeout()),this,SLOT(startData()));

  //
  // Process
  //
  app_process=new QProcess(this);
  connect(app_process,SIGNAL(started()),this,SLOT(startedData()));
  connect(app_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	  this,SLOT(finishedData(int,QProcess::ExitStatus)));
  connect(app_process,SIGNAL(error(QProcess::ProcessError)),
	  this,SLOT(errorData(QProcess::ProcessError)));
  Restart();
}


AppHost::~AppHost()
{
  delete app_process;
  delete app_start_timer;
}


void AppHost::kill()
{
  syslog(LOG_NOTICE,"killing process %u",(unsigned)app_process->pid());
  ::kill(app_process->pid(),SIGTERM);
}


void AppHost::startData()
{
  app_start_interval=0;
  app_process->start(app_config->applicationCommand(app_id),
		     app_config->applicationArguments(app_id));
}


void AppHost::startedData()
{
  app_start_interval=0;
  syslog(LOG_DEBUG,"internal application \"%s\" started normally",
	 (const char *)app_config->applicationCommand(app_id).toAscii());
}


void AppHost::finishedData(int exit_code,QProcess::ExitStatus exit_status)
{
  switch(exit_status) {
  case QProcess::CrashExit:
    syslog(LOG_WARNING,"internal application \"%s\" crashed",
	   (const char *)app_config->applicationCommand(app_id).toAscii());
    break;

  case QProcess::NormalExit:
    syslog(LOG_DEBUG,"internal application \"%s\" exited, exit code=%d",
	   (const char *)app_config->applicationCommand(app_id).toAscii(),
	   exit_code);
    break;
  }
  app_fail_count++;
  if(app_config->applicationRespawn(app_id)) {
    Restart();
  }
}


void AppHost::errorData(QProcess::ProcessError err)
{
  switch(err) {
  case QProcess::FailedToStart:
    syslog(LOG_WARNING,"internal application \"%s\" failed to start",
	   (const char *)app_config->applicationCommand(app_id).toAscii());
    app_fail_count++;
    if(app_config->applicationRespawn(app_id)) {
      Restart();
    }
    break;

  case QProcess::Crashed:
    syslog(LOG_WARNING,"internal application \"%s\" crashed",
	   (const char *)app_config->applicationCommand(app_id).toAscii());
    app_fail_count++;
    break;

  case QProcess::UnknownError:
    syslog(LOG_WARNING,"internal application \"%s\" returned an unknown error",
	   (const char *)app_config->applicationCommand(app_id).toAscii());
    app_fail_count++;
    break;

  case QProcess::Timedout:
  case QProcess::WriteError:
  case QProcess::ReadError:
    break;
  }
}


void AppHost::Restart()
{
  if(app_fail_count>LPCORE_APPLICATION_FAIL_LIMIT) {
    app_start_interval=5000;
    app_fail_count=0;
    syslog(LOG_WARNING,
	   "application \"%s\" respawning too quickly,restart delayed",
	   (const char *)app_config->applicationCommand(app_id).toAscii());
  }
  app_start_timer->start(app_start_interval);
}
