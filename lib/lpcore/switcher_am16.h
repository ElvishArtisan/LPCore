// switcher_am16.h
//
// LPSwitcher implementation for the 360 Systems AM16
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_am16.h,v 1.4 2013/11/06 01:35:24 cvs Exp $
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

#ifndef SWITCHER_AM16_H
#define SWITCHER_AM16_H

#include <vector>

#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>

#include "lpswitcher.h"
#include "lpttydevice.h"

#define AM16_INPUTS 16
#define AM16_OUTPUTS 16
#define AM16_GPIS 0
#define AM16_GPOS 0
#define AM16_LINES 0
#define AM16_SYSEX_START 0xF0
#define AM16_SYSEX_END 0xF7
#define AM16_SYSTEMS_ID 0x1C          // 360 Systems
#define AM16_DEVICE_NUMBER 0x04       // AM16/B (rev 2.00 or greater)
#define AM16_DEVICE_ADDRESS 0x00      // Configured in the hardware
#define AM16_PATCH_NUMBER 0x00        // Program to use for crosspoint updates
#define AM16_TIMEOUT_INTERVAL 1000    // How long to wait for crosspoint map
#define AM16_POLL_INTERVAL 100        // Poll for crosspoint changes this often

class Am16 : public LPSwitcher
{
 Q_OBJECT;
 public:
  Am16(int id,QObject *parent=0);
  ~Am16();
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
  void readyReadData(int sock);
  void timeoutData();
  void pollData();

 private:
  void ProcessMessage(char *msg,int len);
  bool am_sysex_active;
  char am_data_buffer[1024];
  int am_data_ptr;
  std::vector<int> am_pending_inputs;
  std::vector<int> am_pending_outputs;
  int am_midi_socket;
  QSocketNotifier *am_notifier;
  QTimer *am_timeout_timer;
  int am_xpoint_map[AM16_OUTPUTS];
  QTimer *am_poll_timer;
  bool am_poll_pending;
};


#endif  // SWITCHER_AM16_H
