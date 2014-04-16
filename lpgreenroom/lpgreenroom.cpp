// lpgreenroom.cpp
//
// lpgreenroom(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpgreenroom.cpp,v 1.1 2013/11/21 22:31:45 cvs Exp $
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

#include "lpgreenroom.h"

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
  int count=0;
  Room *r=NULL;

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpgreenroom",LPGREENROOM_USAGE);
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
    openlog("lpgreenroom",LOG_PERROR,LOG_USER);
  }
  else {
    openlog("lpgreenroom",0,LOG_USER);
  }

  //
  // Read Configuration
  //
  lp_profile=new LPProfile();
  if(!lp_profile->setSource(LPGREENROOM_CONF_FILE)) {
    syslog(LOG_ERR,"unable to open configuration file at \"%s\"",
	   LPGREENROOM_CONF_FILE);
    exit(256);
  }
  r=new Room(lp_profile);
  while(r->load(count)) {
    lp_rooms.push_back(r);
    count++;
    r=new Room(lp_profile);
  }
  delete r;

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
  connect(lp_connection,SIGNAL(watchdogStateChanged(bool)),
	  this,SLOT(serverWatchdogStateChangedData(bool)));

  lp_connection->
    connectToHost(lp_profile->stringValue("Server","Hostname","localhost"),
		  lp_profile->intValue("Server","Port",
				       LPCORE_CONNECTION_TCP_PORT),
		  lp_profile->stringValue("Server","ProfileName","profile"),
		  lp_profile->stringValue("Server","Password"));
}


void MainObject::serverMessageReceivedData(const LPMessage &msg)
{
  Room *r=NULL;
  int device=0;
  uint8_t chan=0;
  char lp2[1024];
  int mix_minus=-1;

  switch(0xFF&msg[2]) {
  case 0x52:   // Turn ON
    if(msg.size()==8) {
      device=((0xFF&msg[4])<<8)+(0xFF&msg[5]);
      if((r=GetRoom(0xFF&msg[3],0xFF&msg[6],device))!=NULL) {
	if((0xFF&msg[7])==0) {   // Bus 0
	  if((mix_minus=r->mixMinusByDevice(device))>0) {
	    //printf("ON\n");

	    //
	    // Set Mix-Minus to Program Buss
	    //
	    lp2[0]=0x02;
	    lp2[1]=0x08;
	    lp2[2]=0x5A;   // Effects Level
	    lp2[3]=0xFF&r->engine();
	    lp2[4]=msg[4];
	    lp2[5]=msg[5];
	    lp2[6]=0xFF&r->surface();
	    lp2[7]=0x14;   // Mix Minus Effects Type
	    lp2[8]=0xFF&mix_minus;
	    lp2[9]=0xFF&r->programBuss();
	    lp_connection->writeMessage(lp2,10);

	    //
	    // Turn Off Greenroom Buss
	    //
	    lp2[0]=0x02;
	    lp2[1]=0x06;
	    lp2[2]=0x53;   // Turn OFF
	    lp2[3]=0xFF&r->engine();
	    lp2[4]=msg[4];
	    lp2[5]=msg[5];
	    lp2[6]=0xFF&r->surface();
	    lp2[7]=0xFF&r->greenroomBuss();
	    lp_connection->writeMessage(lp2,8);
	  }
	  //
	  // Update State
	  //
	  r->setBuss0StateByDevice(device,true);
	}
      }
    }
    break;

  case 0x53:   // Turn OFF
    if(msg.size()==8) {
      device=((0xFF&msg[4])<<8)+(0xFF&msg[5]);
      if((r=GetRoom(0xFF&msg[3],0xFF&msg[6],device))!=NULL) {
	if((0xFF&msg[7])==0) {   // Bus 0
	  if((mix_minus=r->mixMinusByDevice(device))>0) {
	    //printf("OFF\n");

	    //
	    // Set Mix-Minus to Greenroom Buss
	    //
	    lp2[0]=0x02;
	    lp2[1]=0x08;
	    lp2[2]=0x5A;   // Effects Level
	    lp2[3]=0xFF&r->engine();
	    lp2[4]=msg[4];
	    lp2[5]=msg[5];
	    lp2[6]=0xFF&r->surface();
	    lp2[7]=0x14;   // Mix Minus Effects Type
	    lp2[8]=0xFF&mix_minus;
	    lp2[9]=0xFF&r->greenroomBuss();
	    lp_connection->writeMessage(lp2,10);

	    //
	    // Turn On Greenroom Buss
	    //
	    lp2[0]=0x02;
	    lp2[1]=0x06;
	    lp2[2]=0x52;   // Turn ON
	    lp2[3]=0xFF&r->engine();
	    lp2[4]=msg[4];
	    lp2[5]=msg[5];
	    lp2[6]=0xFF&r->surface();
	    lp2[7]=0xFF&r->greenroomBuss();
	    lp_connection->writeMessage(lp2,8);
	  }
	  //
	  // Update State
	  //
	  r->setBuss0StateByDevice(device,false);
	}
      }
    }
    break;

  case 0x54:   // Input Assign
    if(msg.size()==10) {
      device=((0xFF&msg[8])<<8)+(0xFF&msg[9]);
      chan=msg[5];
      if((r=GetRoom(0xFF&msg[3],0xFF&msg[6]))!=NULL) {
	for(unsigned i=0;i<r->devices();i++) {
	  if(r->deviceNumber(i)==r->deviceNumberByChannel(chan)) {
	    if(!r->buss0StateByDevice(device)) {
	      // printf("UNROUTED 0x%04X from %u\n",
	      //     r->deviceNumber(i),0xFF&chan);

	      //
	      // Turn Off Greenroom Buss
	      //
	      lp2[0]=0x02;
	      lp2[1]=0x06;
	      lp2[2]=0x53;   // Turn OFF
	      lp2[3]=0xFF&r->engine();
	      lp2[4]=0x00;
	      lp2[5]=msg[5];
	      lp2[6]=0xFF&r->surface();
	      lp2[7]=0xFF&r->greenroomBuss();
	      lp_connection->writeMessage(lp2,8);
	    }
	  }
	}
	r->setDeviceNumberByChannel(0xFF&msg[5],device);
      }
      if((r=GetRoom(0xFF&msg[3],0xFF&msg[6],device))!=NULL) {
	if(!r->buss0StateByDevice(device)) {
	  //printf("ROUTED 0x%04X to %u\n",device,0xFF&chan);

	  //
	  // Turn Off Greenroom Buss
	  //
	  lp2[0]=0x02;
	  lp2[1]=0x06;
	  lp2[2]=0x52;   // Turn ON
	  lp2[3]=0xFF&r->engine();
	  lp2[4]=0x00;
	  lp2[5]=msg[5];
	  lp2[6]=0xFF&r->surface();
	  lp2[7]=0xFF&r->greenroomBuss();
	  lp_connection->writeMessage(lp2,8);
	}
	r->setDeviceNumberByChannel(chan,device);
      }
    }
    break;
  }
}


void MainObject::serverWatchdogStateChangedData(bool state)
{
  char lp2[1024];

  if(state) {
    //
    // Request Current Source Assignments
    //
    for(unsigned i=0;i<lp_rooms.size();i++) {
      lp2[0]=0x02;
      lp2[1]=0x06;
      lp2[2]=0x09;   // Read Parameter
      lp2[3]=0xFF&lp_rooms[i]->engine();
      lp2[4]=0x00;
      lp2[5]=0x00;
      lp2[6]=0xFF&lp_rooms[i]->surface();
      lp2[7]=0x54;   // Input Assign
      for(uint8_t j=0;j<LPCORE_MAX_CHANNELS;j++) {
	if(LPConfig::surfaceByChannel(j)==lp_rooms[i]->surface()) {
	  lp2[5]=j;
	  lp_connection->writeMessage(lp2,8);
	}
      }
    }

    syslog(LOG_DEBUG,"connection to LP2 server established");
  }
  else {
    syslog(LOG_DEBUG,"connection to LP2 server dropped");
  }
}


Room *MainObject::GetRoom(int engine,int surface) const
{
  Room *r=NULL;

  for(unsigned i=0;i<lp_rooms.size();i++) {
    if(lp_rooms[i]->belongsTo(engine,surface)) {
      r=lp_rooms[i];
    }
  }
  return r;
}


Room *MainObject::GetRoom(int engine,int surface,int device) const
{
  Room *r=NULL;

  for(unsigned i=0;i<lp_rooms.size();i++) {
    if(lp_rooms[i]->belongsTo(engine,surface,device)) {
      r=lp_rooms[i];
    }
  }
  return r;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
