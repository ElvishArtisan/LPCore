// lpconnection.cpp
//
// Client Connector for LPCore
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpconnection.cpp,v 1.7 2013/08/09 23:19:49 cvs Exp $
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

#include <syslog.h>

#include "lpconnection.h"

LPConnection::LPConnection(bool track_state,QObject *parent)
  : QObject(parent)
{
  conn_track_state=track_state;
  conn_context=LPConnection::Isolated;
  conn_hostname="";
  conn_port=0;
  conn_watchdog_state=false;
  conn_result=LPConnection::Unconnected;

  //
  // Socket
  //
  conn_socket=new QTcpSocket(this);
  connect(conn_socket,SIGNAL(readyRead()),this,SLOT(socketReadyReadData()));
  connect(conn_socket,SIGNAL(connected()),
	  this,SLOT(socketConnectedData()));
  connect(conn_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(socketErrorData(QAbstractSocket::SocketError)));

  //
  // Parser
  //
  conn_parser=new LPParser(this);
  connect(conn_parser,SIGNAL(messageReceived(const LPMessage &)),
	  this,SLOT(messageReceivedData(const LPMessage &)));

  //
  // Watchdog Timer
  //
  conn_watchdog_timer=new QTimer(this);
  conn_watchdog_timer->setSingleShot(true);
  connect(conn_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));
}


LPConnection::~LPConnection()
{
  for(std::map<int,LPEngine *>::const_iterator it=conn_engines.begin();
      it!=conn_engines.end();it++) {
    delete it->second;
  }
  delete conn_watchdog_timer;
  delete conn_parser;
  delete conn_socket;
}


LPConnection::Context LPConnection::context() const
{
  return conn_context;
}


void LPConnection::setContext(LPConnection::Context context)
{
  if(context!=conn_context) {
    SendContext(context);
    conn_context=context;
  }
}


int LPConnection::sourceDevice(int engine,int chan)
{
  if(conn_engines[engine]==NULL) {
    return 0;
  }
  return conn_engines[engine]->sourceDevice(chan);
}


bool LPConnection::bussState(int engine,int device,int surface,int buss)
{
  if(conn_engines[engine]==NULL) {
    return false;
  }
  return conn_engines[engine]->bussState(device,surface,buss);
}


int LPConnection::faderLevel(int engine,int device,int surface)
{
  if(conn_engines[engine]==NULL) {
    return 0;
  }
  return conn_engines[engine]->faderLevel(device,surface);
}


LPMessage::ChannelMode LPConnection::mode(int engine,int device,int surface)
{
  if(conn_engines[engine]==NULL) {
    return LPMessage::ModeStereo;
  }
  return conn_engines[engine]->mode(device,surface);
}


QString LPConnection::profileName() const
{
  return conn_profile_name;
}


LPConnection::Result LPConnection::loginResult() const
{
  return conn_result;
}


void LPConnection::connectToHost(const QString &hostname,uint16_t port,
				 const QString &profile,const QString &password)
{
  conn_hostname=hostname;
  conn_port=port;
  conn_profile_name=profile;
  conn_password=password;
  conn_socket->connectToHost(hostname,port);
}


void LPConnection::writeMessage(const QByteArray &data)
{
  conn_socket->write(data);
}


void LPConnection::writeMessage(const char *data,int len)
{
  writeMessage(QByteArray(data,len));
}


QString LPConnection::resultText(LPConnection::Result res)
{
  QString ret=tr("Unknown login result");

  switch(res) {
  case LPConnection::Valid: 
    ret=tr("Connected");
    break;

  case LPConnection::InvalidUser:
    ret=tr("Invalid user name");
    break;

  case LPConnection::InvalidPassword:
    ret=tr("Invalid password");
    break;

  case LPConnection::NoPermission:
    ret=tr("No vGuest permission");
    break;

  case LPConnection::NoProfile:
    ret=tr("No profile assigned");
    break;

  case LPConnection::Unconnected:
    ret=tr("Unconnected");
    break;
  }

  return ret;
}


void LPConnection::messageReceivedData(const LPMessage &msg)
{
  char lp2[1024];

  if(conn_profile_name.isEmpty()) {
    emit messageReceived(msg);
  }
  else {
    switch(0xFF&msg[2]) {
    case 0xF9:      // Login Query
      lp2[0]=0x02;
      lp2[1]=0x22;
      lp2[2]=0xF9;   // Login
      lp2[3]=LPCORE_VGUEST_ID;
      memset(lp2+4,0x20,32);   // Space Padding
      memcpy(lp2+4,(const char *)conn_profile_name.left(16).toAscii(),
	     conn_profile_name.left(16).length());
      memcpy(lp2+20,(const char *)conn_password.left(16).toAscii(),
	     conn_password.left(16).length());
      conn_socket->write(lp2,36);
      break;

    case 0xF0:      // Login Status
      conn_result=(LPConnection::Result)(0xFF&msg[3]);
      emit loggedIn(conn_result);
      if((conn_result==LPConnection::Valid)&&conn_track_state) {
	for(std::map<int,LPEngine *>::const_iterator it=conn_engines.begin();
	    it!=conn_engines.end();it++) {
	  delete it->second;
	}
	lp2[0]=0x04;
	lp2[1]=0x01;
	lp2[2]=0x02;
	conn_socket->write(lp2,3);
      }

      break;

    default:
      if(msg.isValid()) {
	if(conn_track_state) {
	  UpdateState(msg);
	}
	emit messageReceived(msg);
      }
      else {
	syslog(LOG_NOTICE,"received invalid LP message %s",
	       (const char *)LPMessage::dump(msg).toAscii());
      }
      break;
    }
  }
}


void LPConnection::socketReadyReadData()
{
  char data[1024];
  int n;

  while((n=conn_socket->read(data,1024))>0) {
    conn_parser->writeData(data,n);
  }
}


void LPConnection::socketConnectedData()
{
  SendContext(conn_context);
  if(!conn_watchdog_state) {
    conn_watchdog_timer->stop();
    conn_watchdog_state=true;
    emit watchdogStateChanged(true);
  }
}


void LPConnection::socketErrorData(QAbstractSocket::SocketError err)
{
  QString str=tr("Unknown network error");

  switch(err) {
  case QAbstractSocket::SocketTimeoutError:
  case QAbstractSocket::RemoteHostClosedError:
    conn_watchdog_timer->start(LPCORE_LPCONNECTION_WATCHDOG_INTERVAL);
    if(conn_watchdog_state) {
      conn_watchdog_state=false;
      emit watchdogStateChanged(false);
    }
    return;

  case QAbstractSocket::ConnectionRefusedError:
    str=tr("Connection refused");
    break;


  case QAbstractSocket::HostNotFoundError:
    str=tr("Host not found");
    break;

  case QAbstractSocket::SocketAccessError:
    str=tr("Socket access error");
    break;

  case QAbstractSocket::SocketResourceError:
    str=tr("Socket resource error");
    break;

  case QAbstractSocket::DatagramTooLargeError:
    str=tr("Datagram too large");
    break;

  case QAbstractSocket::NetworkError:
    str=tr("Network error");
    break;

  case QAbstractSocket::AddressInUseError:
    str=tr("Address in use");
    break;

  case QAbstractSocket::SocketAddressNotAvailableError:
    str=tr("Address not available");
    break;

  case QAbstractSocket::UnsupportedSocketOperationError:
    str=tr("Unsupporte socket operation");
    break;

  case QAbstractSocket::UnknownSocketError:
    break;

  default:
    break;
  }
  emit socketError(err,str);
}


void LPConnection::watchdogData()
{
  conn_socket->connectToHost(conn_hostname,conn_port);
}


void LPConnection::SendContext(LPConnection::Context context) const
{
  char lp2[256];

  lp2[0]=0x04;
  lp2[1]=0x02;
  lp2[2]=0x01;
  lp2[3]=0xFF&context;
  conn_socket->write(lp2,4);
}


void LPConnection::UpdateState(const LPMessage &msg)
{

  if(msg.engine()<=0) {
    return;
  }
  if(conn_engines[msg.engine()]==NULL) {
    conn_engines[msg.engine()]=new LPEngine(msg.engine());
  }
  LPEngine *e=conn_engines[msg.engine()];

  switch(msg.command()) {
  case LPMessage::CmdTurnOn:
    e->setBussState(msg.device(),msg.surface(),msg.buss(),true);
    break;

  case LPMessage::CmdTurnOff:
    e->setBussState(msg.device(),msg.surface(),msg.buss(),false);
    break;

  case LPMessage::CmdInputAssign:
    e->setSourceDevice(msg.device(),msg.sourceDevice());
    break;

  case LPMessage::CmdInputMode:
    e->setMode(msg.device(),msg.surface(),msg.channelMode());
    break;

  case LPMessage::CmdFaderLevel:
    e->setFaderLevel(msg.device(),msg.surface(),msg.level());
    break;

  default:
    break;
  }
}
