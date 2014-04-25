// switcher_gpio.cpp
//
// LPSwitcher implementation for GPIO driver based devices.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_gpio.cpp,v 1.2 2013/09/09 18:21:54 cvs Exp $
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <errno.h>

#ifdef HAVE_MC_GPIO
#include <linux/gpio.h>
#endif  // HAVE_MC_GPIO

#include "switcher_gpio.h"

Gpio::Gpio(int id,QObject *parent)
  : LPSwitcher(id,LPSwitcher::TypeGpio,parent)
{
#ifdef HAVE_MC_GPIO
  gpio_fd=-1;
  gpio_gpis=0;
  gpio_gpos=0;

  gpio_gpo_mapper=new QSignalMapper(this);
  connect(gpio_gpo_mapper,SIGNAL(mapped(int)),this,SLOT(gpoData(int)));

  gpio_poll_timer=new QTimer(this);
  connect(gpio_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
#endif  // HAVE_MC_GPIO
}


Gpio::~Gpio()
{
#ifdef HAVE_MC_GPIO
  if(gpio_fd>=0) {
    close(gpio_fd);
  }
  for(unsigned i=0;i<gpio_gpo_timers.size();i++) {
    delete gpio_gpo_timers[i];
  }
  delete gpio_poll_timer;
  delete gpio_gpo_mapper;
#endif  // HAVE_MC_GPIO
}


bool Gpio::open(const QString &device)
{
#ifdef HAVE_MC_GPIO
  if((gpio_fd=::open(device.toAscii(),O_RDONLY|O_NONBLOCK))<0) {
    syslog(LOG_WARNING,"unable to open GPIO device at \"%s\" [%s]",
	   (const char *)device.toAscii(),strerror(errno));
    return false;
  }

  if(!ProbeCard(gpio_fd)) {
    syslog(LOG_WARNING,"unable to probe GPIO device at \"%s\" [%s]",
	   (const char *)device.toAscii(),strerror(errno));
    close(gpio_fd);
    return false;
  }

  for(int i=0;i<gpio_gpos;i++) {
    gpio_gpi_states.push_back(false);

    gpio_gpo_timers.push_back(new QTimer(this));
    gpio_gpo_timers.back()->setSingleShot(true);
    connect(gpio_gpo_timers.back(),SIGNAL(timeout()),
	    gpio_gpo_mapper,SLOT(map()));
    gpio_gpo_mapper->setMapping(gpio_gpo_timers.back(),i);
  }

  syslog(LOG_DEBUG,"opened GPIO device at \"%s\" [%s]",
	 (const char *)device.toAscii(),(const char *)gpio_name.toAscii());
  gpio_poll_timer->start(GPIO_POLL_INTERVAL);

  return true;
#else
  return false;
#endif  // HAVE_MC_GPIO
}


void Gpio::connectToHost(const QString &hostname,const QString &password,
			 uint16_t port)
{
}


int Gpio::inputs()
{
#ifdef HAVE_MC_GPIO
  return GPIO_INPUTS;
#else
  return 0;
#endif  // HAVE_MC_GPIO
}


int Gpio::outputs()
{
#ifdef HAVE_MC_GPIO
  return GPIO_OUTPUTS;
#else
  return 0;
#endif  // HAVE_MC_GPIO
}


int Gpio::gpis()
{
#ifdef HAVE_MC_GPIO
  return gpio_gpis;
#else
  return 0;
#endif  // HAVE_MC_GPIO
}


int Gpio::gpos()
{
#ifdef HAVE_MC_GPIO
  return gpio_gpos;
#else
  return 0;
#endif  // HAVE_MC_GPIO
}


int Gpio::lines()
{
#ifdef HAVE_MC_GPIO
  return GPIO_LINES;
#else
  return 0;
#endif  // HAVE_MC_GPIO
}


bool Gpio::gpiState(int gpi)
{
#ifdef HAVE_MC_GPIO
  return gpio_gpi_states[gpi];
#else
  return false;
#endif  // HAVE_MC_GPIO
}


void Gpio::pulseGpo(int gpo)
{
#ifdef HAVE_MC_GPIO
  struct gpio_line line;

  if(gpio_gpo_timers[gpo]->isActive()) {
    return;
  }
  line.line=gpo;
  line.state=1;
  if(ioctl(gpio_fd,GPIO_SET_OUTPUT,&line)<0) {
    syslog(LOG_WARNING,"unable to set GPIO output %d [%s]",gpo+1,
	   strerror(errno));
  }
  gpio_gpo_timers[gpo]->start(GPIO_PULSE_INTERVAL);
#endif  // HAVE_MC_GPIO
}


void Gpio::gpoData(int gpo)
{
#ifdef HAVE_MC_GPIO
  struct gpio_line line;

  line.line=gpo;
  line.state=0;
  if(ioctl(gpio_fd,GPIO_SET_OUTPUT,&line)<0) {
    syslog(LOG_WARNING,"unable to clear GPIO output %d [%s]",gpo+1,
	   strerror(errno));
  }
#endif  // HAVE_MC_GPIO
}


void Gpio::pollData()
{
#ifdef HAVE_MC_GPIO
  struct gpio_mask mask;
  unsigned gpi=0;

  if(ioctl(gpio_fd,GPIO_GET_INPUTS,&mask)<0) {
    return;
  }
  for(unsigned i=0;i<4;i++) {
    for(unsigned j=0;(j<8*sizeof(unsigned));j++) {
      if((gpi=i*8*sizeof(unsigned)+j)>=gpio_gpi_states.size()) {
	return;
      }
      bool state=((mask.mask[i]>>j)&0x01)==0x01;
      if(state!=gpio_gpi_states[gpi]) {
	gpio_gpi_states[gpi]=state;
	emit gpiChanged(id(),gpi,state);
      }
    }
  }
#endif  // HAVE_MC_GPIO
}


bool Gpio::ProbeCard(int fd)
{
#ifdef HAVE_MC_GPIO
  struct gpio_info info;

  memset(&info,0,sizeof(info));

  if(ioctl(fd,GPIO_GETINFO,&info)<0) {
    return false;
  }
  gpio_gpis=info.inputs;
  gpio_gpos=info.outputs;
  gpio_name=info.name;

  return true;
#else
  return false;
#endif  // HAVE_MC_GPIO
}
