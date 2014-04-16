// lpengine.h
//
// Abstract an Audio Engine state.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpengine.h,v 1.1 2013/08/09 23:19:49 cvs Exp $
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

#ifndef LPENGINE_H
#define LPENGINE_H

#include <map>

#include <lpcore/lpconfig.h>
#include <lpcore/lpmessage.h>

class LPEngine
{
 public:
  LPEngine(unsigned engine);
  ~LPEngine();
  int engine() const;
  int sourceDevice(int chan);
  void setSourceDevice(int chan,int device);
  bool bussState(int device,int surface,int buss);
  void setBussState(int device,int surface,int buss,bool state);
  int faderLevel(int device,int surface);
  void setFaderLevel(int device,int surface,int lvl);
  LPMessage::ChannelMode mode(int device,int surface);
  void setMode(int device,int surface,LPMessage::ChannelMode mode);

 private:
  int GetChannel(int src_device,int surface);
  int eng_engine;
  std::map<int,int> eng_source_devices;
  std::map<int,bool> eng_buss_states[LPCORE_MAX_BUSSES];
  std::map<int,int> eng_fader_levels;
  std::map<int,LPMessage::ChannelMode> eng_modes;
};


#endif  // LPENGINE_H
