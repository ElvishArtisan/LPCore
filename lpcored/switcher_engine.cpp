// switcher_engine.cpp
//
// Engine Driver for LPCore Switcher Devices
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_engine.cpp,v 1.1 2013/12/31 19:31:06 cvs Exp $
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

#include <lpcore/lpswitcherfactory.h>

#include "switcher_engine.h"

SwitcherEngine::SwitcherEngine(LPConfig *config,unsigned n,QObject *parent)
  : Engine(config,n,parent)
{
  switch_is_reset=false;

  switch_device=config->engineSerialDevice(n);

  switch_parser=new LPParser(this);
  connect(switch_parser,SIGNAL(messageReceived(const LPMessage &)),
	  this,SLOT(messageReceivedData(const LPMessage &)));

  switch_switcher=
    LPSwitcherFactory(0,(LPSwitcher::Type)config->engineSwitcherId(n),parent);
  connect(switch_switcher,SIGNAL(gpiChanged(int,int,bool)),
	  this,SLOT(gpiChangedData(int,int,bool)));
}


SwitcherEngine::~SwitcherEngine()
{
  delete switch_switcher;
}


Engine::Type SwitcherEngine::type() const
{
  return Engine::SwitcherType;
}


bool SwitcherEngine::isReset() const
{
  return switch_is_reset;
}


bool SwitcherEngine::open()
{
  return switch_switcher->open(switch_device);
}


int SwitcherEngine::write(const QByteArray &data) const
{
  switch_parser->writeData(data);
  return data.size();
}


int SwitcherEngine::write(const char *data,int len) const
{
  return write(QByteArray(data,len));
}


void SwitcherEngine::reset()
{
  for(int i=0;i<switch_switcher->gpis();i++) {
    gpiChangedData(0,i,switch_switcher->gpiState(i));
  }
  switch_is_reset=true;
  emit resetFinished(engineNumber(),true);
}


void SwitcherEngine::messageReceivedData(const LPMessage &msg)
{
  // printf("MSG: %s\n",(const char *)msg.dump().toAscii());

  switch(0xFF&msg[2]) {
  case 0xA2:   // Momentary GPIO
    if(((0xFF&msg[3])==SWITCHER_ENGINE_GPO_CHAN)&&
       ((0xFF&msg[4])>0)&&((0xFF&msg[4])<switch_switcher->gpos())&&
       ((0xFF&msg[5])>0)) {
      switch_switcher->pulseGpo(0xFF&msg[4]);
    }
    break;

  case 0xB2:   // Turn ON
    if(((0xFF&msg[3])==SWITCHER_ENGINE_GPO_CHAN)&&
       ((0xFF&msg[4])>0)&&((0xFF&msg[4])<switch_switcher->gpos())) {
      // FIXME: Implement ON support in LPSwitcher
    }
    break;

  case 0xB3:   // Turn OFF
    if(((0xFF&msg[3])==SWITCHER_ENGINE_GPO_CHAN)&&
       ((0xFF&msg[4])>0)&&((0xFF&msg[4])<switch_switcher->gpos())) {
      // FIXME: Implement OFF support in LPSwitcher
    }
    break;
  }
}


void SwitcherEngine::gpiChangedData(int id,int line,bool state)
{
  char msg[255];

  msg[0]=0x02;
  msg[1]=0x03;
  if(state) {
    msg[2]=0xB2;
  }
  else {
    msg[2]=0xB3;
  }
  msg[3]=SWITCHER_ENGINE_GPI_CHAN;
  msg[4]=0xFF&(line+1);
  emit messageReceived(engineNumber(),QByteArray(msg,5));
}
