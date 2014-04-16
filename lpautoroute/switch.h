// switch.h
//
// Abstract a Routing Switch
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switch.h,v 1.2 2013/10/08 23:17:32 cvs Exp $
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

#ifndef SWITCH_H
#define SWITCH_H

#include <stdint.h>

#include <vector>

#include <lpcore/lpprofile.h>

class Switch
{
 public:
  Switch(LPProfile *p);
  unsigned routes() const;
  int surface(bool state,unsigned n) const;
  int srcEngine(bool state,unsigned n) const;
  int srcDevice(bool state,unsigned n) const;
  int dstEngine(bool state,unsigned n) const;
  int dstDevice(bool state,unsigned n) const;
  bool belongsTo(int engine,int surface,int device,int buss) const;
  void clear();
  bool load(unsigned n);

 private:
  int switch_engine;
  int switch_surface;
  int switch_device;
  int switch_buss;
  std::vector<int> switch_surfaces[2];
  std::vector<int> switch_src_engines[2];
  std::vector<int> switch_src_devices[2];
  std::vector<int> switch_dst_engines[2];
  std::vector<int> switch_dst_devices[2];
  LPProfile *switch_profile;
};


#endif  // SWITCH_H
