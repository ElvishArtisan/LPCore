// source.h
//
// Abstract an LPNetRouter Source
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: source.h,v 1.8 2013/08/12 21:40:58 cvs Exp $
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

#ifndef SOURCE_H
#define SOURCE_H

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>

#include <lpcore/lpprofile.h>

class Source :public QObject
{
  Q_OBJECT
 public:
  Source(LPProfile *p,QObject *parent=0);
  int id() const;
  unsigned sourceAddresses() const;
  QHostAddress sourceAddress(unsigned n) const;
  QString sourceOnRml(unsigned n) const;
  QString sourceOffRml(unsigned n) const;
  int sourceEngine(unsigned n) const;
  int sourceSurface(unsigned n) const;
  int sourceDevice(unsigned n) const;
  int sourceBuss(unsigned n) const;
  QString name() const;
  unsigned relays() const;
  int relayEngine(unsigned relay) const;
  int relaySurface(unsigned relay) const;
  int relayDevice(unsigned relay) const;
  int relayBuss(unsigned relay) const;
  int directRelayEngine(unsigned relay) const;
  int directRelaySurface(unsigned relay) const;
  int directRelayDevice(unsigned relay) const;
  int directRelayBuss(unsigned relay) const;
  int relayButtonEngine(unsigned relay) const;
  int relayButtonSurface(unsigned relay) const;
  int relayButtonDevice(unsigned relay) const;
  int relayButtonBuss(unsigned relay) const;
  int relayLampEngine(unsigned relay) const;
  int relayLampSurface(unsigned relay) const;
  int relayLampDevice(unsigned relay) const;
  int relayLampBuss(unsigned relay) const;
  bool belongsToSource(const QHostAddress &addr) const;
  int belongsToSourceSource(int engine,int surface,int device,int buss) const;
  int belongsToRelay(int engine,int surface,int device,int buss) const;
  int belongsToRelayButton(int engine,int surface,int device,int buss) const;
  bool belongsToUdp(unsigned udp,const QHostAddress &addr);
  void lightRelayLamp(int relay);
  void clear();
  bool load(int id);

 private slots:
  void lampTimeoutData(int relay);

 signals:
  void resetRelayLamp(int engine,int device,int surface,int buss);

 private:
  std::vector<QHostAddress> src_source_addresses;
  std::vector<int> src_source_engines;
  std::vector<int> src_source_surfaces;
  std::vector<int> src_source_devices;
  std::vector<int> src_source_busses;
  std::vector<QString> src_source_on_rmls;
  std::vector<QString> src_source_off_rmls;
  std::vector<int> src_relay_engines;
  std::vector<int> src_relay_surfaces;
  std::vector<int> src_relay_devices;
  std::vector<int> src_relay_busses;
  std::vector<int> src_direct_relay_engines;
  std::vector<int> src_direct_relay_surfaces;
  std::vector<int> src_direct_relay_devices;
  std::vector<int> src_direct_relay_busses;
  std::vector<int> src_relay_button_engines;
  std::vector<int> src_relay_button_surfaces;
  std::vector<int> src_relay_button_devices;
  std::vector<int> src_relay_button_busses;
  std::vector<int> src_relay_lamp_engines;
  std::vector<int> src_relay_lamp_surfaces;
  std::vector<int> src_relay_lamp_devices;
  std::vector<int> src_relay_lamp_busses;
  std::vector<QTimer *> src_relay_lamp_timers;
  QSignalMapper *src_relay_lamp_mapper;
  std::vector<std::vector<QHostAddress> > src_udp_addresses;
  int src_id;
  QString src_name;
  LPProfile *src_profile;
};


#endif  // SOURCE_H
