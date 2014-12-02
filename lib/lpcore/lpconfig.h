// lpconfig.h
//
// A container class for an LPCore configuration.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpconfig.h,v 1.5 2013/12/31 19:31:05 cvs Exp $
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

#ifndef LPCONFIG_H
#define LPCONFIG_H

#include <stdint.h>

#include <map>
#include <vector>

#include <QtCore/QString>
#include <QtCore/QStringList>

#include <lpcore/lpprofile.h>

#define LPCORE_CONF_FILE "/etc/lp/lpcore.conf"
#define LPCORE_ENGINE_SERIAL_SPEED 38400
#define LPCORE_ENGINE_SERIAL_WORD_LENGTH 8
#define LPCORE_ENGINE_SERIAL_PARITY LPTTYDevice::Even
#define LPCORE_ENGINE_SERIAL_FLOW_CONTROL LPTTYDevice::FlowNone
#define LPCORE_ENGINE_PING_INTERVAL 10000
#define LPCORE_CONNECTION_TCP_PORT 10212
#define LPCORE_MAX_CHANNELS 0xBC
#define LPCORE_MAX_BUSSES 256
#define LPCORE_STARTUP_TIMEOUT_INTERVAL 5000
#define LPCORE_LPCONNECTION_WATCHDOG_INTERVAL 10000
#define LPCORE_APPLICATION_FAIL_LIMIT 3
#define LPCORE_VGUEST_ID 12

class LPConfig
{
 public:
  LPConfig();
  unsigned engines() const;
  int engineNumber(unsigned n) const;
  QString engineType(unsigned n) const;
  QString engineSerialDevice(unsigned n) const;
  int engineSwitcherId(unsigned n) const;
  unsigned applications() const;
  QString applicationCommand(unsigned n) const;
  QStringList applicationArguments(unsigned n) const;
  bool applicationRespawn(unsigned n) const;
  QByteArray startupCodes(unsigned engine);
  bool load();
  void clear();
  static int surfaceByChannel(uint8_t chan);

 private:
  void LoadStartupCodes(LPProfile *p,int engine);
  std::vector<int> lp_engine_numbers;
  std::vector<QString> lp_engine_types;
  std::vector<QString> lp_engine_serial_devices;
  std::vector<int> lp_engine_switcher_ids;
  std::vector<QString> lp_application_commands;
  std::vector<QStringList> lp_application_arguments;
  std::vector<bool> lp_application_respawns;
  std::map<int,QByteArray> lp_startup_codes;
};


#endif  // LPCONFIG_H
