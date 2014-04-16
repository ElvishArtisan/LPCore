// button.h
//
// Abstract a Button Translation
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: button.h,v 1.1 2013/11/21 22:31:48 cvs Exp $
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

#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#include <vector>

#include <lpcore/lpprofile.h>

class Button
{
 public:
  enum Type {TypeLevel=0,TypeRising=1,TypeFalling=2};
  enum Action {ActionNormal=0,ActionInvert=1,ActionMomentary=2};
  Button(LPProfile *p);
  Button::Type triggerType() const;
  int srcEngine() const;
  int srcSurface() const;
  int srcDevice() const;
  int srcBuss() const;
  unsigned destinations() const;
  Button::Action triggerAction(unsigned n) const;
  int dstEngine(unsigned n) const;
  int dstSurface(unsigned n) const;
  int dstDevice(unsigned n) const;
  int dstBuss(unsigned n) const;
  bool belongsTo(int src_engine,int src_surface,
		 int src_device,int src_buss) const;
  void clear();
  bool load(unsigned n);

 private:
  Button::Type button_type;
  int button_src_engine;
  int button_src_surface;
  int button_src_device;
  int button_src_buss;
  std::vector<Button::Action> button_actions;
  std::vector<int> button_dst_engines;
  std::vector<int> button_dst_surfaces;
  std::vector<int> button_dst_devices;
  std::vector<int> button_dst_busses;
  LPProfile *button_profile;
};


#endif  // BUTTON_H
