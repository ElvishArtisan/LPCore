// lpcodecpanel.h
//
// Control Panel Applet for LPCodecPool
//
//   (C) Copyright 2013-2014 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of
//   the License, or (at your option) any later version.
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


#ifndef LPCODECPANEL_H
#define LPCODECPANEL_H

#include <vector>

#include <QtGui/QWidget>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>

#include "codecwidget.h"

#define LPCODECPANEL_USAGE "[options]\n\nThe following options are available:\n\n--hostname=<host-name>\n     Connect to the LPCodecPool server instance at <host-name>.\n"
#define LPCODECPANEL_BUTTON_WIDTH 80
#define LPCODECPANEL_BUTTON_HEIGHT 80
#define LPCODECPANEL_BUTTON_COLOR Qt::green

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  ~MainWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void connectedData();
  void readyReadData();
  void codecChangedData(unsigned port_num,unsigned codec_num);
  void errorData(QAbstractSocket::SocketError err);
  void watchdogStatusChangedData(int id,bool state);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void ProcessMessage(const QStringList &msg);
  void SendToServer(const QString &msg);
  std::vector<CodecWidget *> lp_codec_widgets;
  QTcpSocket *lp_socket;
  QString lp_socket_buffer;
};


#endif  // LPCODECPANEL_H
