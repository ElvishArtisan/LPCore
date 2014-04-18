// lpnetrouter.cpp
//
// lpnetrouter(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpnetrouter.cpp,v 1.22 2013/09/17 21:14:20 cvs Exp $
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
#include <QtNetwork/QTcpServer>

#include <lpcore/lpcmdswitch.h>
#include <lpcore/lpconfig.h>

#include "lpnetrouter.h"

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
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpnetrouter",LPNETROUTER_USAGE);
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
    openlog("lpnetrouter",LOG_PERROR,LOG_USER);
  }
  else {
    openlog("lpnetrouter",0,LOG_USER);
  }

  //
  // Read Configuration
  //
  lp_profile=new LPProfile();
  if(!lp_profile->setSource(LPNETROUTER_CONF_FILE)) {
    syslog(LOG_ERR,"unable to open configuration file at \"%s\"",
	   LPNETROUTER_CONF_FILE);
    exit(256);
  }

  //
  // Switcher Device
  //
  lp_main_switcher=LPSwitcherFactory(1,
	    (LPSwitcher::Type)lp_profile->intValue("Switcher","Type"),this);
  connect(lp_main_switcher,SIGNAL(crosspointChanged(int,int,int)),
	  this,SLOT(crosspointChangedData(int,int,int)));
  connect(lp_main_switcher,SIGNAL(gpiChanged(int,int,bool)),
	  this,SLOT(gpiChangedData(int,int,bool)));
  connect(lp_main_switcher,SIGNAL(silenceSenseChanged(int,int,bool)),
	  this,SLOT(silenceSenseChangedData(int,int,bool)));
  if(!lp_main_switcher->open(lp_profile->stringValue("Switcher","Device"))) {
    syslog(LOG_ERR,"unable to open main switcher device \"%s\"",
	   (const char *)lp_profile->stringValue("Switcher","Device").
	   toAscii());
    exit(256);
  }

  //
  // Load UDP Queues
  //
  int count=0;
  int port=0;
  lp_udp_mapper=new QSignalMapper(this);
  connect(lp_udp_mapper,SIGNAL(mapped(int)),this,SLOT(udpReadyReadData(int)));
  while((port=lp_profile->
	 intValue(QString().sprintf("Udp%d",count+1),"Port"))>0) {
    lp_udp_sockets.push_back(new QUdpSocket(this));
    connect(lp_udp_sockets.back(),SIGNAL(readyRead()),
	    lp_udp_mapper,SLOT(map()));
    lp_udp_mapper->setMapping(lp_udp_sockets.back(),lp_udp_sockets.size()-1);
    if(!lp_udp_sockets.back()->bind(port)) {
      syslog(LOG_WARNING,"unable to bind port %d for UDP queue %lu",
	     port,lp_udp_sockets.size()+1);
    }
    count++;
  }

  //
  // Load Sources
  //
  Source *s=new Source(lp_profile);
  while(s->load(lp_sources.size())) {
    connect(s,SIGNAL(resetRelayLamp(int,int,int,int)),
	    this,SLOT(resetRelayLampData(int,int,int,int)));
    lp_sources.push_back(s);
    s=new Source(lp_profile);
  }
  delete s;

  //
  // Load Destinations
  //
  Destination *d=new Destination(lp_profile);
  while(d->load(lp_destinations.size())) {
    lp_destinations.push_back(d);
    d=new Destination(lp_profile);
  }
  delete d;

  //
  // RML Interface
  //
  lp_rml_socket=new QUdpSocket(this);
  connect(lp_rml_socket,SIGNAL(readyRead()),this,SLOT(rmlReadyReadData()));
  if(!lp_rml_socket->bind(lp_profile->
		   intValue("Rml","Port",LPNETROUTER_DEFAULT_RML_PORT))) {
    syslog(LOG_WARNING,"unable to bind RML port %d",
	   0xFFFF&lp_profile->intValue("Rml","Port",
				       LPNETROUTER_DEFAULT_RML_PORT));
  }
  lp_rml_matrix_number=lp_profile->intValue("Rml","MatrixNumber");
  count=0;
  QHostAddress addr;
  bool ok=false;
  addr=lp_profile->
    addressValue("Rml",QString().sprintf("DestinationAddress%d",count+1),
		 QHostAddress(),&ok);
  while (ok) {
    lp_rml_dst_addresses.push_back(addr);
    lp_rml_dst_ports.push_back(lp_profile->intValue("Rml",QString().
	sprintf("DestinationPort%d",count+1),LPNETROUTER_DEFAULT_RML_PORT));
    addr=lp_profile->
      addressValue("Rml",QString().sprintf("DestinationAddress%d",count+1),
		   QHostAddress(),&ok);
    count++;
  }

  //
  // Panel Command Server
  //
  QTcpServer *server=new QTcpServer(this);
  server->listen(QHostAddress::Any,LPNETROUTER_PANEL_TCP_PORT);
  std::map<int,QString> cmds;
  std::map<int,int> upper_limits;
  std::map<int,int> lower_limits;

  cmds[MainObject::DC]="DC";
  upper_limits[MainObject::DC]=0;
  lower_limits[MainObject::DC]=0;

  cmds[MainObject::GC]="GC";
  upper_limits[MainObject::GC]=0;
  lower_limits[MainObject::GC]=0;

  cmds[MainObject::AX]="AX";
  upper_limits[MainObject::AX]=2;
  lower_limits[MainObject::AX]=2;

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
  // Set Signal Handlers
  //
  signal(SIGHUP,SigHandler);
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);

  //
  // LPCore Connection
  //
  lp_connection=new LPConnection(true,this);
  lp_connection->setContext(LPConnection::Global);
  connect(lp_connection,SIGNAL(messageReceived(const LPMessage &)),
	  this,SLOT(serverMessageReceivedData(const LPMessage &)));

  lp_connection->
    connectToHost(lp_profile->stringValue("Server","Hostname","localhost"),
		  lp_profile->intValue("Server","Port",
				       LPCORE_CONNECTION_TCP_PORT),
		  lp_profile->stringValue("Server","ProfileName","profile"),
		  lp_profile->stringValue("Server","Password"));
}


void MainObject::crosspointChangedData(int id,int output,int input)
{
  QStringList oargs;
  Source *s=NULL;

  for(unsigned i=0;i<lp_destinations.size();i++) {
    if(lp_destinations[i]->id()==output) {
      if(lp_destinations[i]->source()<(int)lp_sources.size()) {
	if(!SourceIsActive(lp_destinations[i]->source(),output)) {
	  s=lp_sources[lp_destinations[i]->source()];
	  for(unsigned j=0;j<s->sourceAddresses();j++) {
	    lp_rml_socket->writeDatagram(s->sourceOffRml(j).toAscii(),
					 s->sourceOffRml(j).length(),
					 s->sourceAddress(j),
					 LPNETROUTER_DEFAULT_RML_PORT);
	  }
	}
      }
      lp_destinations[i]->setSource(input);
      lp_destinations[i]->setArmed(input);
      if(lp_destinations[i]->source()<(int)lp_sources.size()) {
	s=lp_sources[lp_destinations[i]->source()];
	for(unsigned j=0;j<s->sourceAddresses();j++) {
	  if(lp_connection->bussState(s->sourceEngine(j),s->sourceDevice(j),
				      s->sourceSurface(j),s->sourceBuss(j))) {
	    lp_rml_socket->writeDatagram(s->sourceOnRml(j).toAscii(),
					 s->sourceOnRml(j).length(),
					 s->sourceAddress(j),
					 LPNETROUTER_DEFAULT_RML_PORT);
	  }
	}
      }
      oargs.clear();
      oargs.push_back(QString().sprintf("%u",output+1));
      oargs.push_back(QString().sprintf("%u",input+1));
      lp_server->sendCommand((int)MainObject::SX,oargs);
      lp_server->sendCommand((int)MainObject::AX,oargs);

      SendRml(lp_destinations[i]->sourceChangedRml(),output,input,0);
    }
  }
}


void MainObject::gpiChangedData(int id,int gpi,bool state)
{
  printf("gpiChangedData(%d,%d,%d)\n",id,gpi,state);
}


void MainObject::silenceSenseChangedData(int id,int chan,bool state)
{
  if(!lp_destinations[id]->silenceSenseRml(state).isEmpty()) {
    SendRml(lp_destinations[id]->silenceSenseRml(state));
  }
}


void MainObject::serverMessageReceivedData(const LPMessage &msg)
{
  Source *s=NULL;
  Destination *d=NULL;
  int relay=-1;
  int src=-1;
  int output=-1;
  char lp2[256];

  switch(0xFF&msg[2]) {
  case 0x51:    // Momentary
  case 0x52:    // Turn ON
    if((msg.size()==8)||(msg.size()==9)) {
      for(unsigned i=0;i<lp_sources.size();i++) {
	s=lp_sources[i];
	if((relay=s->belongsToRelay(0xFF&msg[3],0xFF&msg[6],
				    ((0xFF&msg[4])<<8)+(0xFF&msg[5]),
				    0xFF&msg[7]))>=0) {
	  for(unsigned j=0;j<lp_destinations.size();j++) {
	    d=lp_destinations[j];
	    if(d->source()==s->id()) {
	      if((output=d->relayOutput(relay))>=0) {
		syslog(LOG_DEBUG,"gpo %d activated",output+1);
		lp_main_switcher->pulseGpo(output);
		LightLamp(s,relay);
	      }
	    }
	  }
	  if(s->directRelayEngine(relay)>0) {
	    lp2[0]=0x02;
	    lp2[1]=0x07;
	    lp2[2]=0x51;   // Momentary
	    lp2[3]=0xFF&s->directRelayEngine(relay);
	    lp2[4]=0xFF&(s->directRelayDevice(relay)>>8);
	    lp2[5]=0xFF&s->directRelayDevice(relay);
	    lp2[6]=0xFF&s->directRelaySurface(relay);
	    lp2[7]=0xFF&s->directRelayBuss(relay);
	    lp2[8]=0x06;
	    lp_connection->writeMessage(lp2,9);
	    syslog(LOG_DEBUG,"activated direct relay %d for source %d",
		   relay+1,i+1);
	  }
	}
	if((relay=s->belongsToRelayButton(0xFF&msg[3],0xFF&msg[6],
					  ((0xFF&msg[4])<<8)+(0xFF&msg[5]),
					  0xFF&msg[7]))>=0) {
	  for(unsigned j=0;j<lp_destinations.size();j++) {
	    d=lp_destinations[j];
	    if(d->source()==s->id()) {
	      if((output=d->relayOutput(relay))>=0) {
		syslog(LOG_DEBUG,"gpo %d activated",output+1);
		lp_main_switcher->pulseGpo(output);
		LightLamp(s,relay);
	      }
	    }
	  }
	  if(s->directRelayEngine(relay)>0) {
	    lp2[0]=0x02;
	    lp2[1]=0x07;
	    lp2[2]=0x51;   // Momentary
	    lp2[3]=0xFF&s->directRelayEngine(relay);
	    lp2[4]=0xFF&(s->directRelayDevice(relay)>>8);
	    lp2[5]=0xFF&s->directRelayDevice(relay);
	    lp2[6]=0xFF&s->directRelaySurface(relay);
	    lp2[7]=0xFF&s->directRelayBuss(relay);
	    lp2[8]=0x06;
	    lp_connection->writeMessage(lp2,9);
	    syslog(LOG_DEBUG,"activated direct relay %d for source %d",
		   relay+1,i+1);
	  }
	}
      }
    }
    break;
  }

  switch(0xFF&msg[2]) {
  case 0x52:    // Turn ON
    if(msg.size()==8) {
      for(unsigned i=0;i<lp_sources.size();i++) {
	s=lp_sources[i];
	if((src=s->belongsToSourceSource(0xFF&msg[3],0xFF&msg[6],
					 ((0xFF&msg[4])<<8)+(0xFF&msg[5]),
					 0xFF&msg[7]))>=0) {
	  for(unsigned j=0;j<lp_destinations.size();j++) {
	    d=lp_destinations[j];
	    if(d->source()==s->id()) {
	      if(!s->sourceOnRml(src).isEmpty()) {
		lp_rml_socket->writeDatagram(s->sourceOnRml(src).toAscii(),
					     s->sourceOnRml(src).length(),
					     s->sourceAddress(src),
					     LPNETROUTER_DEFAULT_RML_PORT);
		syslog(LOG_DEBUG,"sending RML \"%s\" to %s:%d",
		       (const char *)s->sourceOnRml(src).toAscii(),
		       (const char *)s->sourceAddress(src).toString().toAscii(),
		       LPNETROUTER_DEFAULT_RML_PORT);
	      }
	    }
	  }
	}
      }
    }
    break;

  case 0x53:    // Turn OFF
    if(msg.size()==8) {
      for(unsigned i=0;i<lp_sources.size();i++) {
	s=lp_sources[i];
	if((src=s->belongsToSourceSource(0xFF&msg[3],0xFF&msg[6],
					 ((0xFF&msg[4])<<8)+(0xFF&msg[5]),
					 0xFF&msg[7]))>=0) {
	  for(unsigned j=0;j<lp_destinations.size();j++) {
	    d=lp_destinations[j];
	    if(d->source()==s->id()) {
	      if(!s->sourceOffRml(src).isEmpty()) {
		lp_rml_socket->writeDatagram(s->sourceOffRml(src).toAscii(),
					     s->sourceOffRml(src).length(),
					     s->sourceAddress(src),
					     LPNETROUTER_DEFAULT_RML_PORT);
		syslog(LOG_DEBUG,"sending RML \"%s\" to %s:%d",
		       (const char *)s->sourceOffRml(src).toAscii(),
		       (const char *)s->sourceAddress(src).toString().toAscii(),
		       LPNETROUTER_DEFAULT_RML_PORT);
	      }
	    }
	  }
	}
      }
    }
    break;
  }
}


void MainObject::udpReadyReadData(int udp)
{
  char data[1501];
  QString msg;
  int n;
  QUdpSocket *s=lp_udp_sockets[udp];
  QHostAddress addr;

  while((n=s->readDatagram(data,1500,&addr))>0) {
    syslog(LOG_DEBUG,
	   "udp queue %d received %d byte packet from %s at port %u\n",
	   udp+1,n,
	   (const char *)addr.toString().toAscii(),0xFFFF&s->localPort());
    data[n]=0;
    for(unsigned i=0;i<lp_sources.size();i++) {
      Source *s=lp_sources[i];
      if(s->belongsToUdp(udp,addr)) {
	syslog(LOG_DEBUG,"  HERE1");
	for(unsigned j=0;j<lp_destinations.size();j++) {
	  Destination *d=lp_destinations[j];
	  if(d->source()==s->id()) {
	    syslog(LOG_DEBUG,"  HERE2");
	    for(unsigned k=0;k<d->udpDestinations(udp);k++) {
	      syslog(LOG_DEBUG,"  HERE3:%u",k);
	      msg=Rewrite(data,d->udpRewriteRule(udp,k));
	      lp_udp_sockets[udp]->
		writeDatagram((const char *)msg.toAscii(),msg.length(),
			      d->udpAddress(udp,k),d->udpPort(udp,k));
	      syslog(LOG_DEBUG,"forwarded to Destination%u [%s:%u]",
		    j+1,(const char *)d->udpAddress(udp,k).toString().toAscii(),
		     0xFFFF&d->udpPort(udp,k));
	    }
	  }
	}
      }
    }
  }
}


void MainObject::rmlReadyReadData()
{
  char data[1500];
  int n;
  QHostAddress addr;
  uint16_t port;
  QStringList f1;

  while((n=lp_rml_socket->readDatagram(data,1500,&addr,&port))>0) {
    if(data[n-1]=='!') {
      data[n]=0;
      QString str(data);
      f1=str.left(str.length()-1).split(" ");
      ProcessRml(f1,addr,port);
    }
    else {
      syslog(LOG_WARNING,"received malformatted RML from %s:%d",
	     (const char *)addr.toString().toAscii(),0xFFFF&port);
    }
  }
}


void MainObject::commandReceivedData(int id,int cmd,const QStringList &args)
{
  QStringList oargs;
  unsigned input=-1;
  unsigned output=-1;
  bool ok=false;

  switch((MainObject::PanelCommand)cmd) {
  case MainObject::DC:
    lp_server->closeConnection(id);
    break;

  case MainObject::GC:
    oargs.push_back("S");
    oargs.push_back(QString().sprintf("%lu",lp_destinations.size()));
    oargs.push_back(QString().sprintf("%lu",lp_sources.size()));
    lp_server->sendCommand(id,(int)MainObject::GC,oargs);
    for(unsigned i=0;i<lp_sources.size();i++) {
      oargs.clear();
      oargs.push_back("I");
      oargs.push_back(QString().sprintf("%u",i+1));
      oargs.push_back(lp_sources[i]->name());
      lp_server->sendCommand(id,(int)MainObject::GC,oargs);
    }
    for(unsigned i=0;i<lp_destinations.size();i++) {
      oargs.clear();
      oargs.push_back("O");
      oargs.push_back(QString().sprintf("%u",i+1));
      oargs.push_back(lp_destinations[i]->name());
      lp_server->sendCommand(id,(int)MainObject::GC,oargs);
    }
    oargs.clear();
    lp_server->sendCommand(id,(int)MainObject::GC,oargs);
    for(unsigned i=0;i<lp_destinations.size();i++) {
      oargs.clear();
      oargs.push_back(QString().sprintf("%u",i+1));
      oargs.push_back(QString().sprintf("%u",lp_destinations[i]->armed()+1));
      lp_server->sendCommand(id,(int)MainObject::AX,oargs);
      oargs.clear();
      oargs.push_back(QString().sprintf("%u",i+1));
      oargs.push_back(QString().sprintf("%u",lp_destinations[i]->source()+1));
      lp_server->sendCommand(id,(int)MainObject::SX,oargs);
    }
    break;

  case MainObject::AX:
    output=args[0].toUInt(&ok)-1;
    if(ok&&(output<lp_destinations.size())) {
      input=args[1].toUInt(&ok)-1;
      if(ok&&(output<lp_sources.size())) {
	if(lp_destinations[output]->armed()!=(int)input) {
	  if(lp_destinations[output]->source()==lp_server->source(id)) {
	    lp_destinations[output]->setArmed(input);
	    lp_server->sendCommand((int)MainObject::AX,args);
	  }
	}
      }
    }
    break;

  case MainObject::SX:
    output=args[0].toUInt(&ok)-1;
    if(ok&&(output<lp_destinations.size())) {
      input=args[1].toUInt(&ok)-1;
      if(ok&&(output<lp_sources.size())) {
	if(lp_destinations[output]->armed()==(int)input) {
	  if(lp_destinations[output]->source()!=(int)input) {
	    if(lp_server->source(id)==(int)input) {
	      lp_main_switcher->setCrosspoint(output,input);
	    }
	  }
	}
      }
    }
    break;
  }
}


void MainObject::newConnectionData(int id,const QHostAddress &addr,uint16_t port)
{
  for(unsigned i=0;i<lp_sources.size();i++) {
    if(lp_sources[i]->belongsToSource(addr)) {
      lp_server->setSource(id,i);
      syslog(LOG_DEBUG,
	     "associated new panel connection from %s:%d to source %u",
	     (const char *)addr.toString().toAscii(),0xFFFF&port,i+1);
      return;
    }
  }
  syslog(LOG_WARNING,
	 "new panel connection from %s:%d not associated with any source",
	 (const char *)addr.toString().toAscii(),0xFFFF&port);
}


void MainObject::resetRelayLampData(int engine,int device,int surface,int buss)
{
  char lp2[1024];

  lp2[0]=0x02;
  lp2[1]=0x06;
  lp2[2]=0x53;
  lp2[3]=0xFF&engine;
  lp2[4]=0xFF&(device>>8);
  lp2[5]=0xFF&device;
  lp2[6]=0xFF&surface;
  lp2[7]=0xFF&buss;
  lp_connection->writeMessage(lp2,8);
}


QString MainObject::Rewrite(const QString &msg,const QString &rule) const
{
  if(rule.isEmpty()) {
    return msg;
  }
  QString ret=rule;
  QStringList f0=msg.split(" ");
  for(int i=0;i<f0.size();i++) {
    ret.replace(QString().sprintf("%%%d",i+1),f0[i]);
  }

  return ret;
}


bool MainObject::SourceIsActive(int source,int except_dest) const
{
  bool ret=false;

  for(int i=0;i<lp_main_switcher->outputs();i++) {
    if(i!=except_dest) {
      ret=ret||(lp_main_switcher->crosspoint(i)==source);
    }
  }
  return ret;
}


void MainObject::LightLamp(Source *s,int relay)
{
  char lp2[1024];

  lp2[0]=0x02;
  lp2[1]=0x06;
  lp2[2]=0x52;
  lp2[3]=0xFF&s->relayLampEngine(relay);
  lp2[4]=0xFF&(s->relayLampDevice(relay)>>8);
  lp2[5]=0xFF&s->relayLampDevice(relay);
  lp2[6]=0xFF&s->relayLampSurface(relay);
  lp2[7]=0xFF&s->relayLampBuss(relay);
  lp_connection->writeMessage(lp2,8);
  s->lightRelayLamp(relay);
}


void MainObject::SendRml(const QString &rml,unsigned dst,unsigned src,
			 unsigned relay)
{
  if(rml.isEmpty()) {
    return;
  }
  QString str=rml;

  str.replace("%dst_num",QString().sprintf("%u",dst));
  if(dst<lp_destinations.size()) {
    str.replace("%dst_name",lp_destinations[dst]->name());
  }
  else {
    str.replace("%dst_name",tr("Destination")+QString().sprintf(" %u",dst+1));
  }
  str.replace("%src_num",QString().sprintf("%u",src));
  if(src<lp_sources.size()) {
    str.replace("%src_name",lp_sources[src]->name());
  }
  else {
    str.replace("%src_name",tr("Source")+QString().sprintf(" %u",src+1));
  }
  str.replace("%relay_num",QString().sprintf("%u",relay));

  SendRml(str);
}


void MainObject::SendRml(const QString &rml) const
{
  for(unsigned i=0;i<lp_rml_dst_addresses.size();i++) {
    lp_rml_socket->writeDatagram(rml.toAscii(),rml.length(),
				 lp_rml_dst_addresses[i],lp_rml_dst_ports[i]);
  }
}


void MainObject::ProcessRml(QStringList rml,
			    const QHostAddress &src_addr,uint16_t src_port)
{
  bool ok=false;

  if((rml[0]=="ST")&&(rml.size()==4)) {
    if(rml[1].toInt()==lp_rml_matrix_number) {
      int input=rml[2].toUInt(&ok)-1;
      if((!ok)||(input>=lp_main_switcher->inputs())) {
	syslog(LOG_WARNING,
	       "tried to set non-existent source %d via RML from %s:%d",
	       input+1,(const char *)src_addr.toString().toAscii(),
	       src_port);
	return;
      }
      int output=rml[3].toUInt(&ok)-1;
      if((!ok)||(output>=lp_main_switcher->outputs())) {
	syslog(LOG_WARNING,
	       "tried to set non-existent destination %d to source %d via RML from %s:%d",
	       output+1,input+1,(const char *)src_addr.toString().toAscii(),
	       src_port);
	return;
      }
      lp_main_switcher->setCrosspoint(output,input);
      syslog(LOG_INFO,"destination %d set to source %d via RML from %s:%d",
	     output+1,input+1,(const char *)src_addr.toString().toAscii(),
	     src_port);
    }
  }

  if((rml[0]=="GO")&&(rml.size()==6)) {
    for(int i=2;i<5;i++) {
      rml[i]=rml[i+1];
    }
  }
  if((rml[0]=="GO")&&((rml.size()==5)||(rml.size()==6))) {
    if(rml[1].toInt()==lp_rml_matrix_number) {
      int gpo=rml[2].toUInt(&ok)-1;
      if((!ok)||(gpo>=lp_main_switcher->gpos())) {
	syslog(LOG_WARNING,
	       "tried to pulse non-existent GPO %d via RML from %s:%d",
	       gpo+1,(const char *)src_addr.toString().toAscii(),src_port);
	return;
      }
      int state=rml[3].toUInt(&ok);
      if(ok&&state) {
	lp_main_switcher->pulseGpo(gpo);
	syslog(LOG_WARNING,
	       "pulsed GPO %d via RML from %s:%d",
	       gpo+1,(const char *)src_addr.toString().toAscii(),src_port);
      }
    }
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
