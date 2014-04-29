// rooms.cpp
//
// Abstract LPCodecPool Rooms
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

#include "rooms.h"

Rooms::Rooms(LPProfile *p)
{
  unsigned count=1;
  QString section=QString().sprintf("Room%u",count);
  bool ok;
  QString name=p->stringValue(section,"Name","",&ok);
  std::vector<QString> strs;
  std::vector<int> ints;
  std::vector<QHostAddress> addrs;

  while(ok) {
    room_names.push_back(name);
    room_port_names.push_back(strs);
    room_switcher_outputs.push_back(ints);
    room_switcher_inputs.push_back(ints);
    room_codecs.push_back(ints);
    room_source_addresses.push_back(addrs);

    //
    // Get Ports
    //
    unsigned port=1;
    int output=p->intValue(section,QString().sprintf("Port%uSwitcherOutput",
						     port),0,&ok)-1;
    while(ok) {
      room_switcher_outputs.back().push_back(output);
      room_switcher_inputs.back().
	push_back(p->intValue(section,QString().sprintf("Port%uSwitcherInput",
							port))-1);
      room_port_names.back().
	push_back(p->stringValue(section,QString().sprintf("Port%uName",port),
				 QString().sprintf("Codec %u",port)));
      room_codecs.back().push_back(-1);
      port++;
      output=p->intValue(section,QString().sprintf("Port%uSwitcherOutput",
						   port),0,&ok)-1;
    }

    //
    // Get Source Addresses
    //
    port=1;
    QHostAddress addr=p->addressValue(section,
	 QString().sprintf("SourceAddress%u",port),QHostAddress(),&ok);
    while(ok) {
      room_source_addresses.back().push_back(addr);
      port++;
      addr=p->addressValue(section,
	 QString().sprintf("SourceAddress%u",port),QHostAddress(),&ok);
    }

    count++;
    section=QString().sprintf("Room%u",count);
    name=p->stringValue(section,"Name","",&ok);
  }
}


unsigned Rooms::roomQuantity() const
{
  return room_names.size();
}


QString Rooms::name(unsigned n) const
{
  return room_names[n];
}


unsigned Rooms::portQuantity(unsigned n) const
{
  return room_switcher_outputs[n].size();
}


QString Rooms::portName(unsigned n,unsigned port) const
{
  return room_port_names[n][port];
}


int Rooms::switcherOutput(unsigned n,unsigned port) const
{
  return room_switcher_outputs[n][port];
}


int Rooms::switcherInput(unsigned n,unsigned port) const
{
  return room_switcher_inputs[n][port];
}


int Rooms::roomBySwitcherInput(int input) const
{
  for(unsigned i=0;i<room_switcher_inputs.size();i++) {
    for(unsigned j=0;j<room_switcher_inputs[i].size();j++) {
      if(room_switcher_inputs[i][j]==input) {
	return i;
      }
    }
  }

  return -1;
}


int Rooms::portBySwitcherInput(int input) const
{
  for(unsigned i=0;i<room_switcher_inputs.size();i++) {
    for(unsigned j=0;j<room_switcher_inputs[i].size();j++) {
      if(room_switcher_inputs[i][j]==input) {
	return j;
      }
    }
  }

  return -1;
}


int Rooms::roomBySwitcherOutput(int output) const
{
  for(unsigned i=0;i<room_switcher_outputs.size();i++) {
    for(unsigned j=0;j<room_switcher_outputs[i].size();j++) {
      if(room_switcher_outputs[i][j]==output) {
	return i;
      }
    }
  }

  return -1;
}


int Rooms::portBySwitcherOutput(int output) const
{
  for(unsigned i=0;i<room_switcher_outputs.size();i++) {
    for(unsigned j=0;j<room_switcher_outputs[i].size();j++) {
      if(room_switcher_outputs[i][j]==output) {
	return j;
      }
    }
  }

  return -1;
}


int Rooms::codec(unsigned n,unsigned port) const
{
  return room_codecs[n][port];
}


void Rooms::setCodec(unsigned n,unsigned port,int codec)
{
  for(unsigned i=0;i<room_codecs.size();i++) {
    for(unsigned j=0;j<room_codecs[i].size();j++) {
      if(room_codecs[i][j]==codec) {
	room_codecs[i][j]=-1;
      }
    }
  }
  room_codecs[n][port]=codec;
}


int Rooms::belongsTo(const QHostAddress &src_addr) const
{
  for(unsigned i=0;i<room_source_addresses.size();i++) {
    for(unsigned j=0;j<room_source_addresses[i].size();j++) {
      if(room_source_addresses[i][j]==src_addr) {
	return i;
      }
    }
  }
  return -1;
}
