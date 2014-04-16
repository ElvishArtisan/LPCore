// room.h
//
// Abstract a Green Room
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: room.h,v 1.1 2013/11/21 22:31:46 cvs Exp $
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

#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>

#include <vector>
#include <map>

#include <lpcore/lpprofile.h>

class Room
{
 public:
  Room(LPProfile *p);
  int engine() const;
  int surface() const;
  int programBuss() const;
  int greenroomBuss() const;
  unsigned devices() const;
  int deviceNumber(unsigned n) const;
  int deviceNumberByChannel(uint8_t chan);
  int mixMinus(unsigned n) const;
  int mixMinusByDevice(int device) const;
  void setDeviceNumberByChannel(uint8_t chan,int device);
  bool buss0StateByDevice(int device);
  void setBuss0StateByDevice(int device,bool state);
  bool belongsTo(int engine,int surface) const;
  bool belongsTo(int engine,int surface,int device) const;
  void clear();
  bool load(unsigned n);

 private:
  int room_engine;
  int room_surface;
  int room_program_buss;
  int room_greenroom_buss;
  std::vector<int> room_device_numbers;
  std::vector<int> room_mix_minuses;
  std::vector<int> room_buss0_states;
  std::map<uint8_t,int> room_channel_sources;
  LPProfile *room_profile;
};


#endif  // ROOM_H
