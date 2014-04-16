// button.cpp
//
// Abstract a Button Translation
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: button.cpp,v 1.1 2013/11/21 22:31:48 cvs Exp $
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

#include "button.h"

Button::Button(LPProfile *p)
{
  button_profile=p;
}


Button::Type Button::triggerType() const
{
  return button_type;
}


int Button::srcEngine() const
{
  return button_src_engine;
}


int Button::srcSurface() const
{
  return button_src_surface;
}


int Button::srcDevice() const
{
  return button_src_device;
}


int Button::srcBuss() const
{
  return button_src_buss;
}


unsigned Button::destinations() const
{
  return button_actions.size();
}


Button::Action Button::triggerAction(unsigned n) const
{
  return button_actions[n];
}


int Button::dstEngine(unsigned n) const
{
  return button_dst_engines[n];
}


int Button::dstSurface(unsigned n) const
{
  return button_dst_surfaces[n];
}


int Button::dstDevice(unsigned n) const
{
  return button_dst_devices[n];
}


int Button::dstBuss(unsigned n) const
{
  return button_dst_busses[n];
}


bool Button::belongsTo(int src_engine,int src_surface,
		       int src_device,int src_buss) const
{
  return (src_engine==button_src_engine)&&(src_surface==button_src_surface)&&
    (src_device==button_src_device)&&(src_buss==button_src_buss);
}


void Button::clear()
{
  button_type=Button::TypeLevel;
  button_src_engine=0;
  button_src_surface=0;
  button_src_device=0;
  button_src_buss=0;
  button_actions.clear();
  button_dst_engines.clear();
  button_dst_surfaces.clear();
  button_dst_devices.clear();
  button_dst_busses.clear();
}


bool Button::load(unsigned n)
{
  bool ok=false;
  QString section=QString().sprintf("Button%u",n+1);
  QString str;
  int count=0;
  int engine=0;

  button_src_engine=button_profile->intValue(section,"SourceEngine",0,&ok);
  if(!ok) {
    return false;
  }
  button_src_surface=button_profile->intValue(section,"SourceSurface",0);
  button_src_device=button_profile->hexValue(section,"SourceDevice",0);
  button_src_buss=button_profile->intValue(section,"SourceBuss",0);

  str=button_profile->stringValue(section,"TriggerType","Level").toLower();
  if(str=="rising") {
    button_type=Button::TypeRising;
  }
  else {
    if(str=="falling") {
      button_type=Button::TypeFalling;
    }
    else {
      button_type=Button::TypeLevel;
    }
  }

  engine=button_profile->
    intValue(section,QString().sprintf("DestinationEngine%d",count+1),0,&ok);
  while(ok) {
    button_dst_engines.push_back(engine);
    button_dst_surfaces.push_back(button_profile->
       intValue(section,QString().sprintf("DestinationSurface%d",count+1),0));
    button_dst_devices.push_back(button_profile->
       hexValue(section,QString().sprintf("DestinationDevice%d",count+1),0));
    button_dst_busses.push_back(button_profile->
       intValue(section,QString().sprintf("DestinationBuss%d",count+1),0));

    str=button_profile->stringValue(section,QString().
       sprintf("TriggerAction%d",count+1),"Normal").toLower();
    if(str=="invert") {
      button_actions.push_back(Button::ActionInvert);
    }
    else {
      if(str=="momentary") {
	button_actions.push_back(Button::ActionMomentary);
      }
      else {
	button_actions.push_back(Button::ActionNormal);
      }
    }
    count++;
    engine=button_profile->
      intValue(section,QString().sprintf("DestinationEngine%d",count+1),0,&ok);
  }

  return true;
}
