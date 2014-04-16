// lpcored.h
//
// lpcored(8) Routing Daemon.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpcored.h,v 1.17 2013/08/20 20:59:30 cvs Exp $
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

#ifndef LPCORED_H
#define LPCORED_H

#include <map>
#include <vector>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>

#include <lpcore/lpconfig.h>

#include "apphost.h"
#include "engine.h"
#include "connection.h"

#define LPCORED_USAGE "-d\n"
#define LPCORED_PID_FILE "/var/run/lpcored.pid"
#define LPCORED_APPLICATION_DELAY 20000

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void engineMessageReceivedData(int engine,const QByteArray &msg);
  void engineResetData(int engine,bool state);
  void connectionMessageReceivedData(int id,const LPMessage &msg);
  void newConnectionData();
  void garbageCollectionData();
  void applicationStartData();
  void sendFullState(int id);

 private:
  void SendLoginQuery(int id);
  int GetDeviceChannel(uint8_t engine,uint8_t dev_byte1,uint8_t dev_byte2,
		       uint8_t surface);
  void SendToEngine(int engine,const QByteArray &data);
  void SendToEngine(int engine,const char *data,int len);
  void SendClientMessage(const QByteArray &msg,int except_id=-1);
  void SendClientMessage(const char *msg,int len,int except_id=-1);
  void SendClientGlobalMessage(const QByteArray &msg);
  void SendClientGlobalMessage(const char *msg,int len);
  Engine *GetEngine(int engine,Connection *conn,const QByteArray &msg);
  std::map<int,Engine *> main_engines;
  std::map<int,Connection *> main_connections;
  std::vector<AppHost *> main_app_hosts;
  QTcpServer *main_server;
  int main_next_connection_id;
  std::vector<int> main_dead_connections;
  QTimer *main_garbage_timer;
  QTimer *main_application_timer;
  LPConfig *main_config;
  bool main_applications_started;
  friend void SigHandler(int signo);
};


#endif  // LPCORED_H
