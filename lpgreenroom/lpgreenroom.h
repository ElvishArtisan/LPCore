// lpgreenroom.h
//
// lpgreenroom(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpgreenroom.h,v 1.1 2013/11/21 22:31:45 cvs Exp $
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

#ifndef LPGREENROOM_H
#define LPGREENROOM_H

#include <vector>

#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>
#include <lpcore/lpconnection.h>
#include <lpcore/lpmessage.h>
#include <lpcore/lpprofile.h>

#include "room.h"

#define LPGREENROOM_USAGE "[-d]\n"
#define LPGREENROOM_CONF_FILE "/etc/lp/lpgreenroom.conf"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void serverMessageReceivedData(const LPMessage &msg);
  void serverWatchdogStateChangedData(bool state);

 private:
  Room *GetRoom(int engine,int surface) const;
  Room *GetRoom(int engine,int surface,int device) const;
  std::vector<Room *> lp_rooms;
  LPConnection *lp_connection;
  LPProfile *lp_profile;
};


#endif  // LPGREENROOM_H
