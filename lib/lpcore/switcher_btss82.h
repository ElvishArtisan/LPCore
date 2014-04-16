// switcher_btss82.h
//
// LPSwitcher implementation for the BroadcastTools SS8.2
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_btss82.h,v 1.7 2014/03/10 22:21:01 cvs Exp $
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

#ifndef SWITCHER_BTSS82_H
#define SWITCHER_BTSS82_H

#include <QtCore/QTimer>

#include "lpswitcher.h"
#include "lpttydevice.h"

#define BTSS82_INPUTS 8
#define BTSS82_OUTPUTS 2
#define BTSS82_GPIS 16
#define BTSS82_GPOS 8
#define BTSS82_LINES 0
#define BTSS82_SPEED 19200
#define BTSS82_WORD_LENGTH 8
#define BTSS82_PARITY LPTTYDevice::None
#define BTSS82_FLOW_CONTROL LPTTYDevice::FlowNone
#define BTSS82_POLL_INTERVAL 500
#define BTSS82_DEVICE_ID 0

class BtSs82 : public LPSwitcher
{
 Q_OBJECT;
 public:
  BtSs82(int id,QObject *parent=0);
  ~BtSs82();
  bool open(const QString &device);
  void connectToHost(const QString &hostname,const QString &password,
		     uint16_t port);
  int inputs();
  int outputs();
  int gpis();
  int gpos();
  int lines();
  int crosspoint(int output);
  void setCrosspoint(int output,int input);
  bool gpiState(int gpi);
  void pulseGpo(int gpo);
  bool silenceSense(int chan);

 private slots:
  void readyReadData();
  void pollData();

 private:
  void ProcessStatus(const QString &buf);
  void ProcessPipStatus(const QString &buf);
  void ProcessCrosspointStatus(const QString &buf);
  void ProcessSilenceSenseStatus(const QString &buf);
  int bt_crosspoints[BTSS82_OUTPUTS];
  int bt_gpi_states[BTSS82_GPIS];
  bool bt_silence_sense_states[2];
  LPTTYDevice *bt_device;
  QTimer *bt_poll_timer;
  unsigned bt_poll_counter;
  QString bt_buffer;
};


#endif  // SWITCHER_BTSS82_H
