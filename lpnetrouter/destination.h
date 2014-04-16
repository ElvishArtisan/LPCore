// destination.h
//
// Abstract an LPNetRouter Destination
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: destination.h,v 1.2 2014/02/17 12:36:55 cvs Exp $
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

#ifndef DESTINATION_H
#define DESTINATION_H

#include <vector>

#include <QtNetwork/QHostAddress>

#include <lpcore/lpprofile.h>

class Destination
{
 public:
  Destination(LPProfile *p);
  int id() const;
  QString name() const;
  int source() const;
  void setSource(int src);
  int armed() const;
  void setArmed(int src);
  QString sourceChangedRml() const;
  QString relayActivatedRml() const;
  QString silenceSenseRml(bool state) const;
  unsigned relays() const;
  int relayOutput(unsigned relay) const;
  unsigned udpDestinations(unsigned udp) const;
  QHostAddress udpAddress(unsigned udp,unsigned n) const;
  int udpPort(unsigned udp,unsigned n) const;
  QString udpRewriteRule(unsigned udp,unsigned n) const;
  void clear();
  bool load(int id);

 private:
  QString dst_source_changed_rml;
  QString dst_relay_activated_rml;
  QString dst_silence_sense_rml[2];
  std::vector<int> dst_relay_outputs;
  std::vector<std::vector<QHostAddress> > dst_udp_addresses;
  std::vector<std::vector<int> > dst_udp_ports;
  std::vector<std::vector<QString> > dst_udp_rewrite_rules;
  int dst_id;
  QString dst_name;
  int dst_source;
  int dst_armed;
  LPProfile *dst_profile;
};


#endif  // DESTINATION_H
