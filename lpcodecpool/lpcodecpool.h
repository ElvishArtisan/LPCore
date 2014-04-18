// lpcodecpool.h
//
// LPCodecPool(1) LP Application
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

#ifndef LPCODECPOOL_H
#define LPCODECPOOL_H

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

#include "codecs.h"
#include "devices.h"
#include "rooms.h"

#define LPCODECPOOL_USAGE "[-d]\n"
#define LPCODECPOOL_CONF_FILE "/etc/lp/lpcodecpool.conf"
#define LPCODECPOOL_PANEL_TCP_PORT 50935

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  enum PanelCommand {DC=0,GC=1,SX=2};
  MainObject(QObject *parent=0);

 private slots:
  void inputCrosspointChangedData(int id,int output,int input);
  void outputCrosspointChangedData(int id,int output,int input);
  void gpiChangedData(int id,int line,bool state);
  void commandReceivedData(int id,int cmd,const QStringList &args);
  void newConnectionData(int id,const QHostAddress &addr,uint16_t port);

 private:
  void UpdateClients();
  void UpdateClient(int id);
  Codecs *lp_codecs;
  Devices *lp_devices;
  Rooms *lp_rooms;
  LPProfile *lp_profile;
  LPStreamCmdServer *lp_server;
};


#endif  // LPCODECPOOL_H
