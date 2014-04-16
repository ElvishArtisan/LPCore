// lppanel.h
//
// Switcher Control Panel Applet for LPNetRouter
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
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


#ifndef LPPANEL_H
#define LPPANEL_H

#include <vector>

#include <QtCore/QSignalMapper>
#include <QtGui/QPalette>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>

#include "pushbutton.h"

#define LPPANEL_USAGE "[options]\n\nThe following options are available:\n\n--output=<output-num>\n     Use output number <output-num>.\n\n--hostname=<host-name>\n     Connect to the LPNetRouter panel instance at <host-name>.\n\n--arm=<input-num>\n     Arm the <input-num> input.\n"
#define LPPANEL_BUTTON_WIDTH 80
#define LPPANEL_BUTTON_HEIGHT 80
#define LPPANEL_BUTTON_COLOR Qt::green

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
  void errorData(QAbstractSocket::SocketError err);
  void buttonClickedData(int n);
  void watchdogStatusChangedData(int id,bool state);

 private:
  void ProcessMessage(const QStringList &msg);
  void SendToServer(const QString &msg);
  QPixmap *lp_lpanels_map;
  LPConfig *lp_config;
  bool lp_first_connection;
  QSignalMapper *lp_button_mapper;
  std::vector<PushButton *> lp_buttons;
  QPalette lp_active_button_palette;
  int lp_active_source;
  int lp_armed_source;
  int lp_initial_armed_source;
  int lp_inputs;
  int lp_output_number;
  int lp_active_input;
  int lp_armed_input;
  QTcpSocket *lp_socket;
  QString lp_socket_buffer;
};


#endif  // LPPANEL_H
