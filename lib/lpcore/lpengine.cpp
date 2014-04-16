// lpengine.cpp
//
// Abstract an Audio Engine state.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpengine.cpp,v 1.2 2013/08/13 23:58:15 cvs Exp $
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

#include <stdio.h>

#include "lpengine.h"

LPEngine::LPEngine(unsigned engine)
{
  eng_engine=engine;
}


LPEngine::~LPEngine()
{
}


int LPEngine::engine() const
{
  return eng_engine;
}


int LPEngine::sourceDevice(int chan)
{
  return eng_source_devices[chan];
}


void LPEngine::setSourceDevice(int chan,int device)
{
  eng_source_devices[chan]=device;
}


bool LPEngine::bussState(int device,int surface,int buss)
{
  return eng_buss_states[buss][GetChannel(device,surface)];
}


void LPEngine::setBussState(int device,int surface,int buss,bool state)
{
  eng_buss_states[buss][GetChannel(device,surface)]=state;
}


int LPEngine::faderLevel(int device,int surface)
{
  return eng_fader_levels[GetChannel(device,surface)];
}


void LPEngine::setFaderLevel(int device,int surface,int lvl)
{
  eng_fader_levels[GetChannel(device,surface)]=lvl;
}


LPMessage::ChannelMode LPEngine::mode(int device,int surface)
{
  return eng_modes[GetChannel(device,surface)];
}


void LPEngine::setMode(int device,int surface,LPMessage::ChannelMode mode)
{
  eng_modes[GetChannel(device,surface)]=mode;
}


int LPEngine::GetChannel(int src_device,int surface)
{
  if(src_device<0xFF) {
    return src_device;
  }
  for(std::map<int,int>::const_iterator it=eng_source_devices.begin();
      it!=eng_source_devices.end();it++) {
    if((it->second==src_device)&&
       (LPConfig::surfaceByChannel(it->first)==surface)) {
      return it->first;
    }
  }
  return 0;
}
