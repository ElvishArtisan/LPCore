// codecs.cpp
//
// Abstract LPCodecPool Codecs
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

#include "codecs.h"

Codecs::Codecs(LPProfile *p)
{
  unsigned count=1;
  QString section=QString().sprintf("Codec%u",count);
  bool ok=false;

  QString name=p->stringValue(section,"Name","",&ok);
  while(ok) {
    codec_names.push_back(name);
    codec_switcher_outputs.push_back(p->intValue(section,"SwitcherOutput")-1);
    codec_switcher_inputs.push_back(p->intValue(section,"SwitcherInput")-1);
    codec_gpio_lines.push_back(p->intValue(section,"GpioLine")-1);
    codec_configuration_commands.
      push_back(p->stringValue(section,"ConfigurationCommand"));
    codec_busys.push_back(false);
    codec_connected_to_rooms.push_back(-1);
    count++;
    section=QString().sprintf("Codec%u",count);
    name=p->stringValue(section,"Name","",&ok);
  }
}


unsigned Codecs::codecQuantity() const
{
  return codec_names.size();
}


QString Codecs::name(unsigned n) const
{
  return codec_names[n];
}


int Codecs::switcherOutput(unsigned n) const
{
  return codec_switcher_outputs[n];
}


int Codecs::switcherInput(unsigned n) const
{
  return codec_switcher_inputs[n];
}


int Codecs::gpioLine(unsigned n) const
{
  return codec_gpio_lines[n];
}


QString Codecs::configurationCommand(unsigned n) const
{
  return codec_configuration_commands[n];
}


int Codecs::codecBySwitcherInput(int input) const
{
  for(unsigned i=0;i<codec_switcher_inputs.size();i++) {
    if(codec_switcher_inputs[i]==input) {
      return i;
    }
  }

  return -1;
}


int Codecs::codecBySwitcherOutput(int output) const
{
  for(unsigned i=0;i<codec_switcher_outputs.size();i++) {
    if(codec_switcher_outputs[i]==output) {
      return i;
    }
  }

  return -1;
}


int Codecs::codecByGpio(int line) const
{
  for(unsigned i=0;i<codec_gpio_lines.size();i++) {
    if(codec_gpio_lines[i]==line) {
      return i;
    }
  }

  return -1;
}


bool Codecs::isBusy(unsigned n) const
{
  return codec_busys[n];
}


void Codecs::setBusy(unsigned n,bool state)
{
  codec_busys[n]=state;
}


int Codecs::connectedToRoom(unsigned n) const
{
  return codec_connected_to_rooms[n];
}


void Codecs::setConnectedToRoom(unsigned n,int room_num)
{
  codec_connected_to_rooms[n]=room_num;
}
