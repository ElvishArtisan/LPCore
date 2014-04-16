// connection.cpp
//
// LP2 Client Connection
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: connection.cpp,v 1.9 2013/11/06 01:35:24 cvs Exp $
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

#include <QtNetwork/QHostAddress>

#include <lpcore/lpconfig.h>
#include <lpcore/lpmessage.h>

#include "connection.h"

Connection::Connection(int id,QTcpSocket *sock,QObject *parent)
  : QObject(parent)
{
  conn_parser=new LPParser(this);
  connect(conn_parser,SIGNAL(messageReceived(const LPMessage &)),
	  this,SLOT(messageReceivedData(const LPMessage &)));
  connect(conn_parser,SIGNAL(metadataReceived(const QByteArray &)),
	  this,SLOT(metadataReceivedData(const QByteArray &)));

  conn_authenticated=false;
  conn_is_isolated=true;
  conn_ptr=0;
  conn_istate=0;
  conn_body_count=0;
  conn_id=id;
  conn_socket=sock;
  connect(conn_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


Connection::~Connection()
{
  delete conn_socket;
  delete conn_parser;
}


QAbstractSocket::SocketState Connection::state() const
{
  return conn_socket->state();
}


QHostAddress Connection::peerAddress() const
{
  return conn_socket->peerAddress();
}


uint16_t Connection::peerPort() const
{
  return conn_socket->peerPort();
}


bool Connection::authenticated() const
{
  return conn_authenticated;
}


void Connection::setAuthenticated(bool state)
{
  conn_authenticated=state;
}


bool Connection::isIsolated() const
{
  return conn_is_isolated;
}


int Connection::writeOverride(const QByteArray &data)
{
  return conn_socket->write(data);
}


int Connection::writeOverride(const char *data,int len)
{
  return writeOverride(QByteArray(data,len));
}


int Connection::write(const QByteArray &data)
{
  if(conn_authenticated) {
    return conn_socket->write(data);
  }
  return 0;
}


int Connection::write(const char *data,int len)
{
  return write(QByteArray(data,len));
}


void Connection::close()
{
  conn_socket->close();
}


void Connection::messageReceivedData(const LPMessage &msg)
{
  emit messageReceived(conn_id,msg);
}


void Connection::metadataReceivedData(const QByteArray &msg)
{
  bool ok=false;
  char reply[256];

  switch(0xFF&msg[2]) {
  case 0x01:   // Set Connection Context
    if(msg.size()==4) {
      conn_is_isolated=msg[3]==0x00;
      ok=true;
    }
    break;

  case 0x02:   // Send Full State
    emit fullStateRequested(conn_id);
    ok=true;
    break;

  case 0x03:   // Connection Ping
    reply[0]=0x04;
    reply[1]=0x01;
    reply[2]=0x03;
    write(reply,3);
    ok=true;
    break;

  default:
    break;
  }

  if(!ok) {
    syslog(LOG_WARNING,"receieved unknown metadata message %s from %s:%d",
	   (const char *)LPMessage::dump(msg).toAscii(),
	   (const char *)peerAddress().toString().toAscii(),
	   0xFFFF&peerPort());
  }
}


void Connection::readyReadData()
{
  char data[1024];
  int n=0;

  while((n=conn_socket->read(data,1024))>0) {
    conn_parser->writeData(data,n);
  }
}
