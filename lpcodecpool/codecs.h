// codecs.h
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

#ifndef CODECS_H
#define CODECS_H

#include <vector>

#include <lpcore/lpprofile.h>

class Codecs
{
 public:
  Codecs(LPProfile *p);
  unsigned codecQuantity() const;
  QString name(unsigned n) const;
  int switcherOutput(unsigned n) const;
  int switcherInput(unsigned n) const;
  int gpioLine(unsigned n) const;
  int codecBySwitcherInput(int input) const;
  int codecBySwitcherOutput(int output) const;
  int codecByGpio(int line) const;
  bool isBusy(unsigned n) const;
  void setBusy(unsigned n,bool state);
  int connectedToRoom(unsigned n) const;
  void setConnectedToRoom(unsigned n,int room_num);

 private:
  std::vector<QString> codec_names;
  std::vector<int> codec_switcher_outputs;
  std::vector<int> codec_switcher_inputs;
  std::vector<int> codec_gpio_lines;
  std::vector<bool> codec_busys;
  std::vector<int> codec_connected_to_rooms;
};


#endif  // CODECS_H
