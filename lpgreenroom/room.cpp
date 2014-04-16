// room.cpp
//
// Abstract a Green Room
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: room.cpp,v 1.1 2013/11/21 22:31:45 cvs Exp $
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

#include <lpcore/lpconfig.h>

#include "room.h"

Room::Room(LPProfile *p)
{
  clear();
  room_profile=p;
}


int Room::engine() const
{
  return room_engine;
}


int Room::surface() const
{
  return room_surface;
}


int Room::programBuss() const
{
  return room_program_buss;
}


int Room::greenroomBuss() const
{
  return room_greenroom_buss;
}


unsigned Room::devices() const
{
  return room_device_numbers.size();
}


int Room::deviceNumber(unsigned n) const
{
  return room_device_numbers[n];
}


int Room::deviceNumberByChannel(uint8_t chan)
{
  return room_channel_sources[chan];
}


int Room::mixMinus(unsigned n) const
{
  return room_mix_minuses[n];
}


int Room::mixMinusByDevice(int device) const
{
  for(unsigned i=0;i<room_device_numbers.size();i++) {
    if(room_device_numbers[i]==device) {
      return room_mix_minuses[i];
    }    
  }
  return -1;
}


void Room::setDeviceNumberByChannel(uint8_t chan,int device)
{
  if(LPConfig::surfaceByChannel(chan)==room_surface) {
    room_channel_sources[chan]=device;
  }
}


bool Room::buss0StateByDevice(int device)
{
  for(unsigned i=0;i<room_device_numbers.size();i++) {
    if(room_device_numbers[i]==device) {
      return room_buss0_states[i];
    }
  }
  return false;
}


void Room::setBuss0StateByDevice(int device,bool state)
{
  for(unsigned i=0;i<room_device_numbers.size();i++) {
    if(room_device_numbers[i]==device) {
      room_buss0_states[i]=state;
    }
  }
}


bool Room::belongsTo(int engine,int surface) const
{
  return (engine==room_engine)&&(surface==room_surface);
}


bool Room::belongsTo(int engine,int surface,int device) const
{
  if(!belongsTo(engine,surface)) {
    return false;
  }
  for(unsigned i=0;i<room_device_numbers.size();i++) {
    if(device==room_device_numbers[i]) {
      return true;
    }
  }
  return false;
}


void Room::clear()
{
  room_engine=-1;
  room_surface=-1;
  room_program_buss=-1;
  room_greenroom_buss=-1;
  room_device_numbers.clear();
  room_buss0_states.clear();
  room_mix_minuses.clear();
}


bool Room::load(unsigned n)
{
  bool ok=false;
  QString section=QString().sprintf("Room%u",n+1);
  int count=0;
  int device=0;

  room_engine=room_profile->intValue(section,"Engine",0,&ok);
  if(!ok) {
    return false;
  }
  room_surface=room_profile->intValue(section,"Surface",0);
  room_program_buss=room_profile->intValue(section,"ProgramBuss",0);
  room_greenroom_buss=room_profile->intValue(section,"GreenroomBuss",0);

  device=room_profile->
    hexValue(section,QString().sprintf("Device%d",count+1),0,&ok);
  while(ok) {
    room_device_numbers.push_back(device);
    room_mix_minuses.
      push_back(room_profile->intValue(section,QString().sprintf("MixMinus%d",
								 count+1)));
    room_buss0_states.push_back(false);
    count++;
    device=room_profile->
      hexValue(section,QString().sprintf("Device%d",count+1),0,&ok);
  }

  return true;
}
