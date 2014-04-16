// lpautocue.cpp
//
// lpautocue(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpautocue.cpp,v 1.1 2013/11/21 22:31:44 cvs Exp $
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

#include <signal.h>
#include <syslog.h>

#include <QtGui/QApplication>

#include <lpcore/lpcmdswitch.h>
#include <lpcore/lpconfig.h>

#include "lpautocue.h"

void SigHandler(int signum)
{
  switch(signum) {
  case SIGHUP:
  case SIGINT:
  case SIGTERM:
    exit(0);
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  bool debug=false;
  QString hostname="localhost";

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpautocue",LPAUTOCUE_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="-d") {
      debug=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      syslog(LOG_ERR,"invalid argument \"%s\"",
	     (const char *)cmd->key(i).toAscii());
      exit(256);
    }
  }
  delete cmd;

  //
  // Open Syslog
  //
  if(debug) {
    openlog("lpautocue",LOG_PERROR,LOG_USER);
  }
  else {
    openlog("lpautocue",0,LOG_USER);
  }

  //
  // Read Configuration
  //
  lp_profile=new LPProfile();
  if(!lp_profile->setSource(LPAUTOCUE_CONF_FILE)) {
    syslog(LOG_ERR,"unable to open configuration file at \"%s\"",
	   LPAUTOCUE_CONF_FILE);
    exit(256);
  }
  Surface *s=new Surface(lp_profile);
  while(s->load(lp_surfaces.size())) {
    lp_surfaces.push_back(s);
    s=new Surface(lp_profile);
  }
  delete s;

  //
  // Set Signal Handlers
  //
  signal(SIGHUP,SigHandler);
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);

  //
  // Server Connection
  //
  lp_connection=new LPConnection(false,this);
  connect(lp_connection,SIGNAL(messageReceived(const LPMessage &)),
	  this,SLOT(serverMessageReceivedData(const LPMessage &)));

  lp_connection->
    connectToHost(lp_profile->stringValue("Server","Hostname","localhost"),
		  lp_profile->intValue("Server","Port",
				       LPCORE_CONNECTION_TCP_PORT),
		  lp_profile->stringValue("Server","ProfileName","profile"),
		  lp_profile->stringValue("Server","Password"));
}


void MainObject::serverMessageReceivedData(const LPMessage &msg)
{
  char lp2[1024];
  Surface *s=NULL;

  switch(0xFF&msg[2]) {
  case 0x52:
  case 0x53:
    if(msg.size()==8) {
      for(unsigned i=0;i<lp_surfaces.size();i++) {
	s=lp_surfaces[i];
	if(s->belongsTo(0xFF&msg[3],0xFF&msg[6],0xFF&msg[7])) {
	  switch(0xFF&msg[2]) {
	  case 0x52:   // Turn ON
	    if(s->autoOff()) {
	      lp2[0]=0x02;
	      lp2[1]=0x06;
	      lp2[2]=0x53;
	      lp2[3]=0xFF&s->engine();
	      lp2[4]=msg[4];
	      lp2[5]=msg[5];
	      lp2[6]=0xFF&s->surface();
	      lp2[7]=0xFF&s->dstBuss();
	      lp_connection->writeMessage(lp2,8);
	    }
	    break;

	  case 0x53:    // Turn OFF
	    if(s->autoOn()) {
	      lp2[0]=0x02;
	      lp2[1]=0x06;
	      lp2[2]=0x52;
	      lp2[3]=0xFF&s->engine();
	      lp2[4]=msg[4];
	      lp2[5]=msg[5];
	      lp2[6]=0xFF&s->surface();
	      lp2[7]=0xFF&s->dstBuss();
	      lp_connection->writeMessage(lp2,8);
	    }
	    break;
	  }
	}
      }
    }
    break;
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
