// lpmiccontrol.h
//
// lpmiccontrol(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpmiccontrol.h,v 1.1 2013/11/21 22:31:46 cvs Exp $
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

#ifndef LPMICCONTROL_H
#define LPMICCONTROL_H

#include <vector>

#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>
#include <lpcore/lpconnection.h>
#include <lpcore/lpmessage.h>
#include <lpcore/lpprofile.h>

#include "mic.h"

#define LPMICCONTROL_USAGE "[-d]\n"
#define LPMICCONTROL_CONF_FILE "/etc/lp/lpmiccontrol.conf"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void serverMessageReceivedData(const LPMessage &msg);

 private:
  void ProcessMic(Mic *m,bool state,const LPMessage &msg);
  void ProcessTalkback(Mic *m,bool state,const LPMessage &msg);
  void ProcessTurret(Mic *m,int turret,Mic::Function func,bool state,
		     const LPMessage &msg);
  std::vector<Mic *> lp_mics;
  LPConnection *lp_connection;
  LPProfile *lp_profile;
};


#endif  // LPMICCONTROL_H
