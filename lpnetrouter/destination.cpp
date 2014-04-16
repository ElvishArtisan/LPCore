// destination.cpp
//
// Abstract an LPNetRouter Destination
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: destination.cpp,v 1.2 2014/02/17 12:36:54 cvs Exp $
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

#include "destination.h"

Destination::Destination(LPProfile *p)
{
  dst_profile=p;
  clear();
}


int Destination::id() const
{
  return dst_id;
}


QString Destination::name() const
{
  return dst_name;
}


int Destination::source() const
{
  return dst_source;
}


void Destination::setSource(int src)
{
  dst_source=src;
}


int Destination::armed() const
{
  return dst_armed;
}


void Destination::setArmed(int src)
{
  dst_armed=src;
}


QString Destination::sourceChangedRml() const
{
  return dst_source_changed_rml;
}


QString Destination::relayActivatedRml() const
{
  return dst_relay_activated_rml;
}


QString Destination::silenceSenseRml(bool state) const
{
  return dst_silence_sense_rml[state];
}


int Destination::relayOutput(unsigned relay) const
{
  return dst_relay_outputs[relay];
}


unsigned Destination::udpDestinations(unsigned udp) const
{
  return dst_udp_addresses[udp].size();
}


QHostAddress Destination::udpAddress(unsigned udp,unsigned n) const
{
  return dst_udp_addresses[udp][n];
}


int Destination::udpPort(unsigned udp,unsigned n) const
{
  return dst_udp_ports[udp][n];
}


QString Destination::udpRewriteRule(unsigned udp,unsigned n) const
{
  return dst_udp_rewrite_rules[udp][n];
}


void Destination::clear()
{
  dst_source=0;
  dst_armed=0;
  dst_name=QObject::tr("Destination");
  dst_silence_sense_rml[false]="";
  dst_silence_sense_rml[true]="";
  dst_relay_outputs.clear();
}


bool Destination::load(int id)
{
  dst_id=id;
  int output=0;
  bool ok=false;
  QString section=QString().sprintf("Destination%u",id+1);
  int count=0;
  QHostAddress addr;
  unsigned udp=0;
  std::vector<QHostAddress> addrs;
  std::vector<int> ints;
  std::vector<QString> rewrites;

  output=dst_profile->
    intValue(section,QString().sprintf("Relay%dOutput",count+1),0,&ok)-1;
  if(!ok) {
    return false;
  }
  while(ok) {
    dst_relay_outputs.push_back(output);
    udp=0;
    while(dst_profile->intValue(QString().sprintf("Udp%d",udp+1),"Port",0)>0) {
      dst_udp_addresses.push_back(addrs);
      dst_udp_ports.push_back(ints);
      dst_udp_rewrite_rules.push_back(rewrites);
      udp++;
    }
    udp=0;
    addr=dst_profile->addressValue(section,QString().sprintf("Udp%dAddress%u",
				   count+1,udp+1),QHostAddress(),&ok);
    while(ok) {
      if(udp<dst_udp_addresses.size()) {
	dst_udp_addresses[count].push_back(addr);
	dst_udp_ports[count].push_back(dst_profile->intValue(section,
		       QString().sprintf("Udp%dPort%u",count+1,udp+1)));
	dst_udp_rewrite_rules[count].push_back(dst_profile->stringValue(section,
		       QString().sprintf("Udp%dRewriteRule%u",count+1,udp+1)));
      }
      else {
	syslog(LOG_WARNING,
	  "UDP address \"%s\" in Destination %d references undefined UDP queue",
	       (const char *)addr.toString().toAscii(),count+1);
      }
      udp++;
      addr=dst_profile->addressValue(section,
	  QString().sprintf("Udp%dAddress%u",count+1,udp+1),QHostAddress(),&ok);
    }
    count++;
    output=dst_profile->
      intValue(section,QString().sprintf("Relay%dOutput",count+1),0,&ok)-1;
  }
  dst_name=dst_profile->stringValue(section,"Name",QObject::tr("Destination")+
				    QString().sprintf(" %d",id+1));
  dst_source_changed_rml=dst_profile->stringValue(section,"SourceChangedRml");
  dst_relay_activated_rml=dst_profile->stringValue(section,"RelayActivatedRml");
  dst_silence_sense_rml[true]=
    dst_profile->stringValue(section,"SilenceSenseOnRml");
  dst_silence_sense_rml[false]=
    dst_profile->stringValue(section,"SilenceSenseOffRml");

  return true;
}
