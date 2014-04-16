// lpmiccontrol.cpp
//
// lpmiccontrol(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpmiccontrol.cpp,v 1.1 2013/11/21 22:31:46 cvs Exp $
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

#include "lpmiccontrol.h"

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
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpmiccontrol",LPMICCONTROL_USAGE);
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
    openlog("lpmiccontrol",LOG_PERROR,LOG_USER);
  }
  else {
    openlog("lpmiccontrol",0,LOG_USER);
  }

  //
  // Read Configuration
  //
  lp_profile=new LPProfile();
  if(!lp_profile->setSource(LPMICCONTROL_CONF_FILE)) {
    syslog(LOG_ERR,"unable to open configuration file at \"%s\"",
	   LPMICCONTROL_CONF_FILE);
    exit(256);
  }
  Mic *m=new Mic(lp_profile);
  while(m->load(lp_mics.size())) {
    lp_mics.push_back(m);
    m=new Mic(lp_profile);
  }
  delete m;

  //
  // Set Signal Handlers
  //
  signal(SIGHUP,SigHandler);
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);

  //
  // Server Connection
  //
  lp_connection=new LPConnection(true,this);
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
  int turret=0;
  Mic::Function func;

  switch(msg.command()) {
  case LPMessage::CmdTurnOn:
    for(unsigned i=0;i<lp_mics.size();i++) {
      if(lp_mics[i]->isActive(lp_connection)) {
	if(lp_mics[i]->belongsTo(msg.engine(),msg.surface(),
				 msg.device(),msg.buss())) {
	  ProcessMic(lp_mics[i],true,msg);
	}
	if(lp_mics[i]->belongsToTalkback(msg.engine(),msg.surface(),
					 msg.device(),msg.buss())) {
	  ProcessTalkback(lp_mics[i],true,msg);
	}
	if((turret=lp_mics[i]->belongsToTurret(msg.engine(),msg.surface(),
				   msg.device(),msg.buss(),&func))>=0) {
	  ProcessTurret(lp_mics[i],turret,func,true,msg);
	}
      }
    }
    break;
   
  case LPMessage::CmdTurnOff:
    for(unsigned i=0;i<lp_mics.size();i++) {
      if(lp_mics[i]->isActive(lp_connection)) {
	if(lp_mics[i]->belongsTo(msg.engine(),msg.surface(),
				 msg.device(),msg.buss())) {
	  ProcessMic(lp_mics[i],false,msg);
	}
	if(lp_mics[i]->belongsToTalkback(msg.engine(),msg.surface(),
					 msg.device(),msg.buss())) {
	  ProcessTalkback(lp_mics[i],false,msg);
	}
	if((turret=lp_mics[i]->belongsToTurret(msg.engine(),msg.surface(),
				   msg.device(),msg.buss(),&func))>=0) {
	  ProcessTurret(lp_mics[i],turret,func,false,msg);
	}
      }
    }
    break;

  default:
    break;
  }
}


void MainObject::ProcessMic(Mic *m,bool state,const LPMessage &msg)
{
  char lp2[256];
  Mic::Function func[2]={Mic::OffLamp,Mic::OnLamp};
  
  if(state) {
    func[0]=Mic::OnLamp;
    func[1]=Mic::OffLamp;
  }
  for(unsigned i=0;i<m->turrets();i++) {
    lp2[0]=0x02;
    lp2[1]=0x06;
    lp2[2]=0xFF&LPMessage::CmdTurnOn;
    lp2[3]=0xFF&m->turretEngine(func[0],i);
    lp2[4]=0xFF&(m->turretDevice(func[0],i)>>8);
    lp2[5]=0xFF&m->turretDevice(func[0],i);
    lp2[6]=0xFF&m->turretSurface(func[0],i);
    lp2[7]=0xFF&m->turretBuss(func[0],i);
    lp_connection->writeMessage(lp2,8);

    lp2[0]=0x02;
    lp2[1]=0x06;
    lp2[2]=0xFF&LPMessage::CmdTurnOff;
    lp2[3]=0xFF&m->turretEngine(func[1],i);
    lp2[4]=0xFF&(m->turretDevice(func[1],i)>>8);
    lp2[5]=0xFF&m->turretDevice(func[1],i);
    lp2[6]=0xFF&m->turretSurface(func[1],i);
    lp2[7]=0xFF&m->turretBuss(func[1],i);
    lp_connection->writeMessage(lp2,8);
  }
}


void MainObject::ProcessTalkback(Mic *m,bool state,const LPMessage &msg)
{
  char lp2[256];

  if(state) {
    //
    // Mic Audio to Talkback
    //
    lp2[0]=0x02;
    lp2[1]=0x08;
    lp2[2]=0x54;   // Input Assign
    lp2[3]=0xFF&m->engine();
    lp2[4]=0xFF&(m->talkbackMixerInputDevice(0)>>8);
    lp2[5]=0xFF&m->talkbackMixerInputDevice(0);
    lp2[6]=0xFF&m->surface();
    lp2[7]=0xFF&m->talkbackMicEngine();
    lp2[8]=0xFF&(m->talkbackMicDevice()>>8);
    lp2[9]=0xFF&m->talkbackMicDevice();
    lp_connection->writeMessage(lp2,10);

    //
    // GreenRoom to Talkback
    //
    lp2[0]=0x02;
    lp2[1]=0x08;
    lp2[2]=0x54;   // Input Assign
    lp2[3]=0xFF&m->engine();
    lp2[4]=0xFF&(m->talkbackMixerInputDevice(1)>>8);
    lp2[5]=0xFF&m->talkbackMixerInputDevice(1);
    lp2[6]=0xFF&m->surface();
    lp2[7]=0xFF&m->engine();
    lp2[8]=0xFF&(m->greenRoomDevice()>>8);
    lp2[9]=0xFF&m->greenRoomDevice();
    lp_connection->writeMessage(lp2,10);
  }
  else {
    //
    // Pre-Fader Phone Audio to Talkback
    //
    lp2[0]=0x02;
    lp2[1]=0x08;
    lp2[2]=0x54;   // Input Assign
    lp2[3]=0xFF&m->engine();
    lp2[4]=0xFF&(m->talkbackMixerInputDevice(0)>>8);
    lp2[5]=0xFF&m->talkbackMixerInputDevice(0);
    lp2[6]=0xFF&m->surface();
    lp2[7]=0xFF&m->engine();
    lp2[8]=0xFF&(m->phonesPfSourceDevice()>>8);
    lp2[9]=0xFF&m->phonesPfSourceDevice();
    lp_connection->writeMessage(lp2,10);

    //
    // GreenRoom to Talkback
    //
    lp2[0]=0x02;
    lp2[1]=0x08;
    lp2[2]=0x54;   // Input Assign
    lp2[3]=0xFF&m->engine();
    lp2[4]=0xFF&(m->talkbackMixerInputDevice(1)>>8);
    lp2[5]=0xFF&m->talkbackMixerInputDevice(1);
    lp2[6]=0xFF&m->surface();
    lp2[7]=0xFF&m->engine();
    lp2[8]=0xFF&(m->phonesPfSourceDevice()>>8);
    lp2[9]=0xFF&m->phonesPfSourceDevice();
    lp_connection->writeMessage(lp2,10);

    //
    // Clear Cough Button
    //
    lp2[0]=0x02;
    lp2[1]=0x06;
    lp2[2]=0x53;   // Turn OFF
    lp2[3]=0xFF&m->engine();
    lp2[4]=0xFF&(m->device()>>8);
    lp2[5]=0xFF&m->device();
    lp2[6]=0xFF&m->surface();
    lp2[7]=0xFF&m->coughBuss();
    lp_connection->writeMessage(lp2,8);
  }
}


void MainObject::ProcessTurret(Mic *m,int turret,Mic::Function func,bool state,
			       const LPMessage &msg)
{
  char lp2[256];

  switch(func) {
  case Mic::TalkbackButton:
    if(state) {
      //
      // Talkback Lamp On
      //
      for(unsigned i=0;i<m->turrets();i++) {
	lp2[0]=0x02;
	lp2[1]=0x06;
	lp2[2]=0x52;   // Turn ON
	lp2[3]=0xFF&m->turretEngine(Mic::TalkbackLamp,i);
	lp2[4]=0xFF&(m->turretDevice(Mic::TalkbackLamp,i)>>8);
	lp2[5]=0xFF&m->turretDevice(Mic::TalkbackLamp,i);
	lp2[6]=0xFF&m->turretSurface(Mic::TalkbackLamp,i);
	lp2[7]=0xFF&m->turretBuss(Mic::TalkbackLamp,i);
	lp_connection->writeMessage(lp2,8);
      }

      //
      // Route Mic to Talkback Return
      //
      lp2[0]=0x02;
      lp2[1]=0x08;
      lp2[2]=0x54;   // Input Assign
      lp2[3]=0xFF&m->talkbackReturnEngine();
      lp2[4]=0xFF&(m->talkbackReturnDevice()>>8);
      lp2[5]=0xFF&m->talkbackReturnDevice();
      lp2[6]=0xFF&m->surface();
      lp2[7]=0xFF&m->engine();
      lp2[8]=0xFF&(m->device()>>8);
      lp2[9]=0xFF&m->device();
      lp_connection->writeMessage(lp2,10);

      //
      // Turn On Cough Buss
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x52;   // Turn ON
      lp2[3]=0xFF&m->engine();
      lp2[4]=0xFF&(m->device()>>8);
      lp2[5]=0xFF&m->device();
      lp2[6]=0xFF&m->surface();
      lp2[7]=0xFF&m->coughBuss();
      lp_connection->writeMessage(lp2,8);

      //
      // Turn On Monitor
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x52;   // Turn ON
      lp2[3]=0xFF&m->talkbackMonitorEngine();
      lp2[4]=0xFF&(m->talkbackMonitorDevice()>>8);
      lp2[5]=0xFF&m->talkbackMonitorDevice();
      lp2[6]=0xFF&m->talkbackMonitorSurface();
      lp2[7]=0xFF&m->greenroomBuss();
      lp_connection->writeMessage(lp2,8);

      //
      // Turn On Headphone
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x52;   // Turn ON
      lp2[3]=0xFF&m->talkbackHeadphoneEngine();
      lp2[4]=0xFF&(m->talkbackHeadphoneDevice()>>8);
      lp2[5]=0xFF&m->talkbackHeadphoneDevice();
      lp2[6]=0xFF&m->talkbackHeadphoneSurface();
      lp2[7]=0xFF&m->greenroomBuss();
      lp_connection->writeMessage(lp2,8);

      //
      // Turn On Talkback Return
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x52;   // Turn ON
      lp2[3]=0xFF&m->talkbackReturnEngine();
      lp2[4]=0xFF&(m->talkbackReturnDevice()>>8);
      lp2[5]=0xFF&m->talkbackReturnDevice();
      lp2[6]=0xFF&m->talkbackReturnSurface();
      lp2[7]=0xFF&m->onOffBuss();
      lp_connection->writeMessage(lp2,8);
    }
    else {
      //
      // Talkback Lamp Off
      //
      for(unsigned i=0;i<m->turrets();i++) {
	lp2[0]=0x02;
	lp2[1]=0x06;
	lp2[2]=0x53;   // Turn OFF
	lp2[3]=0xFF&m->turretEngine(Mic::TalkbackLamp,i);
	lp2[4]=0xFF&(m->turretDevice(Mic::TalkbackLamp,i)>>8);
	lp2[5]=0xFF&m->turretDevice(Mic::TalkbackLamp,i);
	lp2[6]=0xFF&m->turretSurface(Mic::TalkbackLamp,i);
	lp2[7]=0xFF&m->turretBuss(Mic::TalkbackLamp,i);
	lp_connection->writeMessage(lp2,8);
      }

      //
      // Turn Off Talkback Return
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x53;   // Turn OFF
      lp2[3]=0xFF&m->talkbackReturnEngine();
      lp2[4]=0xFF&(m->talkbackReturnDevice()>>8);
      lp2[5]=0xFF&m->talkbackReturnDevice();
      lp2[6]=0xFF&m->talkbackReturnSurface();
      lp2[7]=0xFF&m->onOffBuss();
      lp_connection->writeMessage(lp2,8);

      //
      // Turn Off Headphone
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x53;   // Turn OFF
      lp2[3]=0xFF&m->talkbackHeadphoneEngine();
      lp2[4]=0xFF&(m->talkbackHeadphoneDevice()>>8);
      lp2[5]=0xFF&m->talkbackHeadphoneDevice();
      lp2[6]=0xFF&m->talkbackHeadphoneSurface();
      lp2[7]=0xFF&m->greenroomBuss();
      lp_connection->writeMessage(lp2,8);

      //
      // Turn Off Monitor
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x53;   // Turn OFF
      lp2[3]=0xFF&m->talkbackMonitorEngine();
      lp2[4]=0xFF&(m->talkbackMonitorDevice()>>8);
      lp2[5]=0xFF&m->talkbackMonitorDevice();
      lp2[6]=0xFF&m->talkbackMonitorSurface();
      lp2[7]=0xFF&m->greenroomBuss();
      lp_connection->writeMessage(lp2,8);

      //
      // Turn Off Cough Buss
      //
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x53;   // Turn OFF
      lp2[3]=0xFF&m->engine();
      lp2[4]=0xFF&(m->device()>>8);
      lp2[5]=0xFF&m->device();
      lp2[6]=0xFF&m->surface();
      lp2[7]=0xFF&m->coughBuss();
      lp_connection->writeMessage(lp2,8);
    }
    break;

  case Mic::OnButton:
  case Mic::OnLamp:
  case Mic::OffButton:
  case Mic::OffLamp:
  case Mic::CoughButton:
  case Mic::CoughLamp:
  case Mic::TalkbackLamp:
  case Mic::LastFunction:
    break;
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
