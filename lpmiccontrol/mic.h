// mic.h
//
// Abstract a Mic Turret Configuration
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: mic.h,v 1.1 2013/11/21 22:31:46 cvs Exp $
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

#ifndef MIC_H
#define MIC_H

#include <stdint.h>

#include <vector>

#include <lpcore/lpconnection.h>
#include <lpcore/lpprofile.h>

class Mic
{
 public:
  enum Function {OnButton=0,OnLamp=1,OffButton=2,OffLamp=3,CoughButton=4,
		 CoughLamp=5,TalkbackButton=6,TalkbackLamp=7,LastFunction=8};
  Mic(LPProfile *p);
  int gateEngine() const;
  int gateSurface() const;
  int gateDevice() const;
  int gateBuss() const;
  bool gateState() const;
  int engine() const;
  int surface() const;
  int device() const;
  int onOffBuss() const;
  int greenroomBuss() const;
  int coughBuss() const;
  int talkbackBuss() const;
  int talkbackMixerInputDevice(int input) const;
  int phonesPfSourceDevice() const;
  int greenRoomDevice() const;
  int talkbackMicEngine() const;
  int talkbackMicDevice() const;
  int talkbackMicSurface() const;
  int talkbackReturnEngine() const;
  int talkbackReturnDevice() const;
  int talkbackReturnSurface() const;
  int talkbackMonitorEngine() const;
  int talkbackMonitorDevice() const;
  int talkbackMonitorSurface() const;
  int talkbackHeadphoneEngine() const;
  int talkbackHeadphoneDevice() const;
  int talkbackHeadphoneSurface() const;
  unsigned turrets() const;
  int turretEngine(Mic::Function func,unsigned n) const;
  int turretSurface(Mic::Function func,unsigned n) const;
  int turretDevice(Mic::Function func,unsigned n) const;
  int turretBuss(Mic::Function func,unsigned n) const;
  bool isActive(LPConnection *conn) const;
  bool belongsTo(int engine,int surface,int device,int buss) const;
  bool belongsToTalkback(int engine,int surface,int device,int buss) const;
  int belongsToTurret(int engine,int surface,int device,int buss,
		      Mic::Function *func) const;
  void clear();
  bool load(unsigned n);

 private:
  int mic_gate_engine;
  int mic_gate_surface;
  int mic_gate_device;
  int mic_gate_buss;
  bool mic_gate_state;
  int mic_engine;
  int mic_surface;
  int mic_device;
  int mic_on_off_buss;
  int mic_greenroom_buss;
  int mic_cough_buss;
  int mic_talkback_buss;
  int mic_talkback_mixer_input_device[2];
  int mic_phones_pf_source_device;
  int mic_green_room_device;
  int mic_talkback_mic_engine;
  int mic_talkback_mic_surface;
  int mic_talkback_mic_device;
  int mic_talkback_return_engine;
  int mic_talkback_return_surface;
  int mic_talkback_return_device;
  int mic_talkback_monitor_engine;
  int mic_talkback_monitor_surface;
  int mic_talkback_monitor_device;
  int mic_talkback_headphone_engine;
  int mic_talkback_headphone_surface;
  int mic_talkback_headphone_device;
  std::vector<int> mic_turret_engines[Mic::LastFunction];
  std::vector<int> mic_turret_surfaces[Mic::LastFunction];
  std::vector<int> mic_turret_devices[Mic::LastFunction];
  std::vector<int> mic_turret_busses[Mic::LastFunction];
  LPProfile *mic_profile;
};


#endif  // MIC_H
