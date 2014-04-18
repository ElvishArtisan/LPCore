// lpnetrouter.h
//
// LPNetRouter(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpnetrouter.h,v 1.2 2014/02/17 12:36:55 cvs Exp $
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

#ifndef LPNETROUTER_H
#define LPNETROUTER_H

#include <vector>

#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>

#include <lpcore/lpconfig.h>
#include <lpcore/lpconnection.h>
#include <lpcore/lpmessage.h>
#include <lpcore/lpprofile.h>
#include <lpcore/lpstreamcmdserver.h>
#include <lpcore/lpswitcherfactory.h>

#include "destination.h"
#include "source.h"

#define LPNETROUTER_USAGE "[-d]\n"
#define LPNETROUTER_CONF_FILE "/etc/lp/lpnetrouter.conf"
#define LPNETROUTER_DEFAULT_RML_PORT 5859
#define LPNETROUTER_PANEL_TCP_PORT 50317

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  enum PanelCommand {DC=0,GC=1,AX=2,SX=3};
  MainObject(QObject *parent=0);

 private slots:
  void crosspointChangedData(int id,int output,int input);
  void gpiChangedData(int id,int gpi,bool state);
  void silenceSenseChangedData(int id,int chan,bool state);
  void serverMessageReceivedData(const LPMessage &msg);
  void udpReadyReadData(int udp);
  void rmlReadyReadData();
  void commandReceivedData(int id,int cmd,const QStringList &args);
  void newConnectionData(int id,const QHostAddress &addr,uint16_t port);
  void resetRelayLampData(int engine,int device,int surface,int buss);

 private:
  QString Rewrite(const QString &msg,const QString &rule) const;
  bool SourceIsActive(int source,int except_dest) const;
  void LightLamp(Source *s,int relay);
  void SendRml(const QString &rml,unsigned dst,unsigned src,unsigned relay);
  void SendRml(const QString &rml) const;
  void ProcessRml(QStringList rml,const QHostAddress &src_addr,
		  uint16_t src_port);
  std::vector<Source *> lp_sources;
  std::vector<Destination *> lp_destinations;
  std::vector<QUdpSocket *> lp_udp_sockets;
  QUdpSocket *lp_rml_socket;
  int lp_rml_matrix_number;
  std::vector<QHostAddress> lp_rml_dst_addresses;
  std::vector<int> lp_rml_dst_ports;
  QSignalMapper *lp_udp_mapper;
  LPSwitcher *lp_main_switcher;
  LPConnection *lp_connection;
  LPProfile *lp_profile;
  LPStreamCmdServer *lp_server;
};


#endif  // LPNETROUTER_H
