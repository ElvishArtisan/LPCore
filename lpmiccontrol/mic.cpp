// mic.cpp
//
// Abstract a Mic Turret Configuration
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: mic.cpp,v 1.1 2013/11/21 22:31:46 cvs Exp $
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

#include "mic.h"

Mic::Mic(LPProfile *p)
{
  mic_profile=p;
}


int Mic::gateEngine() const
{
  return mic_gate_engine;
}


int Mic::gateSurface() const
{
  return mic_gate_surface;
}


int Mic::gateDevice() const
{
  return mic_gate_device;
}


int Mic::gateBuss() const
{
  return mic_gate_buss;
}


bool Mic::gateState() const
{
  return mic_gate_state;
}


int Mic::engine() const
{
  return mic_engine;
}


int Mic::surface() const
{
  return mic_surface;
}


int Mic::device() const
{
  return mic_device;
}


int Mic::onOffBuss() const
{
  return mic_on_off_buss;
}


int Mic::greenroomBuss() const
{
  return mic_greenroom_buss;
}


int Mic::coughBuss() const
{
  return mic_cough_buss;
}


int Mic::talkbackBuss() const
{
  return mic_talkback_buss;
}


int Mic::talkbackMixerInputDevice(int input) const
{
  return mic_talkback_mixer_input_device[input];
}


int Mic::phonesPfSourceDevice() const
{
  return mic_phones_pf_source_device;
}


int Mic::greenRoomDevice() const
{
  return mic_green_room_device;
}


int Mic::talkbackMicEngine() const
{
  return mic_talkback_mic_engine;
}


int Mic::talkbackMicDevice() const
{
  return mic_talkback_mic_device;
}


int Mic::talkbackMicSurface() const
{
  return mic_talkback_mic_surface;
}


int Mic::talkbackReturnEngine() const
{
  return mic_talkback_return_engine;
}


int Mic::talkbackReturnDevice() const
{
  return mic_talkback_return_device;
}


int Mic::talkbackReturnSurface() const
{
  return mic_talkback_return_surface;
}


int Mic::talkbackMonitorEngine() const
{
  return mic_talkback_monitor_engine;
}


int Mic::talkbackMonitorDevice() const
{
  return mic_talkback_monitor_device;
}


int Mic::talkbackMonitorSurface() const
{
  return mic_talkback_monitor_surface;
}


int Mic::talkbackHeadphoneEngine() const
{
  return mic_talkback_headphone_engine;
}


int Mic::talkbackHeadphoneDevice() const
{
  return mic_talkback_headphone_device;
}


int Mic::talkbackHeadphoneSurface() const
{
  return mic_talkback_headphone_surface;
}


unsigned Mic::turrets() const
{
  return mic_turret_engines[0].size();
}


int Mic::turretEngine(Mic::Function func,unsigned n) const
{
  return mic_turret_engines[func][n];
}


int Mic::turretSurface(Mic::Function func,unsigned n) const
{
  return mic_turret_surfaces[func][n];
}


int Mic::turretDevice(Mic::Function func,unsigned n) const
{
  return mic_turret_devices[func][n];
}


int Mic::turretBuss(Mic::Function func,unsigned n) const
{
  return mic_turret_busses[func][n];
}


bool Mic::isActive(LPConnection *conn) const
{
  if(mic_gate_engine==0) {
    return true;
  }
  if(mic_gate_state) {
    return conn->
      bussState(mic_gate_engine,mic_gate_device,mic_gate_surface,mic_gate_buss);
  }
  return !conn->
    bussState(mic_gate_engine,mic_gate_device,mic_gate_surface,mic_gate_buss);
}


bool Mic::belongsTo(int engine,int surface,int device,int buss) const
{
  return (engine==mic_engine)&&(surface==mic_surface)&&
    (device==mic_device)&&(buss==mic_on_off_buss);
}


bool Mic::belongsToTalkback(int engine,int surface,int device,int buss) const
{
  return (engine==mic_engine)&&(surface==mic_surface)&&
    (device==mic_device)&&(buss==mic_talkback_buss);
}


int Mic::belongsToTurret(int engine,int surface,int device,int buss,
			 Mic::Function *func) const
{
  for(unsigned i=0;i<Mic::LastFunction;i++) {
    for(unsigned j=0;j<mic_turret_engines[i].size();j++) {
      if((engine==mic_turret_engines[i][j])&&
	 (surface==mic_turret_surfaces[i][j])&&
	 (device==mic_turret_devices[i][j])&&
	 (buss==mic_turret_busses[i][j])) {
	*func=(Mic::Function)i;
	return j;
      }
    }
  }
  return -1;
}


void Mic::clear()
{
  mic_gate_engine=0;
  mic_gate_surface=0;
  mic_gate_device=0;
  mic_gate_buss=0;
  mic_gate_state=true;
  mic_engine=0;
  mic_surface=0;
  mic_device=0;
  mic_greenroom_buss=0;
  mic_on_off_buss=0;
  mic_cough_buss=0;
  mic_talkback_buss=0;
  mic_talkback_mixer_input_device[0]=0;
  mic_talkback_mixer_input_device[1]=0;
  mic_phones_pf_source_device=0;
  mic_green_room_device=0;
  mic_talkback_mic_engine=0;
  mic_talkback_mic_surface=0;
  mic_talkback_mic_device=0;
  mic_talkback_return_engine=0;
  mic_talkback_return_surface=0;
  mic_talkback_return_device=0;
  mic_talkback_monitor_engine=0;
  mic_talkback_monitor_surface=0;
  mic_talkback_monitor_device=0;
  mic_talkback_headphone_engine=0;
  mic_talkback_headphone_surface=0;
  mic_talkback_headphone_device=0;
  for(int i=0;i<Mic::LastFunction;i++) {
    mic_turret_engines[i].clear();
    mic_turret_surfaces[i].clear();
    mic_turret_devices[i].clear();
    mic_turret_busses[i].clear();
  }
}


bool Mic::load(unsigned n)
{
  bool ok=false;
  QString section=QString().sprintf("Mic%u",n+1);
  QString str;
  int count=0;
  int engine=0;

  mic_engine=mic_profile->intValue(section,"Engine",0,&ok);
  if(!ok) {
    return false;
  }
  mic_surface=mic_profile->intValue(section,"Surface",0);
  mic_device=mic_profile->hexValue(section,"Device",0);
  mic_on_off_buss=mic_profile->intValue(section,"OnOffBuss",0);
  mic_greenroom_buss=mic_profile->intValue(section,"GreenroomBuss",0);
  mic_cough_buss=mic_profile->intValue(section,"CoughBuss",0);
  mic_talkback_buss=mic_profile->intValue(section,"TalkbackBuss",0);
  mic_talkback_mixer_input_device[0]=
    mic_profile->hexValue(section,"TalkbackMixerInput1Device");
  mic_talkback_mixer_input_device[1]=
    mic_profile->hexValue(section,"TalkbackMixerInput2Device");
  mic_phones_pf_source_device=
    mic_profile->hexValue(section,"PhonesPfSourceDevice");
  mic_green_room_device=
    mic_profile->hexValue(section,"GreenRoomDevice");
  mic_talkback_mic_engine=mic_profile->intValue(section,"TalkbackMicEngine");
  mic_talkback_mic_device=mic_profile->hexValue(section,"TalkbackMicDevice");
  mic_talkback_mic_surface=mic_profile->intValue(section,"TalkbackMicSurface");
  mic_talkback_return_engine=
    mic_profile->intValue(section,"TalkbackReturnEngine");
  mic_talkback_return_device=
    mic_profile->hexValue(section,"TalkbackReturnDevice");
  mic_talkback_return_surface=
    mic_profile->intValue(section,"TalkbackReturnSurface");
  mic_talkback_monitor_engine=
    mic_profile->intValue(section,"TalkbackMonitorEngine");
  mic_talkback_monitor_device=
    mic_profile->hexValue(section,"TalkbackMonitorDevice");
  mic_talkback_monitor_surface=
    mic_profile->intValue(section,"TalkbackMonitorSurface");
  mic_talkback_headphone_engine=
    mic_profile->intValue(section,"TalkbackHeadphoneEngine");
  mic_talkback_headphone_device=
    mic_profile->hexValue(section,"TalkbackHeadphoneDevice");
  mic_talkback_headphone_surface=
    mic_profile->intValue(section,"TalkbackHeadphoneSurface");
  mic_gate_engine=mic_profile->intValue(section,"GateEngine",0);
  mic_gate_surface=mic_profile->intValue(section,"GateSurface",0);
  mic_gate_device=mic_profile->hexValue(section,"GateDevice",0);
  mic_gate_buss=mic_profile->intValue(section,"GateBuss",0);
  mic_gate_state=mic_profile->boolValue(section,"GateState",true);
  engine=mic_profile->
    intValue(section,QString().sprintf("OnButtonEngine%d",count+1),0,&ok);
  while(ok) {
    mic_turret_engines[Mic::OnButton].push_back(engine);
    mic_turret_surfaces[Mic::OnButton].push_back(mic_profile->
       intValue(section,QString().sprintf("OnButtonSurface%d",count+1),0));
    mic_turret_devices[Mic::OnButton].push_back(mic_profile->
       hexValue(section,QString().sprintf("OnButtonDevice%d",count+1),0));
    mic_turret_busses[Mic::OnButton].push_back(mic_profile->
       intValue(section,QString().sprintf("OnButtonBuss%d",count+1),0));

    mic_turret_engines[Mic::OffButton].push_back(mic_profile->
       intValue(section,QString().sprintf("OffButtonEngine%d",count+1),0));
    mic_turret_surfaces[Mic::OffButton].push_back(mic_profile->
       intValue(section,QString().sprintf("OffButtonSurface%d",count+1),0));
    mic_turret_devices[Mic::OffButton].push_back(mic_profile->
       hexValue(section,QString().sprintf("OffButtonDevice%d",count+1),0));
    mic_turret_busses[Mic::OffButton].push_back(mic_profile->
       intValue(section,QString().sprintf("OffButtonBuss%d",count+1),0));

    mic_turret_engines[Mic::OnLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("OnLampEngine%d",count+1),0));
    mic_turret_surfaces[Mic::OnLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("OnLampSurface%d",count+1),0));
    mic_turret_devices[Mic::OnLamp].push_back(mic_profile->
       hexValue(section,QString().sprintf("OnLampDevice%d",count+1),0));
    mic_turret_busses[Mic::OnLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("OnLampBuss%d",count+1),0));

    mic_turret_engines[Mic::OffLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("OffLampEngine%d",count+1),0));
    mic_turret_surfaces[Mic::OffLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("OffLampSurface%d",count+1),0));
    mic_turret_devices[Mic::OffLamp].push_back(mic_profile->
       hexValue(section,QString().sprintf("OffLampDevice%d",count+1),0));
    mic_turret_busses[Mic::OffLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("OffLampBuss%d",count+1),0));

    mic_turret_engines[Mic::CoughButton].push_back(mic_profile->
       intValue(section,QString().sprintf("CoughButtonEngine%d",count+1),0));
    mic_turret_surfaces[Mic::CoughButton].push_back(mic_profile->
       intValue(section,QString().sprintf("CoughButtonSurface%d",count+1),0));
    mic_turret_devices[Mic::CoughButton].push_back(mic_profile->
       hexValue(section,QString().sprintf("CoughButtonDevice%d",count+1),0));
    mic_turret_busses[Mic::CoughButton].push_back(mic_profile->
       intValue(section,QString().sprintf("CoughButtonBuss%d",count+1),0));

    mic_turret_engines[Mic::CoughLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("CoughLampEngine%d",count+1),0));
    mic_turret_surfaces[Mic::CoughLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("CoughLampSurface%d",count+1),0));
    mic_turret_devices[Mic::CoughLamp].push_back(mic_profile->
       hexValue(section,QString().sprintf("CoughLampDevice%d",count+1),0));
    mic_turret_busses[Mic::CoughLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("CoughLampBuss%d",count+1),0));

    mic_turret_engines[Mic::TalkbackButton].push_back(mic_profile->
       intValue(section,QString().sprintf("TalkbackButtonEngine%d",count+1),0));
    mic_turret_surfaces[Mic::TalkbackButton].push_back(mic_profile->
       intValue(section,QString().sprintf("TalkbackButtonSurface%d",count+1),0));
    mic_turret_devices[Mic::TalkbackButton].push_back(mic_profile->
       hexValue(section,QString().sprintf("TalkbackButtonDevice%d",count+1),0));
    mic_turret_busses[Mic::TalkbackButton].push_back(mic_profile->
       intValue(section,QString().sprintf("TalkbackButtonBuss%d",count+1),0));

    mic_turret_engines[Mic::TalkbackLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("TalkbackLampEngine%d",count+1),0));
    mic_turret_surfaces[Mic::TalkbackLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("TalkbackLampSurface%d",count+1),0));
    mic_turret_devices[Mic::TalkbackLamp].push_back(mic_profile->
       hexValue(section,QString().sprintf("TalkbackLampDevice%d",count+1),0));
    mic_turret_busses[Mic::TalkbackLamp].push_back(mic_profile->
       intValue(section,QString().sprintf("TalkbackLampBuss%d",count+1),0));

    count++;
    engine=mic_profile->
      intValue(section,QString().sprintf("OnButtonEngine%d",count+1),0,&ok);
  }

  return true;
}
