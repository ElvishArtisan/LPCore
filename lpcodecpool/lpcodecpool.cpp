// lpcodecpool.cpp
//
// lpcodecpool(1) LP Application
//
//   (C) Copyright 2013-2014 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdio.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include <QtGui/QApplication>
#include <QtNetwork/QTcpServer>

#include <lpcore/lpcmdswitch.h>
#include <lpcore/lpconfig.h>

#include "lpcodecpool.h"

void SigHandler(int signum)
{
  switch(signum) {
  case SIGHUP:
  case SIGINT:
  case SIGTERM:
    unlink(LPCODECPOOL_PID_FILE);
    exit(0);
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  FILE *f=NULL;
  bool debug=false;
  QString hostname="localhost";

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpcodecpool",LPCODECPOOL_USAGE);
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
    openlog("lpcodecpool",LOG_PERROR,LOG_USER);
  }
  else {
    openlog("lpcodecpool",0,LOG_USER);
  }

  //
  // Read Configuration
  //
  lp_profile=new LPProfile();
  if(!lp_profile->setSource(LPCODECPOOL_CONF_FILE)) {
    syslog(LOG_ERR,"unable to open configuration file at \"%s\"",
	   LPCODECPOOL_CONF_FILE);
    exit(256);
  }

  //
  // Devices
  //
  lp_devices=new Devices(lp_profile);
  if(!lp_devices->open()) {
    exit(256);
  }
  connect(lp_devices->inputSwitcher(),SIGNAL(crosspointChanged(int,int,int)),
	  this,SLOT(inputCrosspointChangedData(int,int,int)));
  connect(lp_devices->outputSwitcher(),SIGNAL(crosspointChanged(int,int,int)),
	  this,SLOT(outputCrosspointChangedData(int,int,int)));
  connect(lp_devices->gpioDevice(),SIGNAL(gpiChanged(int,int,bool)),
	  this,SLOT(gpiChangedData(int,int,bool)));

  //
  // Codecs
  //
  lp_codecs=new Codecs(lp_profile);
  if(lp_codecs->codecQuantity()==0) {
    syslog(LOG_ERR,"no configured codecs found");
    exit(256);
  }

  //
  // Rooms
  //
  lp_rooms=new Rooms(lp_profile);
  if(lp_rooms->roomQuantity()==0) {
    syslog(LOG_ERR,"no configured rooms found");
    exit(256);
  }
  for(unsigned i=0;i<lp_rooms->roomQuantity();i++) {
    if(lp_rooms->portQuantity(i)==0) {
      syslog(LOG_ERR,"no configured ports found in room \"%s\" [Room%u]",
	     (const char *)lp_rooms->name(i).toUtf8(),i);
      exit(256);
    }
  }

  //
  // Panel Command Server
  //
  QTcpServer *server=new QTcpServer(this);
  server->listen(QHostAddress::Any,LPCODECPOOL_PANEL_TCP_PORT);
  std::map<int,QString> cmds;
  std::map<int,int> upper_limits;
  std::map<int,int> lower_limits;

  cmds[MainObject::DC]="DC";
  upper_limits[MainObject::DC]=0;
  lower_limits[MainObject::DC]=0;

  cmds[MainObject::GC]="GC";
  upper_limits[MainObject::GC]=0;
  lower_limits[MainObject::GC]=0;

  cmds[MainObject::SX]="SX";
  upper_limits[MainObject::SX]=2;
  lower_limits[MainObject::SX]=2;

  lp_server=
    new LPStreamCmdServer(cmds,upper_limits,lower_limits,server,this);
  connect(lp_server,SIGNAL(commandReceived(int,int,const QStringList &)),
	  this,SLOT(commandReceivedData(int,int,const QStringList &)));
  connect(lp_server,SIGNAL(newConnection(int,const QHostAddress &,uint16_t)),
	  this,SLOT(newConnectionData(int,const QHostAddress &,uint16_t)));

  //
  // Detach
  //
  if(!debug) {
    daemon(0,0);
  }

  //
  // Write PID File
  //
  if((f=fopen(LPCODECPOOL_PID_FILE,"w"))!=NULL) {
    fprintf(f,"%u",getpid());
    fclose(f);
  }

  //
  // Set Signal Handlers
  //
  signal(SIGHUP,SigHandler);
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);
}


void MainObject::inputCrosspointChangedData(int id,int output,int input)
{
  //  printf("inputCrosspointChangedData(%d,%d,%d)\n",id,output,input);
}


void MainObject::outputCrosspointChangedData(int id,int output,int input)
{
  //  printf("outputCrosspointChangedData(%d,%d,%d)\n",id,output,input);

  int codec_num=lp_codecs->codecBySwitcherInput(input);
  int room_num=lp_rooms->roomBySwitcherOutput(output);
  int port_num=lp_rooms->portBySwitcherOutput(output);

  //
  // Process Crosspoint Change
  //
  if((codec_num<0)&&(room_num<0)&&(port_num<0)) {
    return;   // Crosspoint unused, ignore
  }
  if((codec_num<0)||(room_num<0)||(port_num<0)) {
    syslog(LOG_WARNING,"unrecognized output crosspoint change received: output: %d  input: %d  codec: %d  room: %d  port: %d",
	   output,input,codec_num,room_num,port_num);
    return;
  }
  lp_rooms->setCodec(room_num,port_num,codec_num);

  //
  // Notify Clients
  //
  UpdateClients();
}


void MainObject::gpiChangedData(int id,int line,bool state)
{
}


void MainObject::commandReceivedData(int id,int cmd,const QStringList &args)
{
  QStringList oargs;
  unsigned codec_num;
  unsigned port_num;
  bool ok=false;
  int room_num=lp_server->source(id);

  switch((MainObject::PanelCommand)cmd) {
  case MainObject::DC:
    lp_server->closeConnection(id);
    break;
  case MainObject::GC:
    oargs.push_back("S");
    oargs.push_back(QString().sprintf("%u",lp_codecs->codecQuantity()));
    oargs.push_back(QString().sprintf("%u",lp_rooms->portQuantity(room_num)));
    oargs.push_back(lp_rooms->name(room_num));
    lp_server->sendCommand(id,(int)MainObject::GC,oargs);

    for(unsigned i=0;i<lp_codecs->codecQuantity();i++) {
      oargs.clear();
      oargs.push_back("O");
      oargs.push_back(QString().sprintf("%u",i+1));
      oargs.push_back(lp_codecs->name(i));
      lp_server->sendCommand(id,(int)MainObject::GC,oargs);

      oargs.clear();
      oargs.push_back("B");
      oargs.push_back(QString().sprintf("%u",i+1));
      oargs.push_back(QString().sprintf("%u",1+lp_codecs->busyInRoom(i)));
      lp_server->sendCommand(id,(int)MainObject::GC,oargs);
    }

    for(unsigned i=0;i<lp_rooms->portQuantity(room_num);i++) {
      oargs.clear();
      oargs.push_back("I");
      oargs.push_back(QString().sprintf("%u",i+1));
      oargs.push_back(lp_rooms->portName(room_num,i));
      lp_server->sendCommand(id,(int)MainObject::GC,oargs);
    }

    oargs.clear();
    lp_server->sendCommand(id,(int)MainObject::GC,oargs);

    UpdateClient(id);
    break;

  case MainObject::SX:
    codec_num=args[0].toUInt(&ok)-1;
    if(ok&&(codec_num<lp_codecs->codecQuantity())) {
      port_num=args[1].toUInt(&ok)-1;
      if(ok&&(port_num<lp_rooms->portQuantity(room_num))) {
	lp_devices->inputSwitcher()->
	  setCrosspoint(lp_codecs->switcherOutput(codec_num),
			lp_rooms->switcherInput(room_num,port_num));
	lp_devices->outputSwitcher()->
	  setCrosspoint(lp_rooms->switcherOutput(room_num,port_num),
			lp_codecs->switcherInput(codec_num));
	lp_rooms->setCodec(room_num,port_num,codec_num);
	UpdateClients();
      }
    }
    break;
  }
}


void MainObject::newConnectionData(int id,const QHostAddress &addr,uint16_t port)
{
  int room=-1;
  if((room=lp_rooms->belongsTo(addr))>=0) {
    lp_server->setSource(id,room);
    syslog(LOG_DEBUG,
	   "associated new panel connection from %s:%d to room \"%s\" [%u]",
	   (const char *)addr.toString().toAscii(),0xFFFF&port,
	   (const char *)lp_rooms->name(room).toUtf8(),room+1);
    return;
  }
  syslog(LOG_WARNING,
	 "new panel connection from %s:%d not associated with any room",
	 (const char *)addr.toString().toAscii(),0xFFFF&port);
  lp_server->closeConnection(id);
}


void MainObject::UpdateClients()
{
  std::vector<int> ids;

  lp_server->getIdList(&ids);
  for(unsigned i=0;i<ids.size();i++) {
    UpdateClient(ids[i]);
  }
}


void MainObject::UpdateClient(int id)
{
  QStringList oargs;
  int room_num=lp_server->source(id);

  for(unsigned j=0;j<lp_rooms->portQuantity(room_num);j++) {
    oargs.clear();
    oargs.push_back(QString().sprintf("%d",1+lp_rooms->codec(room_num,j)));
    oargs.push_back(QString().sprintf("%u",j+1));
    lp_server->sendCommand(id,(int)MainObject::SX,oargs);
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
