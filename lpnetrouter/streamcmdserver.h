// streamcmdserver.h
//
// Parse commands on connection-oriented protocols.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: streamcmdserver.h,v 1.1 2013/11/21 22:31:48 cvs Exp $
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

#ifndef STREAMCMDSERVER_H
#define STREAMCMDSERVER_H

#include <stdint.h>

#include <vector>
#include <map>

#include <QtCore/QObject>
#include <QtCore/QSignalMapper>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class StreamCmdServer : public QObject
{
 Q_OBJECT;
 public:
  StreamCmdServer(const std::map<int,QString> &cmd_table,
		  const std::map<int,int> &upper_table,
		  const std::map<int,int> &lower_table,
		  QTcpServer *server,QObject *parent);
  ~StreamCmdServer();
  int source(int id);
  void setSource(int id,int src);

 public slots:
  void sendCommand(int id,int cmd,const QStringList &args=QStringList());
  void sendCommand(int cmd,const QStringList &args=QStringList());
  void closeConnection(int id);

 signals:
  void newConnection(int id,const QHostAddress &addr,uint16_t port);
  void commandReceived(int id,int cmd,const QStringList &args);

 private slots:
  void newConnectionData();
  void readyReadData(int id);
  void collectGarbageData();

 private:
  void ProcessCommand(int id);
  QTcpServer *cmd_server;
  QSignalMapper *cmd_read_mapper;
  QTimer *cmd_garbage_timer;
  std::map<int,QTcpSocket *> cmd_sockets;
  std::map<int,QString> cmd_recv_buffers;
  std::map<int,int> cmd_sources;
  std::map<int,QString> cmd_cmd_table;
  std::map<int,int> cmd_upper_table;
  std::map<int,int> cmd_lower_table;
  int cmd_source;
};


#endif  // STREAMCMDSERVER_H
