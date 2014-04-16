// lpconnection.h
//
// Client Connector for LPCore
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpconnection.h,v 1.4 2013/08/09 23:19:49 cvs Exp $
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

#ifndef LPCONNECTION_H
#define LPCONNECTION_H

#include <stdint.h>

#include <map>

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>
#include <lpcore/lpengine.h>
#include <lpcore/lpmessage.h>
#include <lpcore/lpparser.h>

class LPConnection : public QObject
{
  Q_OBJECT;
 public:
  enum Context {Isolated=0,Global=1};
  enum Result {Unconnected=0,Valid=10,InvalidUser=11,InvalidPassword=12,
	       NoPermission=13,NoProfile=14};
  LPConnection(bool track_state,QObject *parent);
  ~LPConnection();
  LPConnection::Context context() const;
  void setContext(LPConnection::Context context);
  int sourceDevice(int engine,int chan);
  bool bussState(int engine,int device,int surface,int buss);
  int faderLevel(int engine,int device,int surface);
  LPMessage::ChannelMode mode(int engine,int device,int surface);
  QString profileName() const;
  LPConnection::Result loginResult() const;
  void connectToHost(const QString &hostname,
		     uint16_t port=LPCORE_CONNECTION_TCP_PORT,
		     const QString &profile="",const QString &password="");
  void writeMessage(const QByteArray &data);
  void writeMessage(const char *data,int len);
  static QString resultText(LPConnection::Result res);

 signals:
  void messageReceived(const LPMessage &msg);
  void loggedIn(LPConnection::Result res);
  void watchdogStateChanged(bool state);
  void socketError(QAbstractSocket::SocketError err,const QString &txt);

 private slots:
  void messageReceivedData(const LPMessage &msg);
  void socketReadyReadData();
  void socketConnectedData();
  void socketErrorData(QAbstractSocket::SocketError err);
  void watchdogData();

 private:
  void SendContext(LPConnection::Context context) const;
  void UpdateState(const LPMessage &msg);
  QString conn_hostname;
  LPConnection::Context conn_context;
  uint16_t conn_port;
  QString conn_profile_name;
  QString conn_password;
  QTcpSocket *conn_socket;
  LPParser *conn_parser;
  QTimer *conn_watchdog_timer;
  bool conn_watchdog_state;
  LPConnection::Result conn_result;
  bool conn_track_state;
  std::map<int,LPEngine *> conn_engines;
};


#endif  // LPCONNECTION_H
