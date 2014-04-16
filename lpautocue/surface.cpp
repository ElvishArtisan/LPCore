// surface.cpp
//
// Abstract a Mixing Surface
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: surface.cpp,v 1.1 2013/11/21 22:31:45 cvs Exp $
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

#include "surface.h"

Surface::Surface(LPProfile *p)
{
  surface_profile=p;
  clear();
}


int Surface::engine() const
{
  return surface_engine;
}


int Surface::surface() const
{
  return surface_surface;
}


int Surface::srcBuss() const
{
  return surface_src_buss;
}


int Surface::dstBuss() const
{
  return surface_dst_buss;
}


bool Surface::autoOn() const
{
  return surface_auto_on;
}


bool Surface::autoOff() const
{
  return surface_auto_off;
}


bool Surface::belongsTo(int engine,int surface,int src_buss) const
{
  return (engine==surface_engine)&&(surface==surface_surface)&&
    (src_buss==surface_src_buss);
}


void Surface::clear()
{
  surface_engine=0;
  surface_surface=0;
  surface_src_buss=0;
  surface_dst_buss=2;
  surface_auto_on=true;
  surface_auto_off=true;
}


bool Surface::load(unsigned n)
{
  bool ok=false;
  QString section=QString().sprintf("Surface%u",n+1);

  surface_engine=surface_profile->intValue(section,"Engine",0,&ok);
  if(!ok) {
    return false;
  }
  surface_surface=surface_profile->intValue(section,"Surface");
  surface_src_buss=surface_profile->intValue(section,"SourceBuss");
  surface_dst_buss=surface_profile->intValue(section,"DestinationBuss");
  surface_auto_on=surface_profile->boolValue(section,"AutoOn",true);
  surface_auto_off=surface_profile->boolValue(section,"AutoOff",true);

  return true;
}
