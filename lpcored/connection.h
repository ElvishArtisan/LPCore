// connection.h
//
// LP2 Client Connection
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: connection.h,v 1.9 2013/08/09 23:19:50 cvs Exp $
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpparser.h>

class Connection : public QObject
{
  Q_OBJECT;
 public:
  Connection(int id,QTcpSocket *sock,QObject *parent=NULL);
  ~Connection();
  QAbstractSocket::SocketState state() const;
  QHostAddress peerAddress() const;
  uint16_t peerPort() const;
  bool authenticated() const;
  void setAuthenticated(bool state);
  bool isIsolated() const;
  int writeOverride(const QByteArray &data);
  int writeOverride(const char *data,int len);
  int write(const QByteArray &data);
  int write(const char *data,int len);
  void close();

 signals:
  void messageReceived(int id,const LPMessage &msg);
  void fullStateRequested(int id);

 private slots:
  void messageReceivedData(const LPMessage &msg);
  void metadataReceivedData(const QByteArray &msg);
  void readyReadData();

 private:
  LPParser *conn_parser;
  int conn_id;
  bool conn_authenticated;
  bool conn_is_isolated;
  QTcpSocket *conn_socket;
  char conn_buffer[1024];
  int conn_ptr;
  int conn_istate;
  int conn_body_count;
};


#endif  // CONNECTION_H
