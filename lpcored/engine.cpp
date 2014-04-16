// engine.cpp
//
// Base Class for LP Engine Drivers
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: engine.cpp,v 1.14 2013/12/31 19:31:05 cvs Exp $
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

#include "engine.h"

SourceDevice::SourceDevice(int device)
{
  src_device=device;
  src_surface=0;
  src_mode=0;
  src_fader_level=0;
  for(int i=0;i<LPCORE_MAX_BUSSES;i++) {
    src_bus_states[i]=false;
  }
}


int SourceDevice::device() const
{
  return src_device;
}


void SourceDevice::setDevice(int dev)
{
  src_device=dev;
}


int SourceDevice::surface() const
{
  return src_surface;
}


void SourceDevice::setSurface(int surf)
{
  src_surface=surf;
}


int SourceDevice::mode() const
{
  return src_mode;
}


void SourceDevice::setMode(int mode)
{
  src_mode=mode;
}


int SourceDevice::faderLevel() const
{
  return src_fader_level;
}


void SourceDevice::setFaderLevel(int lvl)
{
  src_fader_level=lvl;
}


bool SourceDevice::busState(int bus) const
{
  return src_bus_states[bus];
}


void SourceDevice::setBusState(int bus,bool state)
{
  src_bus_states[bus]=state;
}


Engine::Engine(LPConfig *config,unsigned n,QObject *parent)
  : QObject(parent)
{
  engine_number=config->engineNumber(n);

  //
  // Channel Sources
  //
  for(int i=0;i<256;i++) {
    engine_source_devices.push_back(new SourceDevice(i));
  }
}


Engine::~Engine()
{
  for(unsigned i=0;i<engine_source_devices.size();i++) {
    delete engine_source_devices[i];
  }
}


int Engine::engineNumber() const
{
  return engine_number;
}


SourceDevice *Engine::sourceDevice(uint8_t chan)
{
  return engine_source_devices[chan];
}


SourceDevice *Engine::sourceDevice(int device,int surface)
{
  for(unsigned i=0;i<256;i++) {
    if((engine_source_devices[i]->device()==device)&&
       (engine_source_devices[i]->surface()==surface)) {
      return engine_source_devices[i];
    }
  }
  if(device<256) {
    return engine_source_devices[device];
  }
  return NULL;
}


void Engine::setSourceDevice(uint8_t chan,int device)
{
  engine_source_devices[chan]->setDevice(device);
  engine_source_devices[chan]->setSurface(LPConfig::surfaceByChannel(chan));
}


void Engine::setSourceMode(uint8_t chan,int mode)
{
  engine_source_devices[chan]->setMode(mode);
  engine_source_devices[chan]->setSurface(LPConfig::surfaceByChannel(chan));
}


void Engine::setSourceFaderLevel(uint8_t chan,int fader_lvl)
{
  engine_source_devices[chan]->setFaderLevel(fader_lvl);
  engine_source_devices[chan]->setSurface(LPConfig::surfaceByChannel(chan));
}


void Engine::setSourceBusState(uint8_t chan,int bus,bool state)
{
  engine_source_devices[chan]->setBusState(bus,state);
  engine_source_devices[chan]->setSurface(LPConfig::surfaceByChannel(chan));
}


int Engine::destinationChannel(int device,int surface)
{
  for(unsigned i=0;i<engine_source_devices.size();i++) {
    if((engine_source_devices[i]->device()==device)&&
       (engine_source_devices[i]->surface()==surface)) {
      return i;
    }
  }
  return -1;
}


QString Engine::typeString(Engine::Type type)
{
  QString ret=tr("Unknown");

  switch(type) {
  case Engine::LogitekAeType:
    ret=tr("Logitek Audio Engine");
    break;

  case Engine::SwitcherType:
    ret=tr("LPCore Switcher");
    break;
  }

  return ret;
}
