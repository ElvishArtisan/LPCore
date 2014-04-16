// lpcore.h
//
// lpcore(1) Monitor
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpcore.h,v 1.9 2013/08/09 23:19:50 cvs Exp $
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

#ifndef LPCORE_H
#define LPCORE_H

#include <QtCore/QTimer>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QTcpSocket>

#include <lpcore/lpconfig.h>
#include <lpcore/lpconnection.h>
#include <lpcore/lpmessage.h>

#define LPCORE_RECEIVE_COLOR "green"
#define LPCORE_SEND_COLOR "blue"
#define LPCORE_NOTATION_COLOR "black"
#define LPCORE_ERROR_COLOR "red"
#define LPCORE_KEYBOARD_SCAN_INTERVAL 100
#define LPCORE_USAGE "[options]\n\n--hostname=<hostname>\n     Connect to the vGuest/LP service at <hostname>.  Default value is\n     'localhost'.\n\n--port=<port-num>\n     Connect using TCP port <port-num>.  Default value is 10212.\n\n--username=<profile-name>\n     The name of the vGuest profile to use.  If not specified, lpcore(1)\n     will not perform automatic login processing.\n\n--password=<password>\n     Password for profile <profile-name>.\n\n--track-state\n     Enable state tracking\n"

class MainWidget : public QWidget
{
 Q_OBJECT;
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void contextChangedData(int n);
  void returnPressedData();
  void messageReceivedData(const LPMessage &msg);
  void watchdogStateChangedData(bool state);
  void loggedInData(LPConnection::Result res);
  void socketErrorData(QAbstractSocket::SocketError err,const QString &str);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void WriteToHistory(const QString &text);
  void UpdateWindowTitle();
  QLabel *lp_context_label;
  QComboBox *lp_context_box;
  QPushButton *lp_clear_button;
  QLineEdit *lp_send_edit;
  QTextEdit *lp_recv_edit;
  LPConnection *lp_connection;
};


#endif  // LPCORE_H
