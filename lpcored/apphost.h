// apphost.h
//
// Host an LPCore Internal Application
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: apphost.h,v 1.3 2013/08/20 20:59:30 cvs Exp $
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

#ifndef APPHOST_H
#define APPHOST_H

#include <stdint.h>

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QTimer>

#include <lpcore/lpconfig.h>

class AppHost : public QObject
{
  Q_OBJECT;
 public:
  AppHost(int id,LPConfig *config,QObject *parent=NULL);
  ~AppHost();
  void kill();

 private slots:
  void startData();
  void startedData();
  void finishedData(int exit_code,QProcess::ExitStatus exit_status);
  void errorData(QProcess::ProcessError err);

 private:
  void Restart();
  QProcess *app_process;
  int app_id;
  QTimer *app_start_timer;
  int app_fail_count;
  int app_start_interval;
  LPConfig *app_config;
};


#endif  // APPHOST_H
