// devices.h
//
// Abstract an LPCodecPool Device Set
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

#ifndef DEVICES_H
#define DEVICES_H

#include <QtCore/QObject>

#include <lpcore/lpprofile.h>
#include <lpcore/lpswitcher.h>

class Devices
{
 public:
  Devices(LPProfile *p,QObject *parent=0);
  ~Devices();
  bool open();
  LPSwitcher *inputSwitcher() const;
  LPSwitcher *outputSwitcher() const;
  LPSwitcher *gpioDevice() const;

 private:
  LPSwitcher *dev_input_switcher;
  QString dev_input_switcher_device;
  LPSwitcher *dev_output_switcher;
  QString dev_output_switcher_device;
  LPSwitcher *dev_gpio_device;
  QString dev_gpio_device_device;
};


#endif  // DEVICES_H
