// switch.cpp
//
// Abstract a Routing Switch
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switch.cpp,v 1.2 2013/10/08 23:17:32 cvs Exp $
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

#include "switch.h"

Switch::Switch(LPProfile *p)
{
  switch_profile=p;
  clear();
}


unsigned Switch::routes() const
{
  return switch_src_engines[0].size();
}


int Switch::surface(bool state,unsigned n) const
{
  return switch_surfaces[state][n];
}


int Switch::srcEngine(bool state,unsigned n) const
{
  return switch_src_engines[state][n];
}


int Switch::srcDevice(bool state,unsigned n) const
{
  return switch_src_devices[state][n];
}


int Switch::dstEngine(bool state,unsigned n) const
{
  return switch_dst_engines[state][n];
}


int Switch::dstDevice(bool state,unsigned n) const
{
  return switch_dst_devices[state][n];
}


bool Switch::belongsTo(int engine,int surface,int device,int buss) const
{
  return (engine==switch_engine)&&(surface==switch_surface)&&
    (device==switch_device)&&(buss==switch_buss);
}


void Switch::clear()
{
  switch_engine=0;
  switch_surface=0;
  switch_device=0;
  switch_buss=0;
  for(unsigned i=0;i<2;i++) {
    switch_surfaces[i].clear();
    switch_src_engines[i].clear();
    switch_src_devices[i].clear();
    switch_dst_engines[i].clear();
    switch_dst_devices[i].clear();
  }
}


bool Switch::load(unsigned n)
{
  bool ok=false;
  QString section=QString().sprintf("Switch%u",n+1);
  int count=0;
  int engine;

  switch_engine=switch_profile->intValue(section,"Engine",0,&ok);
  if(!ok) {
    return false;
  }
  switch_surface=switch_profile->intValue(section,"Surface");
  switch_device=switch_profile->hexValue(section,"Device");
  switch_buss=switch_profile->intValue(section,"Buss");
  engine=switch_profile->
    intValue(section,QString().sprintf("OnSourceEngine%d",count+1),0,&ok);
  while(ok) {
    switch_src_engines[1].push_back(engine);
    switch_src_engines[0].
      push_back(switch_profile->intValue(section,
		QString().sprintf("OffSourceEngine%d",count+1),0,&ok));

    switch_src_devices[1].
      push_back(switch_profile->hexValue(section,
		QString().sprintf("OnSourceDevice%d",count+1),0,&ok));
    switch_src_devices[0].
      push_back(switch_profile->hexValue(section,
		QString().sprintf("OffSourceDevice%d",count+1),0,&ok));

    switch_dst_engines[1].
      push_back(switch_profile->intValue(section,
		QString().sprintf("OnDestinationEngine%d",count+1),0,&ok));
    switch_dst_engines[0].
      push_back(switch_profile->intValue(section,
		QString().sprintf("OffDestinationEngine%d",count+1),0,&ok));

    switch_dst_devices[1].
      push_back(switch_profile->hexValue(section,
		QString().sprintf("OnDestinationDevice%d",count+1),0,&ok));
    switch_dst_devices[0].
      push_back(switch_profile->hexValue(section,
		QString().sprintf("OffDestinationDevice%d",count+1),0,&ok));

    switch_surfaces[1].
      push_back(switch_profile->intValue(section,
		QString().sprintf("OnSurface%d",count+1),0,&ok));
    switch_surfaces[0].
      push_back(switch_profile->intValue(section,
		QString().sprintf("OffSurface%d",count+1),0,&ok));
    count++;
    engine=switch_profile->
      intValue(section,QString().sprintf("OnSourceEngine%d",count+1),0,&ok);
  }

  return true;
}
