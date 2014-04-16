// switcher_gpio.h
//
// LPSwitcher implementation for GPIO driver based devices.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_gpio.h,v 1.1 2013/09/09 16:42:57 cvs Exp $
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

#ifndef SWITCHER_GPIO_H
#define SWITCHER_GPIO_H

#include <stdint.h>

#include <vector>

#include <QtCore/QTimer>
#include <QtCore/QSignalMapper>

#include "lpswitcher.h"
#include "lpttydevice.h"

#define GPIO_INPUTS 0
#define GPIO_OUTPUTS 0
#define GPIO_LINES 0
#define GPIO_POLL_INTERVAL 100
#define GPIO_PULSE_INTERVAL 300

class Gpio : public LPSwitcher
{
 Q_OBJECT;
 public:
  Gpio(int id,QObject *parent=0);
  ~Gpio();
  bool open(const QString &device);
  void connectToHost(const QString &hostname,const QString &password,
		     uint16_t port);
  int inputs();
  int outputs();
  int gpis();
  int gpos();
  int lines();
  bool gpiState(int gpi);
  void pulseGpo(int gpo);

 private slots:
  void gpoData(int gpo);
  void pollData();

 private:
  bool ProbeCard(int fd);
  int gpio_fd;
  int gpio_gpis;
  int gpio_gpos;
  QString gpio_name;
  QSignalMapper *gpio_gpo_mapper;
  QTimer *gpio_poll_timer;
  std::vector<bool> gpio_gpi_states;
  std::vector<QTimer *> gpio_gpo_timers;
};


#endif  // SWITCHER_GPIO_H
