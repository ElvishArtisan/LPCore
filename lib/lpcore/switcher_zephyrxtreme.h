// switcher_zephyrxtreme.h
//
// LPSwitcher implementation for the Telos Zephyr Xtreme
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_zephyrxtreme.h,v 1.1 2013/09/07 00:30:55 cvs Exp $
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

#ifndef SWITCHER_ZEPHYRXTREME_H
#define SWITCHER_ZEPHYRXTREME_H

#include <QtCore/QTimer>
#include <QtNetwork/QTcpSocket>

#include "lpswitcher.h"
#include "lpttydevice.h"

#define ZEPHYRXTREME_INPUTS 0
#define ZEPHYRXTREME_OUTPUTS 0
#define ZEPHYRXTREME_GPIS 0
#define ZEPHYRXTREME_GPOS 0
#define ZEPHYRXTREME_LINES 2
#define ZEPHYRXTREME_SPEED 9600
#define ZEPHYRXTREME_WORD_LENGTH 8
#define ZEPHYRXTREME_PARITY LPTTYDevice::None
#define ZEPHYRXTREME_FLOW_CONTROL LPTTYDevice::FlowNone
#define ZEPHYRXTREME_POLL_INTERVAL 2000
#define ZEPHYRXTREME_DEFAULT_PASSWORD "Telos"
#define ZEPHYRXTREME_DEFAULT_TCP_PORT 23

class ZephyrXtreme : public LPSwitcher
{
 Q_OBJECT;
 public:
  ZephyrXtreme(int id,QObject *parent=0);
  ~ZephyrXtreme();
  bool open(const QString &device);
  void connectToHost(const QString &hostname,const QString &password,
		     uint16_t port);
  int inputs();
  int outputs();
  int gpis();
  int gpos();
  int lines();
  bool xmtSupports(LPSwitcher::XmtAlgo algo) const;
  bool rcvSupports(LPSwitcher::RcvAlgo algo) const;
  LPSwitcher::XmtAlgo xmtAlgorithm() const;
  void setXmtAlgorithm(LPSwitcher::XmtAlgo algo);
  LPSwitcher::RcvAlgo rcvAlgorithm() const;
  void setRcvAlgorithm(LPSwitcher::RcvAlgo algo);
  LPSwitcher::ChannelRate channelRate() const;
  void setChannelRate(LPSwitcher::ChannelRate rate);
  int sampleRate() const;
  void setSampleRate(int rate);
  int crosspoint(int output);
  void setCrosspoint(int output,int input);
  bool gpiState(int gpi);
  void pulseGpo(int gpo);
  bool silenceSense(int chan);
  void dialLine(int line,const QString &str);
  void dropLine(int line);
  LPSwitcher::LineState lineState(int line);
  QString dialedString(int line) const;
  bool isLocked() const;

 private slots:
  void connectedData();
  void disconnectedData();
  void socketReadyRead();
  void readyReadData();
  void pollData();

 private:
  void ProcessStatus(const QString &str);
  void SetState(int line,LPSwitcher::LineState state);
  void SetXmtAlgorithm(LPSwitcher::XmtAlgo algo);
  void SetRcvAlgorithm(LPSwitcher::RcvAlgo algo);
  void SetChannelRate(LPSwitcher::ChannelRate rate);
  void SetSampleRate(int rate);
  void SendRates();
  void WriteToDevice(const QString &cmd);
  LPSwitcher::LineState zep_line_state[2];
  QString zep_dialed_string[2];
  QString zep_pending_string[2];
  bool zep_locked;
  LPSwitcher::XmtAlgo zep_xmt_algo;
  LPSwitcher::RcvAlgo zep_rcv_algo;
  LPSwitcher::ChannelRate zep_channel_rate;
  LPSwitcher::ChannelRate zep_pending_channel_rate;
  int zep_sample_rate;
  int zep_pending_sample_rate;
  QString zep_hostname;
  QString zep_password;
  uint16_t zep_port;
  QTcpSocket *zep_socket;
  LPTTYDevice *zep_device;
  QTimer *zep_poll_timer;
  QString zep_buffer;
};


#endif  // SWITCHER_ZEPHYRXTREME_H
