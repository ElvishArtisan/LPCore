// surface.h
//
// Abstract a Mixing Surface
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: surface.h,v 1.1 2013/11/21 22:31:45 cvs Exp $
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

#ifndef SURFACE_H
#define SURFACE_H

#include <stdint.h>

#include <vector>

#include <lpcore/lpprofile.h>

class Surface
{
 public:
  Surface(LPProfile *p);
  int engine() const;
  int surface() const;
  int srcBuss() const;
  int dstBuss() const;
  bool autoOn() const;
  bool autoOff() const;
  bool belongsTo(int engine,int surface,int src_buss) const;
  void clear();
  bool load(unsigned n);

 private:
  int surface_engine;
  int surface_surface;
  int surface_src_buss;
  int surface_dst_buss;
  bool surface_auto_on;
  bool surface_auto_off;
  LPProfile *surface_profile;
};


#endif  // SURFACE_H
