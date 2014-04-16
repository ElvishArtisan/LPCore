// lpautocue.h
//
// lpautocue(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpautoroute.h,v 1.2 2013/10/08 23:17:32 cvs Exp $
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

#ifndef LPAUTOCUE_H
#define LPAUTOCUE_H

#include <vector>

#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>
#include <lpcore/lpconnection.h>
#include <lpcore/lpmessage.h>
#include <lpcore/lpprofile.h>

#include "switch.h"

#define LPAUTOROUTE_USAGE "[-d]\n"
#define LPAUTOROUTE_CONF_FILE "/etc/lp/lpautoroute.conf"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void serverMessageReceivedData(const LPMessage &msg);

 private:
  std::vector<Switch *> lp_switches;
  LPConnection *lp_connection;
  LPProfile *lp_profile;
};


#endif  // LPAUTOCUE_H
