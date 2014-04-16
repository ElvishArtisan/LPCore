// lpcored.cpp
//
// lpcored(8) LP Routing Daemon.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpcored.cpp,v 1.33 2013/12/31 19:31:06 cvs Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <QtGui/QApplication>

#include <lpcore/lpcmdswitch.h>
#include <lpcore/lpprofile.h>

#include "enginefactory.h"
#include "lpcored.h"

MainObject *main_object=NULL;

void SigHandler(int signo)
{
  switch(signo) {
  case SIGTERM:
  case SIGINT:
    for(unsigned i=0;i<main_object->main_app_hosts.size();i++) {
      main_object->main_app_hosts[i]->kill();
    }
    unlink(LPCORED_PID_FILE);
    exit(0);
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  FILE *f;
  bool debug=false;
  main_applications_started=false;
  main_next_connection_id=1;
  main_object=this;

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpcored",LPCORED_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="-d") {
      debug=true;
    }
  }
  delete cmd;

  //
  // Open Syslog
  //
  if(debug) {
    openlog("lpcored",LOG_PERROR,LOG_USER);
  }
  else {
    openlog("lpcored",0,LOG_USER);
  }

  //
  // Load Configuration
  //
  main_config=new LPConfig();
  if(!main_config->load()) {
    syslog(LOG_ERR,"unable to open configuration file \"%s\"",LPCORE_CONF_FILE);
    exit(256);
  }
  if(main_config->engines()==0) {
    syslog(LOG_ERR,"no engines configured in \"%s\", exiting",LPCORE_CONF_FILE);
    exit(256);
  }

  //
  // Start Internal Application Timer
  //
  main_application_timer=new QTimer(this);
  main_application_timer->setSingleShot(true);
  connect(main_application_timer,SIGNAL(timeout()),
	  this,SLOT(applicationStartData()));
  main_application_timer->start(LPCORED_APPLICATION_DELAY);

  //
  // Start Engine Drivers
  //
  for(unsigned i=0;i<main_config->engines();i++) {
    main_engines[main_config->engineNumber(i)]=
      EngineFactory(main_config,i,this);
    connect(main_engines[main_config->engineNumber(i)],
	    SIGNAL(messageReceived(int,const QByteArray &)),
	    this,
	    SLOT(engineMessageReceivedData(int,const QByteArray &)));
    connect(main_engines[main_config->engineNumber(i)],
	    SIGNAL(resetFinished(int,bool)),
	    this,SLOT(engineResetData(int,bool)));
    if(main_engines[main_config->engineNumber(i)]->open()) {
      syslog(LOG_DEBUG,
	     "opened connection to engine %d with device \"%s\"",
	     main_config->engineNumber(i),
	     (const char *)main_config->engineSerialDevice(i).toUtf8());
    }
    else {
      syslog(LOG_WARNING,
	     "unable to open connection to engine %d with device \"%s\"",
	     main_config->engineNumber(i),
	     (const char *)main_config->engineSerialDevice(i).toUtf8());
    }
  }

  //
  // Client Server
  //
  main_garbage_timer=new QTimer(this);
  main_garbage_timer->setSingleShot(true);
  connect(main_garbage_timer,SIGNAL(timeout()),
	  this,SLOT(garbageCollectionData()));
  main_server=new QTcpServer(this);
  connect(main_server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));
  main_server->listen(QHostAddress::Any,LPCORE_CONNECTION_TCP_PORT);

  //
  // Detach
  //
  if(!debug) {
    daemon(0,0);
  }

  //
  // Log PID
  //
  if((f=fopen(LPCORED_PID_FILE,"w"))!=NULL) {
    fprintf(f,"%d",getpid());
    fclose(f);
  }

  //
  // Reset Engines
  //
  for(std::map<int,Engine *>::const_iterator it=main_engines.begin();
      it!=main_engines.end();it++) {
    it->second->reset();
  }

  //
  // Set Signals
  //
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);
}


void MainObject::engineMessageReceivedData(int engine,const QByteArray &msg)
{
  bool ok=false;
  char lp2[256];
  SourceDevice *dev;
  Engine *e=main_engines[engine];

  //printf("%d: %s\n",engine,(const char *)LPParser::dump(msg).toAscii());

  switch(0xFF&msg[2]) {
  case 0x81:   // Setup Table Transfer
    if(msg.size()==5) {
      //printf("%s\n",(const char *)LPParser::dump(msg).toAscii());
      ok=true;
    }
    break;

  case 0x82:   // Transfer Table
    if(msg.size()>=8) {
      //printf("%s\n",(const char *)LPParser::dump(msg).toAscii());
      ok=true;
    }
    break;

  case 0x84:   // Report Status / Ping Response
    if(msg.size()==7) {
      if((0xFF&msg[3])==0x01) {  // Audio Engine
	//syslog(LOG_DEBUG,"engine %d reports AES software v%X.%X",
	//       engine,0x0F&(msg[4]>>4),0x0F&msg[4]);
      }
      ok=true;
    }
    break;

  case 0x85:   // System Command
    if(msg.size()>=5) {
      if((0xFF&msg[3])==0) { // Current Status
	switch(0xFF&msg[4]) {
	case 0xA5:
	  syslog(LOG_DEBUG,"status upload starting from engine %d",engine);
	  break;

	case 0xA6:
	  syslog(LOG_DEBUG,"status upload finished from engine %d",engine);
	  break;

	case 0xA7:
	  syslog(LOG_DEBUG,"clear all control surface settings from engine %d",
		 engine);
	  break;

	case 0xA8:
	  syslog(LOG_DEBUG,"ping received from engine %d",engine);
	  break;

	case 0xAA:
	  syslog(LOG_DEBUG,"power on from engine %d",engine);
	  break;

	default:
	  syslog(LOG_NOTICE,"unknown status byte received from engine %d",
		 engine);
	  break;
	}
      }
      ok=true;
    }
    break;

  case 0xA2:   // Set Relay Pulse
    if((msg.size()==5)||msg.size()==6) {
      lp2[0]=0x02;
      lp2[1]=0x07;
      lp2[2]=0x51;
      lp2[3]=0xFF&engine;
      lp2[4]=0x00;
      lp2[5]=msg[3];
      lp2[6]=0xFF&LPConfig::surfaceByChannel(msg[3]);
      lp2[7]=msg[4];
      if(msg.size()==5) {
	lp2[8]=0;
      }
      else {
	lp2[8]=msg[5];
      }
      SendClientMessage(lp2,9);
      ok=true;
    }
    break;

  case 0xAD:   // Set Fader Level
    if(msg.size()==5) {
      e->setSourceFaderLevel(msg[3],0xFF&msg[4]);
      dev=e->sourceDevice(msg[3]);
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x56;
      lp2[3]=0xFF&engine;
      lp2[4]=(0xFF&(dev->device()>>8));
      lp2[5]=0xFF&dev->device();
      lp2[6]=0xFF&dev->surface();
      lp2[7]=msg[4];
      SendClientMessage(lp2,8);
      ok=true;
    }
    break;

  case 0xAE:   // Set Effects Level
    if((msg.size()>=6)&&(msg.size()<=8)) {
      ok=true;
    }
    break;

  case 0xB2:   // Turn ON
    if(msg.size()==5) {
      e->setSourceBusState(0xFF&msg[3],0xFF&msg[4],true);
      dev=e->sourceDevice(msg[3]);
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x52;
      lp2[3]=0xFF&engine;
      lp2[4]=(0xFF&(dev->device()>>8));
      lp2[5]=0xFF&dev->device();
      lp2[6]=0xFF&dev->surface();
      lp2[7]=msg[4];
      SendClientMessage(lp2,8);
      ok=true;
    }
    break;

  case 0xB3:   // Turn OFF
    if(msg.size()==5) {
      e->setSourceBusState(0xFF&msg[3],0xFF&msg[4],false);
      dev=e->sourceDevice(msg[3]);
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x53;
      lp2[3]=0xFF&engine;
      lp2[4]=(0xFF&(dev->device()>>8));
      lp2[5]=0xFF&dev->device();
      lp2[6]=0xFF&dev->surface();
      lp2[7]=msg[4];
      SendClientMessage(lp2,8);
      ok=true;
    }
    break;

  case 0xB4:   // Set Input Assignment
    if(msg.size()==6) {
      e->setSourceDevice(0xFF&msg[3],((0xFF&msg[4])<<8)+(0xFF&msg[5]));
      lp2[0]=0x02;
      lp2[1]=0x08;
      lp2[2]=0x54;
      lp2[3]=0xFF&engine;  // Destination Engine
      lp2[4]=0x00;  // Destination Channel
      lp2[5]=msg[3];
      lp2[6]=0xFF&LPConfig::surfaceByChannel(msg[3]);
      lp2[7]=0xFF&engine;   // Source Engine
      lp2[8]=msg[4];  // Source Device
      lp2[9]=msg[5];
      SendClientMessage(lp2,10);
      ok=true;
    }
    break;

  case 0xB5:   // Set Input Mode
    if(msg.size()==5) {
      e->setSourceMode(0xFF&msg[3],0xFF&msg[4]);
      dev=e->sourceDevice(msg[3]);
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x55;
      lp2[3]=0xFF&engine;
      lp2[4]=(0xFF&(dev->device()>>8));
      lp2[5]=0xFF&dev->device();
      lp2[6]=0xFF&dev->surface();
      lp2[7]=msg[4];
      SendClientMessage(lp2,8);
      ok=true;
    }
    break;

  case 0xC1:   // Set Meter Level
    ok=true;
    break;

  case 0xC2:   // Set Meter Group
    ok=true;
    break;

  default:
    if(msg.size()==(msg[1]+2)) {
      syslog(LOG_DEBUG,"received unrecognized LP1 message from engine %d: %s",
	     engine,(const char *)LPParser::dump(msg).toAscii());
      ok=true;
    }
    break;
  }

  if(!ok) {
    syslog(LOG_WARNING,
	   "received unrecognized/corrupt message from engine %d: %s",
	   engine,(const char *)LPParser::dump(msg).toAscii());
  }
}


void MainObject::engineResetData(int engine,bool state)
{
  bool all=true;

  if(state) {
    syslog(LOG_DEBUG,"engine %d reset complete",engine);
    if(main_application_timer->isActive()) {
      for(std::map<int,Engine *>::const_iterator it=main_engines.begin();
	  it!=main_engines.end();it++) {
	if(!it->second->isReset()) {
	  all=false;
	}
      }
      if(all) {
	main_application_timer->stop();
	applicationStartData();
      }
    }
  }
  else {
    syslog(LOG_WARNING,"engine %d failed to reset",engine);
  }
}


void MainObject::connectionMessageReceivedData(int id,const LPMessage &msg)
{
  bool ok=false;
  char lp1[256];
  char lp2[256];
  int chan;
  Engine *e=NULL;
  Connection *conn=main_connections[id];
  SourceDevice *dev;

  if(!conn->authenticated()) {
    switch(0xFF&msg[2]) {
    case 0xF9:   // Login
      if((msg.size()==36)&&((0xFF&msg[3])==LPCORE_VGUEST_ID)) {
	conn->setAuthenticated(true);
	lp2[0]=0x02;
	lp2[1]=0xFF&(3+main_engines.size());
	lp2[2]=0xF0;   // Connect Status
	lp2[3]=0x0A;   // Valid Connection
	lp2[4]=0xFF&main_engines.size();
	for(unsigned i=0;i<main_engines.size();i++) {
	  lp2[5+i]=0x01;
	}
	conn->writeOverride(lp2,5+main_engines.size());
	syslog(LOG_INFO,"logged in user \"%s\" from %s:%d\n",
	       (const char *)msg.mid(4,16).trimmed().data(),
	       (const char *)conn->peerAddress().toString().toAscii(),
	       0xFFFF&conn->peerPort());
      }
      else {
	syslog(LOG_WARNING,"rejected login attempt from %s:%d",
	       (const char *)conn->peerAddress().toString().toAscii(),
	       0xFFFF&conn->peerPort());
	conn->close();
      }
      break;

    default:
      SendLoginQuery(id);
      break;
    }
    return;
  }

  switch(0xFF&msg[2]) {
  case 0x08:      // Ping
    if(msg.size()==7) {
      lp1[0]=0x02;
      lp1[1]=0x01;
      lp1[2]=0x04;
      SendToEngine(1,lp1,3);
      ok=true;
    }
    break;

  case 0x09:    // Read Parameter
    if((msg.size()>=8)&&(msg.size()<=9)) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      dev=e->sourceDevice(((0xFF&msg[4])<<8)+(0xFF&msg[5]),0xFF&msg[6]);
      if(dev!=NULL) {
	switch(0xFF&msg[7]) {
	case 0x52:  // Turn ON
	case 0x53:  // Turn OFF
	  lp2[0]=0x02;
	  lp2[1]=0x06;
	  if(dev->busState(0xFF&msg[8])) {
	    lp2[2]=0x52;
	  }
	  else {
	    lp2[2]=0x53;
	  }
	  lp2[3]=msg[3];
	  lp2[4]=msg[4];
	  lp2[5]=msg[5];
	  lp2[6]=msg[6];
	  lp2[7]=msg[8];
	  conn->write(lp2,8);
	  ok=true;
	  break;

	case 0x54:  // Input Assign
	  dev=e->sourceDevice(msg[5]);
	  lp2[0]=0x02;
	  lp2[1]=0x08;
	  lp2[2]=0x54;
	  lp2[3]=msg[3];
	  lp2[4]=msg[4];
	  lp2[5]=msg[5];
	  lp2[6]=msg[6];
	  lp2[7]=msg[3];   // FIXME: Is this always the same engine?
	  lp2[8]=0xFF&(dev->device()>>8);
	  lp2[9]=0xFF&dev->device();
	  conn->write(lp2,10);
	  ok=true;
	  break;

	case 0x55:  // Input Mode
	  lp2[0]=0x02;
	  lp2[1]=0x06;
	  lp2[2]=0x55;
	  lp2[3]=msg[3];
	  lp2[4]=msg[4];
	  lp2[5]=msg[5];
	  lp2[6]=msg[6];
	  lp2[7]=0xFF&dev->mode();
	  conn->write(lp2,8);
	  ok=true;
	  break;

	case 0x56:  // Fader Level
	  lp2[0]=0x02;
	  lp2[1]=0x06;
	  lp2[2]=0x56;
	  lp2[3]=msg[3];
	  lp2[4]=msg[4];
	  lp2[5]=msg[5];
	  lp2[6]=msg[6];
	  lp2[7]=0xFF&dev->faderLevel();
	  conn->write(lp2,8);
	  ok=true;
	  break;
	}
      }
    }
    break;

  case 0x51:    // Momentary Relay Closure
    if(msg.size()==9) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      lp1[0]=0x02;
      lp1[1]=0x04;
      lp1[2]=0xA2;
      lp1[3]=msg[5];   // Channel
      lp1[4]=msg[7];   // Relay
      if((0xFF&msg[8])==0) {  // Default duration is 500 mS
	lp1[5]=10;   // Duration
      }
      else {
	lp1[5]=msg[8];   // Duration
      }
      SendToEngine(0xFF&msg[3],lp1,6);
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0x52:    // Turn ON
    if(msg.size()==8) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      if((chan=GetDeviceChannel(msg[3],msg[4],msg[5],msg[6]))<0) {
	ok=true;
	break;
      }
      lp1[0]=0x02;
      lp1[1]=0x03;
      lp1[2]=0xB2;
      lp1[3]=0xFF&chan;
      lp1[4]=msg[7];
      SendToEngine(0xFF&msg[3],lp1,5);
      e->setSourceBusState(chan,0xFF&msg[7],true);
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0x53:    // Turn OFF
    if(msg.size()==8) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      if((chan=GetDeviceChannel(msg[3],msg[4],msg[5],msg[6]))<0) {
	ok=true;
	break;
      }
      lp1[0]=0x02;
      lp1[1]=0x03;
      lp1[2]=0xB3;
      lp1[3]=0xFF&chan;
      lp1[4]=msg[7];
      SendToEngine(0xFF&msg[3],lp1,5);
      e->setSourceBusState(chan,0xFF&msg[7],false);
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0x54:    // Set Input Source
    if(msg.size()==10) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      lp1[0]=0x02;
      lp1[1]=0x04;
      lp1[2]=0xB4;
      lp1[3]=msg[5];  // Destination Channel
      lp1[4]=msg[8];  // Source Device
      lp1[5]=msg[9];
      SendToEngine(0xFF&msg[3],lp1,6);
      e->setSourceDevice(0xFF&msg[5],((0xFF&msg[8])<<8)+(0xFF&msg[9]));
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0x55:    // Set Input Mode
    if(msg.size()==8) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      if((chan=GetDeviceChannel(msg[3],msg[4],msg[5],msg[6]))<0) {
	ok=true;
	break;
      }
      lp1[0]=0x02;
      lp1[1]=0x03;
      lp1[2]=0xB5;
      lp1[3]=0xFF&chan;
      lp1[4]=msg[7];  // Source Device
      SendToEngine(0xFF&msg[3],lp1,5);
      e->setSourceMode(chan,0xFF&msg[7]);
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0x56:    // Set Fader Level
    if(msg.size()==8) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      if((chan=GetDeviceChannel(msg[3],msg[4],msg[5],msg[6]))<0) {
	ok=true;
	break;
      }
      lp1[0]=0x02;
      lp1[1]=0x03;
      lp1[2]=0xAD;
      lp1[3]=0xFF&chan;
      lp1[4]=msg[7];
      SendToEngine(0xFF&msg[3],lp1,5);
      e->setSourceFaderLevel(chan,0xFF&msg[7]);
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0x5A:    // Set Effects Level
    if(msg.size()==10) {  // FIXME: We only support setting mix-minus buss here!
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      if((chan=GetDeviceChannel(msg[3],msg[4],msg[5],msg[6]))<0) {
	ok=true;
	break;
      }
      lp1[0]=0x02;
      lp1[1]=0x04;
      lp1[2]=0xAE;
      lp1[3]=0xFF&(msg[8]+0x0A);
      lp1[4]=0x14;    // Mix Minus
      lp1[5]=msg[9];  // Buss Number (1-6, 9-12)
      SendToEngine(0xFF&msg[3],lp1,6);
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0x5C:    // Write Text
    if(msg.size()>=8) {
      if((e=main_engines[0xFF&msg[3]])==NULL) {
	return;
      }
      lp1[0]=0x02;
      lp1[1]=(0xFF&msg[1])-4;
      lp1[2]=0xB6;
      lp1[3]=msg[5];   // Channel
      lp1[4]=msg[7];   // Line
      lp1[5]=msg[8];   // Starting Column
      memcpy(lp1+6,msg.data()+10,msg.size()-8);   // Text
      SendToEngine(0xFF&msg[3],lp1,(0xFF&lp1[1])+2);
      SendClientGlobalMessage(msg);
      ok=true;
    }
    break;

  case 0xF9:   // Login
    ok=true;
    break;

  default:
    if(msg.size()==(msg[1]+2)) {
      syslog(LOG_DEBUG,"received unrecognized LP2 message from %s:%d: %s",
	     (const char *)conn->
	     peerAddress().toString().toAscii(),
	     0xFFFF&conn->peerPort(),
	     (const char *)LPParser::dump(msg).toAscii());
      ok=true;
    }
    break;
  }

  if(!ok) {
    syslog(LOG_WARNING,"received unrecognized/corrupt message from %s:%d: %s",
	   (const char *)conn->
	   peerAddress().toString().toAscii(),
	   0xFFFF&conn->peerPort(),
	   (const char *)LPParser::dump(msg).toAscii());
  }
}


void MainObject::newConnectionData()
{
  main_connections[main_next_connection_id]=
    new Connection(main_next_connection_id,main_server->nextPendingConnection(),
		   this);
  connect(main_connections[main_next_connection_id],
	  SIGNAL(messageReceived(int,const LPMessage &)),
	  this,
	  SLOT(connectionMessageReceivedData(int,const LPMessage &)));
  connect(main_connections[main_next_connection_id],
	  SIGNAL(fullStateRequested(int)),
	  this,SLOT(sendFullState(int)));
  SendLoginQuery(main_next_connection_id);
  main_next_connection_id++;
}


void MainObject::garbageCollectionData()
{
  for(unsigned i=0;i<main_dead_connections.size();i++) {
    delete main_connections[main_dead_connections[i]];
    main_connections.erase(main_dead_connections[i]);
  }
  main_dead_connections.clear();
}


void MainObject::applicationStartData()
{
  if(!main_applications_started) {
    for(std::map<int,Engine *>::const_iterator it=main_engines.begin();
	it!=main_engines.end();it++) {
      if(!it->second->isReset()) {
	syslog(LOG_WARNING,"engine %d on \"%s\" failed to respond to reset",
	       it->first,
	       (const char *)main_config->engineSerialDevice(it->first-1).
	       toAscii());
      }
    }
    if(main_config->applications()>0) {
      syslog(LOG_DEBUG,"starting internal applications");
      for(unsigned i=0;i<main_config->applications();i++) {
	main_app_hosts.push_back(new AppHost(i,main_config,this));
      }
    }
    main_applications_started=true;
  }
}


void MainObject::sendFullState(int id)
{
  SourceDevice *sd=NULL;
  char lp2[256];

  for(std::map<int,Engine *>::const_iterator it=main_engines.begin();
      it!=main_engines.end();it++) {
    if(it->second==NULL) {
      syslog(LOG_WARNING,"attempted to reference non-existing engine %d",
	     it->first);
      continue;
    }
    for(int i=0;i<LPCORE_MAX_CHANNELS;i++) {
      if((sd=it->second->sourceDevice(i))!=NULL) {
	if(sd->surface()>0) {
	  if((sd->device()>0)&&(sd->device()!=i)) {   // Input Assignments
	    lp2[0]=0x02;
	    lp2[1]=0x08;
	    lp2[2]=0x54;
	    lp2[3]=0xFF&it->first;
	    lp2[4]=0x00;
	    lp2[5]=0xFF&i;
	    lp2[6]=0xFF&sd->surface();
	    lp2[7]=0xFF&it->first;
	    lp2[8]=0xFF&(sd->device()>>8);
	    lp2[9]=0xFF&sd->device();
	    main_connections[id]->write(lp2,10);

	    lp2[0]=0x02;     // Fader Levels
	    lp2[1]=0x06;
	    lp2[2]=0x56;
	    lp2[3]=0xFF&it->first;
	    lp2[4]=0xFF&(sd->device()>>8);
	    lp2[5]=0xFF&sd->device();
	    lp2[6]=0xFF&sd->surface();
	    lp2[7]=0xFF&sd->faderLevel();
	    main_connections[id]->write(lp2,8);

	    lp2[0]=0x02;     // Input Modes
	    lp2[1]=0x06;
	    lp2[2]=0x55;
	    lp2[3]=0xFF&it->first;
	    lp2[4]=0xFF&(sd->device()>>8);
	    lp2[5]=0xFF&sd->device();
	    lp2[6]=0xFF&sd->surface();
	    lp2[7]=0xFF&sd->mode();
	    main_connections[id]->write(lp2,8);
	  }
	}

	for(int j=0;j<LPCORE_MAX_BUSSES;j++) {   // Buss States
	  if(sd->busState(j)) {
	    lp2[0]=0x02;
	    lp2[1]=0x06;
	    lp2[2]=0x52;
	    lp2[3]=0xFF&it->first;
	    lp2[4]=0xFF&(sd->device()>>8);
	    lp2[5]=0xFF&sd->device();
	    lp2[6]=0xFF&sd->surface();
	    lp2[7]=0xFF&j;
	    main_connections[id]->write(lp2,8);
	  }
	}
      }
    }
  }
}


void MainObject::SendLoginQuery(int id)
{
  char msg[3]={0x02,0x01,0xF9};

  main_connections[id]->writeOverride(msg,3);
}



int MainObject::GetDeviceChannel(uint8_t engine,uint8_t dev_byte1,
				 uint8_t dev_byte2,uint8_t surface)
{
  int chan=dev_byte2;
  int dev=((0xFF&dev_byte1)<<8)+(0xFF&dev_byte2);
  if(dev>=0x0100) {
    if((main_engines[engine]==NULL)||
       ((chan=main_engines[engine]->destinationChannel(dev,surface))<0)) {
      return -1;
    }
  }
  return chan;
}


void MainObject::SendToEngine(int engine,const QByteArray &data)
{
  if(main_engines[engine]!=NULL) {
    main_engines[engine]->write(data);
  }
  else {
    syslog(LOG_NOTICE,
	   "attempted to send message %s to non-existent engine %d",
	   (const char *)LPParser::dump(data).toAscii(),engine);
  }
}


void MainObject::SendToEngine(int engine,const char *data,int len)
{
  SendToEngine(engine,QByteArray(data,len));
}


void MainObject::SendClientMessage(const QByteArray &msg,int except_id)
{
  for(std::map<int,Connection *>::const_iterator it=main_connections.begin();
      it!=main_connections.end();it++) {
    if(it->first!=except_id) {
      if(it->second->write(msg)<0) {
	main_dead_connections.push_back(it->first);
	main_garbage_timer->start(0);
      }
    }
  }
}


void MainObject::SendClientMessage(const char *msg,int len,int except_id)
{
  SendClientMessage(QByteArray(msg,len),except_id);
}


void MainObject::SendClientGlobalMessage(const QByteArray &msg)
{
  for(std::map<int,Connection *>::const_iterator it=main_connections.begin();
      it!=main_connections.end();it++) {
    if(!it->second->isIsolated()) {
      if(it->second->write(msg)<0) {
	main_dead_connections.push_back(it->first);
	main_garbage_timer->start(0);
      }
    }
  }
}


void MainObject::SendClientGlobalMessage(const char *msg,int len)
{
  SendClientGlobalMessage(QByteArray(msg,len));
}


Engine *MainObject::GetEngine(int engine,Connection *conn,const QByteArray &msg)
{
  Engine *e=main_engines[engine];

  if(e==NULL) {
    syslog(LOG_WARNING,"nonexistent engine %d referenced from %s:%d [%s]",
	   engine,(const char *)conn->peerAddress().toString().toAscii(),
	   0xFFFF&conn->peerPort(),
	   (const char *)LPMessage::dump(msg).toAscii());
  }
  return e;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
