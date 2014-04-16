// lpremotebutton.cpp
//
// lpremotebutton(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpremotebutton.cpp,v 1.1 2013/11/21 22:31:48 cvs Exp $
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

#include "lpremotebutton.h"

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
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpremotebutton",LPREMOTEBUTTON_USAGE);
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
    openlog("lpremotebutton",LOG_PERROR,LOG_USER);
  }
  else {
    openlog("lpremotebutton",0,LOG_USER);
  }

  //
  // Read Configuration
  //
  lp_profile=new LPProfile();
  if(!lp_profile->setSource(LPREMOTEBUTTON_CONF_FILE)) {
    syslog(LOG_ERR,"unable to open configuration file at \"%s\"",
	   LPREMOTEBUTTON_CONF_FILE);
    exit(256);
  }
  Button *b=new Button(lp_profile);
  while(b->load(lp_buttons.size())) {
    lp_buttons.push_back(b);
    b=new Button(lp_profile);
  }
  delete b;

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
  Button *b=NULL;

  switch(0xFF&msg[2]) {
  case 0x51:
  case 0x52:
  case 0x53:
    if((msg.size()==8)||(msg.size()==9)) {
      for(unsigned i=0;i<lp_buttons.size();i++) {
	b=lp_buttons[i];
	if(b->belongsTo(0xFF&msg[3],0xFF&msg[6],
			((0xFF&msg[4])<<8)+(0xFF&msg[5]),0xFF&msg[7])) {
	  for(unsigned j=0;j<b->destinations();j++) {
	    switch(0xFF&msg[2]) {
	    case 0x51:   // Momentary
	      switch(b->triggerAction(j)) {
	      case Button::ActionNormal:
		SendOn(b,j);
		break;
	      
	      case Button::ActionInvert:
		SendOff(b,j);
		break;
	      
	      case Button::ActionMomentary:
		SendMomentary(b,j,0xFF&msg[8]);
		break;
	      }
	      break;

	    case 0x52:   // Turn ON
	      switch(b->triggerType()) {
	      case Button::TypeLevel:
	      case Button::TypeRising:
		switch(b->triggerAction(j)) {
		case Button::ActionNormal:
		  SendOn(b,j);
		  break;

		case Button::ActionInvert:
		  SendOff(b,j);
		  break;

		case Button::ActionMomentary:
		  SendMomentary(b,j,6);
		  break;
		}
		break;

	      case Button::TypeFalling:
		break;
	      }
	      break;

	    case 0x53:    // Turn OFF
	      switch(b->triggerType()) {
	      case Button::TypeLevel:
	      case Button::TypeFalling:
		switch(b->triggerAction(j)) {
		case Button::ActionNormal:
		  SendOff(b,j);
		  break;

		case Button::ActionInvert:
		  SendOn(b,j);
		  break;

		case Button::ActionMomentary:
		  SendMomentary(b,j,6);
		  break;
		}
		break;

	      case Button::TypeRising:
		break;
	      }
	      break;
	    }
	  }
	}
      }
    }
    break;
  }
}


void MainObject::SendOn(Button *b,unsigned dst)
{
  char lp2[1024];

  lp2[0]=0x02;
  lp2[1]=0x06;
  lp2[2]=0x52;
  lp2[3]=0xFF&b->dstEngine(dst);
  lp2[4]=0xFF&(b->dstDevice(dst)>>8);
  lp2[5]=0xFF&b->dstDevice(dst);
  lp2[6]=0xFF&b->dstSurface(dst);
  lp2[7]=0xFF&b->dstBuss(dst);
  lp_connection->writeMessage(lp2,8);
}


void MainObject::SendOff(Button *b,unsigned dst)
{
  char lp2[1024];

  lp2[0]=0x02;
  lp2[1]=0x06;
  lp2[2]=0x53;
  lp2[3]=0xFF&b->dstEngine(dst);
  lp2[4]=0xFF&(b->dstDevice(dst)>>8);
  lp2[5]=0xFF&b->dstDevice(dst);
  lp2[6]=0xFF&b->dstSurface(dst);
  lp2[7]=0xFF&b->dstBuss(dst);
  lp_connection->writeMessage(lp2,8);
}


void MainObject::SendMomentary(Button *b,unsigned dst,int duration)
{
  char lp2[1024];

  lp2[0]=0x02;
  lp2[1]=0x07;
  lp2[2]=0x51;
  lp2[3]=0xFF&b->dstEngine(dst);
  lp2[4]=0xFF&(b->dstDevice(dst)>>8);
  lp2[5]=0xFF&b->dstDevice(dst);
  lp2[6]=0xFF&b->dstSurface(dst);
  lp2[7]=0xFF&b->dstBuss(dst);
  lp2[8]=0xFF&duration;
  lp_connection->writeMessage(lp2,9);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
