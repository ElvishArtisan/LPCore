// devices.cpp
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

#include <syslog.h>

#include <lpcore/lpswitcherfactory.h>

#include "devices.h"

Devices::Devices(LPProfile *p,QObject *parent)
{
  dev_input_switcher_device=p->stringValue("Devices","InputSwitcherDevice");
  dev_input_switcher=
    LPSwitcherFactory(0,(LPSwitcher::Type)
		      p->intValue("Devices","InputSwitcherType",-1));

  dev_output_switcher_device=p->stringValue("Devices","OutputSwitcherDevice");
  if(dev_output_switcher_device==dev_input_switcher_device) {
    dev_output_switcher=dev_input_switcher;
  }
  else {
    dev_output_switcher=
      LPSwitcherFactory(0,(LPSwitcher::Type)
			p->intValue("Devices","OutputSwitcherType",-1));
  }
  dev_gpio_device_device=p->stringValue("Devices","GpioDevice");
  if(dev_gpio_device_device==dev_input_switcher_device) {
    dev_gpio_device=dev_input_switcher;
  }
  else {
    if(dev_gpio_device_device==dev_output_switcher_device) {
      dev_gpio_device=dev_output_switcher;
    }
    else {
      dev_gpio_device=
	LPSwitcherFactory(0,(LPSwitcher::Type)
			  p->intValue("Devices","GpioType",-1));
    }
  }
}


Devices::~Devices()
{
  if(dev_gpio_device!=NULL) {
    if((dev_gpio_device!=dev_input_switcher)&&
       (dev_gpio_device!=dev_output_switcher)) {
      delete dev_gpio_device;
    }
  }
  if(dev_output_switcher!=NULL) {
    if(dev_output_switcher!=dev_input_switcher) {
      delete dev_output_switcher;
    }
  }
  if(dev_input_switcher!=NULL) {
    delete dev_input_switcher;
  }
}


bool Devices::open()
{
  if(!dev_input_switcher->open(dev_input_switcher_device)) {
    syslog(LOG_ERR,"unable to open switcher device at \"%s\"",
	   (const char *)dev_input_switcher_device.toUtf8());
    return false;
  }
  if(dev_output_switcher!=dev_input_switcher) {
    if(!dev_output_switcher->open(dev_output_switcher_device)) {
      syslog(LOG_ERR,"unable to open switcher device at \"%s\"",
	     (const char *)dev_output_switcher_device.toUtf8());
      return false;
    }
  }
  if((dev_gpio_device!=dev_input_switcher)&&
     (dev_gpio_device!=dev_output_switcher)) {
    if(!dev_gpio_device->open(dev_gpio_device_device)) {
      syslog(LOG_ERR,"unable to open switcher device at \"%s\"",
	     (const char *)dev_gpio_device_device.toUtf8());
      return false;
    }
  }

  return true;
}


LPSwitcher *Devices::inputSwitcher() const
{
  return dev_input_switcher;
}


LPSwitcher *Devices::outputSwitcher() const
{
  return dev_output_switcher;
}


LPSwitcher *Devices::gpioDevice() const
{
  return dev_gpio_device;
}
