// dialer.h
//
// Dialer Widget
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: dialer.h,v 1.3 2013/09/07 00:30:55 cvs Exp $
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

#ifndef DIALER_H
#define DIALER_H

#include <vector>

#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QWidget>

#include <lpcore/lpconfig.h>
#include <lpcore/lpswitcher.h>

#include "statuslight.h"

class Dialer : public QWidget
{
 Q_OBJECT;
 public:
  Dialer(LPSwitcher *switcher,QWidget *parent=0);
  ~Dialer();
  QSize sizeHint() const;

 private slots:
  void dialData(int line);
  void xmtAlgorithmActivatedData(int n);
  void rcvAlgorithmActivatedData(int n);
  void channelRateActivatedData(int n);
  void sampleRateActivatedData(int n);
  void lineStateChangedData(int line,LPSwitcher::LineState state);
  void lockChangedData(int line,bool state);
  void dialedStringChangedData(int line,const QString &str);
  void xmtAlgorithmChangedData(LPSwitcher::XmtAlgo algo);
  void rcvAlgorithmChangedData(LPSwitcher::RcvAlgo algo);
  void channelRateChangedData(LPSwitcher::ChannelRate rate);
  void sampleRateChangedData(int rate);

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  std::vector<QLabel *> dial_state_labels;
  std::vector<QLabel *> dial_dialed_string_labels;
  std::vector<QPushButton *> dial_dial_buttons;
  std::vector<QLabel *> dial_lock_labels;
  std::vector<StatusLight *> dial_lock_lights;
  QLabel *dial_xmt_mode_label;
  QLineEdit *dial_xmt_mode_edit;
  QComboBox *dial_xmt_mode_box;
  QLabel *dial_rcv_mode_label;
  QLineEdit *dial_rcv_mode_edit;
  QComboBox *dial_rcv_mode_box;
  QLabel *dial_channel_rate_label;
  QLineEdit *dial_channel_rate_edit;
  QComboBox *dial_channel_rate_box;
  QLabel *dial_sample_rate_label;
  QLineEdit *dial_sample_rate_edit;
  QComboBox *dial_sample_rate_box;
  QLabel *dial_dial_label;
  QLineEdit *dial_dial_edit;
  LPSwitcher *dial_switcher;
};


#endif  // DIALER_H
