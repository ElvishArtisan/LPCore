// switcher_sas16000.h
//
// LPSwitcher implementation for the BroadcastTools SS8.2
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SWITCHER_SAS16000_H
#define SWITCHER_SAS16000_H

#include <QtCore/QTimer>

#include "lpswitcher.h"
#include "lpttydevice.h"

#define SAS16000_INPUTS 32
#define SAS16000_OUTPUTS 32
#define SAS16000_SPEED 9600
#define SAS16000_WORD_LENGTH 8
#define SAS16000_PARITY LPTTYDevice::None
#define SAS16000_FLOW_CONTROL LPTTYDevice::FlowNone
#define SAS16000_POLL_INTERVAL 500

class Sas16000 : public LPSwitcher
{
 Q_OBJECT;
 public:
  Sas16000(int id,QObject *parent=0);
  ~Sas16000();
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

 private:
  void ProcessStatus(const QString &buf);
  int bt_crosspoints[SAS16000_OUTPUTS];
  LPTTYDevice *bt_device;
  QString bt_buffer;
  bool bt_xmap;
};


#endif  // SWITCHER_SAS16000_H
