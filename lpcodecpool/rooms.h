// rooms.h
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

#ifndef ROOMS_H
#define ROOMS_H

#include <vector>

#include <QtNetwork/QHostAddress>

#include <lpcore/lpprofile.h>

class Rooms
{
 public:
  Rooms(LPProfile *p);
  unsigned roomQuantity() const;
  QString name(unsigned n) const;
  unsigned portQuantity(unsigned n) const;
  QString portName(unsigned n,unsigned port) const;
  int switcherOutput(unsigned n,unsigned port) const;
  int switcherInput(unsigned n,unsigned port) const;
  int roomBySwitcherInput(int input) const;
  int portBySwitcherInput(int input) const;
  int roomBySwitcherOutput(int output) const;
  int portBySwitcherOutput(int output) const;
  int codec(unsigned n,unsigned port) const;
  void setCodec(unsigned n,unsigned port,int codec);
  int belongsTo(const QHostAddress &src_addr) const;

 private:
  std::vector<QString> room_names;
  std::vector<std::vector<QString> > room_port_names;
  std::vector<std::vector<int> > room_switcher_outputs;
  std::vector<std::vector<int> > room_switcher_inputs;
  std::vector<std::vector<int> > room_codecs;
  std::vector<std::vector<QHostAddress> > room_source_addresses;
};


#endif  // CODECS_H
