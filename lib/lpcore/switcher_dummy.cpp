// switcher_dummy.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "switcher_dummy.h"

Dummy::Dummy(int id,QObject *parent)
  : LPSwitcher(id,LPSwitcher::TypeDummy,parent)
{
  for(int i=0;i<DUMMY_OUTPUTS;i++) {
    dummy_pending_map[i]=i;
    dummy_xpoint_map[i]=i;
  }

  //
  // Timeout Timer
  //
  dummy_timeout_timer=new QTimer(this);
  dummy_timeout_timer->setSingleShot(true);
  connect(dummy_timeout_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
}


Dummy::~Dummy()
{
  delete dummy_timeout_timer;
}


bool Dummy::open(const QString &device)
{
  return true;
}


void Dummy::connectToHost(const QString &hostname,const QString &password,
			 uint16_t port)
{
}


int Dummy::inputs()
{
  return DUMMY_INPUTS;
}


int Dummy::outputs()
{
  return DUMMY_OUTPUTS;
}


int Dummy::gpis()
{
  return DUMMY_GPIS;
}


int Dummy::gpos()
{
  return DUMMY_GPOS;
}


int Dummy::lines()
{
  return DUMMY_LINES;
}


int Dummy::crosspoint(int output)
{
  return dummy_xpoint_map[output];
}


void Dummy::setCrosspoint(int output,int input)
{
  if((output<0)||(output>=DUMMY_OUTPUTS)||(input<0)||(input>=DUMMY_INPUTS)) {
    syslog(LOG_WARNING,"Dummy: request to set output %d to input %d ignored",
	   output,input);
    return;
  }
  dummy_pending_map[output]=input;
  dummy_timeout_timer->start(0);
}


bool Dummy::gpiState(int gpi)
{
  return false;
}


void Dummy::pulseGpo(int gpo)
{
}


bool Dummy::silenceSense(int chan)
{
  return false;
}


void Dummy::timeoutData()
{
  for(int i=0;i<DUMMY_OUTPUTS;i++) {
    if(dummy_xpoint_map[i]!=dummy_pending_map[i]) {
      dummy_xpoint_map[i]=dummy_pending_map[i];
      emit crosspointChanged(id(),i,dummy_xpoint_map[i]);
      fprintf(stderr,"LPSwitcher::Dummy: crosspoint %d changed to %d\n",
	      i+1,dummy_xpoint_map[i]+1);
    }
  }
}
