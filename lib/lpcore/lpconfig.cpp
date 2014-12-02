// lpconfig.cpp
//
// A container class for an LPCore configuration.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpconfig.cpp,v 1.3 2013/12/31 19:31:05 cvs Exp $
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

#include "lpconfig.h"

//
// Device -> Surface Mappings
//
int lp_surface_map[256]=
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
   0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
   0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
   0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
   0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00};

LPConfig::LPConfig()
{
  clear();
}


unsigned LPConfig::engines() const
{
  return lp_engine_numbers.size();
}


int LPConfig::engineNumber(unsigned n) const
{
  return lp_engine_numbers[n];
}


QString LPConfig::engineType(unsigned n) const
{
  return lp_engine_types[n];
}


QString LPConfig::engineSerialDevice(unsigned n) const
{
  return lp_engine_serial_devices[n];
}


int LPConfig::engineSwitcherId(unsigned n) const
{
  return lp_engine_switcher_ids[n];
}


unsigned LPConfig::applications() const
{
  return lp_application_commands.size();
}


QString LPConfig::applicationCommand(unsigned n) const
{
  return lp_application_commands[n];
}


QStringList LPConfig::applicationArguments(unsigned n) const
{
  return lp_application_arguments[n];
}


bool LPConfig::applicationRespawn(unsigned n) const
{
  return lp_application_respawns[n];
}


QByteArray LPConfig::startupCodes(unsigned engine)
{
  return lp_startup_codes[engine];
}


bool LPConfig::load()
{
  unsigned count=0;
  int engine;
  QString command;
  QStringList args;
  bool ok=false;

  LPProfile *p=new LPProfile();
  if(!p->setSource(LPCORE_CONF_FILE)) {
    delete p;
    return false;
  }

  engine=p->intValue("Engines",QString().sprintf("Number%u",count+1),0,&ok);
  while(ok) {
    lp_engine_numbers.push_back(engine);
    lp_engine_serial_devices.
      push_back(p->stringValue("Engines",QString().sprintf("SerialDevice%u",
							   count+1),"",&ok));
    lp_engine_types.
      push_back(p->stringValue("Engines",QString().sprintf("Type%u",
					 count+1),"LogitekAudioEngine",&ok));
    lp_engine_switcher_ids.
      push_back(p->intValue("Engines",QString().sprintf("SwitcherId%u",
					 count+1)));
    LoadStartupCodes(p,engine);
    count++;
    engine=p->intValue("Engines",QString().sprintf("Number%u",count+1),0,&ok);
  }

  count=0;
  command=p->
    stringValue("Applications",QString().sprintf("Command%u",count+1),0,&ok);
  while(ok) {
    lp_application_commands.push_back(command);
    args=p->stringValue("Applications",QString().sprintf("Arguments%u",
					     count+1),"",&ok).split(" ");
    if((args.size()==1)&&args[0].isEmpty()) {
      args=QStringList();
    }
    lp_application_arguments.push_back(args);
    lp_application_respawns.
      push_back(p->boolValue("Applications",QString().sprintf("Respawn%u",
						count+1),false,&ok));

    count++;
    command=p->
      stringValue("Applications",QString().sprintf("Command%u",count+1),0,&ok);
  }
  delete p;
  return true;
}


void LPConfig::clear()
{
  lp_engine_numbers.clear();
  lp_engine_types.clear();
  lp_engine_serial_devices.clear();
  lp_application_commands.clear();
  lp_application_arguments.clear();
  lp_application_respawns.clear();
}


int LPConfig::surfaceByChannel(uint8_t chan)
{
  return lp_surface_map[chan];
}


void LPConfig::LoadStartupCodes(LPProfile *p,int engine)
{
  QString section=QString().sprintf("Startup%d",engine);
  unsigned line=0;
  bool ok=false;
  bool ok2=false;
  QString code;

  code=p->stringValue(section,QString().sprintf("Code%u",line+1),"",&ok);
  while(ok) {
    QStringList f0=code.split(" ");
    for(int i=0;i<f0.size();i++) {
      uint8_t byte=f0[i].toUInt(&ok2,16);
      if((!ok2)||(f0[i].length()!=2)) {
	syslog(LOG_ERR,"malformatted code line \"%s\" in [%s] section",
	       (const char *)code.toAscii(),
	       (const char *)section.toAscii());
	exit(256);
      }
      lp_startup_codes[engine].append(byte);
    }
    line++;
    code=p->stringValue(section,QString().sprintf("Code%u",line+1),"",&ok);
  }
}
