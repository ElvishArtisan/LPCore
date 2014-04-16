// source.cpp
//
// Abstract an LPNetRouter Source
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: source.cpp,v 1.3 2014/03/17 21:56:58 cvs Exp $
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

#include <syslog.h>

#include <QtCore/QObject>

#include "source.h"

Source::Source(LPProfile *p,QObject *parent)
  : QObject(parent)
{
  src_profile=p;
  clear();

  src_relay_lamp_mapper=new QSignalMapper(this);
  connect(src_relay_lamp_mapper,SIGNAL(mapped(int)),
	  this,SLOT(lampTimeoutData(int)));
}


int Source::id() const
{
  return src_id;
}


unsigned Source::sourceAddresses() const
{
  return src_source_addresses.size();
}


QHostAddress Source::sourceAddress(unsigned n) const
{
  return src_source_addresses[n];
}


QString Source::sourceOnRml(unsigned n) const
{
  return src_source_on_rmls[n];
}


QString Source::sourceOffRml(unsigned n) const
{
  return src_source_off_rmls[n];
}


int Source::sourceEngine(unsigned n) const
{
  return src_source_engines[n];
}


int Source::sourceSurface(unsigned n) const
{
  return src_source_surfaces[n];
}


int Source::sourceDevice(unsigned n) const
{
  return src_source_devices[n];
}


int Source::sourceBuss(unsigned n) const
{
  return src_source_busses[n];
}


QString Source::name() const
{
  return src_name;
}


unsigned Source::relays() const
{
  return src_relay_engines.size();
}


int Source::relayEngine(unsigned relay) const
{
  return src_relay_engines[relay];
}


int Source::relaySurface(unsigned relay) const
{
  return src_relay_surfaces[relay];
}


int Source::relayDevice(unsigned relay) const
{
  return src_relay_devices[relay];
}


int Source::relayBuss(unsigned relay) const
{
  return src_relay_busses[relay];
}


int Source::directRelayEngine(unsigned relay) const
{
  return src_direct_relay_engines[relay];
}


int Source::directRelaySurface(unsigned relay) const
{
  return src_direct_relay_surfaces[relay];
}


int Source::directRelayDevice(unsigned relay) const
{
  return src_direct_relay_devices[relay];
}


int Source::directRelayBuss(unsigned relay) const
{
  return src_direct_relay_busses[relay];
}


int Source::relayButtonEngine(unsigned relay) const
{
  return src_relay_button_engines[relay];
}


int Source::relayButtonSurface(unsigned relay) const
{
  return src_relay_button_surfaces[relay];
}


int Source::relayButtonDevice(unsigned relay) const
{
  return src_relay_button_devices[relay];
}


int Source::relayButtonBuss(unsigned relay) const
{
  return src_relay_button_busses[relay];
}


int Source::relayLampEngine(unsigned relay) const
{
  return src_relay_lamp_engines[relay];
}


int Source::relayLampSurface(unsigned relay) const
{
  return src_relay_lamp_surfaces[relay];
}


int Source::relayLampDevice(unsigned relay) const
{
  return src_relay_lamp_devices[relay];
}


int Source::relayLampBuss(unsigned relay) const
{
  return src_relay_lamp_busses[relay];
}


bool Source::belongsToSource(const QHostAddress &addr) const
{
  for(unsigned i=0;i<src_source_addresses.size();i++) {
    if(src_source_addresses[i]==addr) {
      return true;
    }
  }
  return false;
}


int Source::belongsToSourceSource(int engine,int surface,int device,
				  int buss) const
{
  for(unsigned i=0;i<src_source_engines.size();i++) {
    if((engine==src_source_engines[i])&&
       (surface==src_source_surfaces[i])&&
       (device==src_source_devices[i])&&
       (buss==src_source_busses[i])) {
      return i;
    }
  }
  return -1;
}


int Source::belongsToRelay(int engine,int surface,int device,int buss) const
{
  for(unsigned i=0;i<src_relay_engines.size();i++) {
    if((engine==src_relay_engines[i])&&
       (surface==src_relay_surfaces[i])&&
       (device==src_relay_devices[i])&&
       (buss==src_relay_busses[i])) {
      return i;
    }
  }
  return -1;
}


int Source::belongsToRelayButton(int engine,int surface,int device,
				 int buss) const
{
  for(unsigned i=0;i<src_relay_button_engines.size();i++) {
    if((engine==src_relay_button_engines[i])&&
       (surface==src_relay_button_surfaces[i])&&
       (device==src_relay_button_devices[i])&&
       (buss==src_relay_button_busses[i])) {
      return i;
    }
  }
  return -1;
}


bool Source::belongsToUdp(unsigned udp,const QHostAddress &addr)
{
  if(udp<src_udp_addresses.size()) {
    for(unsigned i=0;i<src_udp_addresses[udp].size();i++) {
      if(addr==src_udp_addresses[udp][i]) {
	return true;
      }
    }
  }
  return false;
}


void Source::lightRelayLamp(int relay)
{
  src_relay_lamp_timers[relay]->start(2000);
}


void Source::clear()
{
  for(unsigned i=0;i<src_relay_lamp_timers.size();i++) {
    delete src_relay_lamp_timers[i];
  }
  src_relay_lamp_timers.clear();
  src_name=QObject::tr("Source");
  src_relay_engines.clear();
  src_relay_surfaces.clear();
  src_relay_devices.clear();
  src_relay_busses.clear();
  src_direct_relay_engines.clear();
  src_direct_relay_surfaces.clear();
  src_direct_relay_devices.clear();
  src_direct_relay_busses.clear();
  src_relay_button_engines.clear();
  src_relay_button_surfaces.clear();
  src_relay_button_devices.clear();
  src_relay_button_busses.clear();
  src_relay_lamp_engines.clear();
  src_relay_lamp_surfaces.clear();
  src_relay_lamp_devices.clear();
  src_relay_lamp_busses.clear();
}


bool Source::load(int id)
{
  src_id=id;
  int engine=0;
  bool ok=false;
  QString section=QString().sprintf("Source%u",id+1);
  int count=0;
  QHostAddress addr;
  unsigned udp=0;
  std::vector<QHostAddress> addrs;

  addr=src_profile->
    addressValue(section,QString().sprintf("SourceAddress%d",count+1),
		 QHostAddress(),&ok);
  while(ok) {
    src_source_addresses.push_back(addr);
    src_source_engines.
      push_back(src_profile->intValue(section,
		    QString().sprintf("OnAirSourceEngine%d",count+1)));
    src_source_surfaces.
      push_back(src_profile->intValue(section,
		    QString().sprintf("OnAirSourceSurface%d",count+1)));
    src_source_devices.
      push_back(src_profile->hexValue(section,
		    QString().sprintf("OnAirSourceDevice%d",count+1)));
    src_source_busses.
      push_back(src_profile->intValue(section,
		    QString().sprintf("OnAirSourceBuss%d",count+1)));
    src_source_on_rmls.
      push_back(src_profile->stringValue(section,
		    QString().sprintf("OnAirOnRml%d",count+1)));
    src_source_off_rmls.
      push_back(src_profile->stringValue(section,
		    QString().sprintf("OnAirOffRml%d",count+1)));
    count++;
    addr=src_profile->
      addressValue(section,QString().sprintf("SourceAddress%d",count+1),
		   QHostAddress(),&ok);
  }

  count=0;
  engine=src_profile->
    intValue(section,QString().sprintf("Relay%dEngine",count+1),0,&ok);
  if(!ok) {
    return false;
  }
  while(ok) {
    src_relay_engines.push_back(engine);
    src_relay_surfaces.push_back(src_profile->intValue(section,
				 QString().sprintf("Relay%dSurface",count+1)));
    src_relay_devices.push_back(src_profile->hexValue(section,
				 QString().sprintf("Relay%dDevice",count+1)));
    src_relay_busses.push_back(src_profile->intValue(section,
				 QString().sprintf("Relay%dBuss",count+1)));
    src_direct_relay_engines.push_back(src_profile->intValue(section,
			QString().sprintf("Relay%dDirectEngine",count+1)));
    src_direct_relay_surfaces.push_back(src_profile->intValue(section,
			QString().sprintf("Relay%dDirectSurface",count+1)));
    src_direct_relay_devices.push_back(src_profile->hexValue(section,
			QString().sprintf("Relay%dDirectDevice",count+1)));
    src_direct_relay_busses.push_back(src_profile->intValue(section,
			QString().sprintf("Relay%dDirectBuss",count+1)));
    src_relay_button_engines.push_back(src_profile->intValue(section,
			   QString().sprintf("Relay%dButtonEngine",count+1)));
    src_relay_button_surfaces.push_back(src_profile->intValue(section,
			   QString().sprintf("Relay%dButtonSurface",count+1)));
    src_relay_button_devices.push_back(src_profile->hexValue(section,
			   QString().sprintf("Relay%dButtonDevice",count+1)));
    src_relay_button_busses.push_back(src_profile->intValue(section,
			   QString().sprintf("Relay%dButtonBuss",count+1)));
    src_relay_lamp_engines.push_back(src_profile->intValue(section,
			   QString().sprintf("Relay%dLampEngine",count+1)));
    src_relay_lamp_surfaces.push_back(src_profile->intValue(section,
			   QString().sprintf("Relay%dLampSurface",count+1)));
    src_relay_lamp_devices.push_back(src_profile->hexValue(section,
			   QString().sprintf("Relay%dLampDevice",count+1)));
    src_relay_lamp_busses.push_back(src_profile->intValue(section,
			   QString().sprintf("Relay%dLampBuss",count+1)));
    src_relay_lamp_timers.push_back(new QTimer(this));
    src_relay_lamp_timers.back()->setSingleShot(true);
    connect(src_relay_lamp_timers.back(),SIGNAL(timeout()),
	    src_relay_lamp_mapper,SLOT(map()));
    src_relay_lamp_mapper->setMapping(src_relay_lamp_timers.back(),count);
    udp=0;
    while(src_profile->intValue(QString().sprintf("Udp%d",udp+1),"Port",0)>0) {
      src_udp_addresses.push_back(addrs);
      udp++;
    }
    udp=0;
    addr=src_profile->addressValue(section,QString().sprintf("Udp%dAddress%u",
				   count+1,udp+1),QHostAddress(),&ok);
    while(ok) {
      if(udp<src_udp_addresses.size()) {
	src_udp_addresses[udp].push_back(addr);
      }
      else {
	syslog(LOG_WARNING,
	       "UDP address \"%s\" in Source %d references undefined UDP queue",
	       (const char *)addr.toString().toAscii(),count+1);
      }
      addr=src_profile->
	addressValue(section,
		     QString().sprintf("Udp%dAddress%u",
				       count+1,udp+1),QHostAddress(),&ok);
      udp++;
    }
    count++;
    engine=src_profile->
      intValue(section,QString().sprintf("Relay%dEngine",count+1),0,&ok);
  }
  src_name=src_profile->stringValue(section,"Name",QObject::tr("Source")+
				    QString().sprintf(" %d",id+1));

  return true;
}


void Source::lampTimeoutData(int relay)
{
  emit resetRelayLamp(src_relay_lamp_engines[relay],
		      src_relay_lamp_devices[relay],
		      src_relay_lamp_surfaces[relay],
		      src_relay_lamp_busses[relay]);
}
