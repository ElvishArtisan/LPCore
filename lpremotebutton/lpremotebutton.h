// lpremotebutton.h
//
// lpremotebutton(1) LP Driver
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpremotebutton.h,v 1.1 2013/11/21 22:31:48 cvs Exp $
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

#ifndef LPREMOTEBUTTON_H
#define LPREMOTEBUTTON_H

#include <vector>

#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>
#include <lpcore/lpconnection.h>
#include <lpcore/lpmessage.h>
#include <lpcore/lpprofile.h>

#include "button.h"

#define LPREMOTEBUTTON_USAGE "[-d]\n"
#define LPREMOTEBUTTON_CONF_FILE "/etc/lp/lpremotebutton.conf"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void serverMessageReceivedData(const LPMessage &msg);

 private:
  void SendOn(Button *b,unsigned dst);
  void SendOff(Button *b,unsigned dst);
  void SendMomentary(Button *b,unsigned dst,int duration);
  std::vector<Button *> lp_buttons;
  LPConnection *lp_connection;
  LPProfile *lp_profile;
};


#endif  // LPREMOTEBUTTON_H
