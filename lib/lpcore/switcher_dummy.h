// switcher_dummy.h
//
// Dummy LPSwitcher implementation
//
//   (C) Copyright 2013-2014 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SWITCHER_DUMMY_H
#define SWITCHER_DUMMY_H

#include <vector>

#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>

#include "lpswitcher.h"
#include "lpttydevice.h"

#define DUMMY_INPUTS 16
#define DUMMY_OUTPUTS 16
#define DUMMY_GPIS 0
#define DUMMY_GPOS 0
#define DUMMY_LINES 0

class Dummy : public LPSwitcher
{
 Q_OBJECT;
 public:
  Dummy(int id,QObject *parent=0);
  ~Dummy();
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
  void timeoutData();

 private:
  int dummy_pending_map[DUMMY_OUTPUTS];
  int dummy_xpoint_map[DUMMY_OUTPUTS];
  QTimer *dummy_timeout_timer;
};


#endif  // SWITCHER_DUMMY_H
