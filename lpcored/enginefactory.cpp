// enginefactory.cpp
//
// Create an Engine instance.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: enginefactory.cpp,v 1.2 2013/12/31 19:31:06 cvs Exp $
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

#include "ae_engine.h"
#include "switcher_engine.h"
#include "enginefactory.h"

Engine *EngineFactory(LPConfig *config,unsigned n,QObject *parent)
{
  Engine *e=NULL;
  Engine::Type type=Engine::LogitekAeType;
  QString str=config->engineType(n);

  if(str.toLower()=="logitekaudioengine") {
    type=Engine::LogitekAeType;
  }
  if(str.toLower()=="switcher") {
    type=Engine::SwitcherType;
  }

  switch(type) {
  case Engine::LogitekAeType:
    e=new AeEngine(config,n,parent);
    break;

  case Engine::SwitcherType:
    e=new SwitcherEngine(config,n,parent);
    break;
  }
  syslog(LOG_DEBUG,"engine %u is a %s",config->engineNumber(n),
	 (const char *)Engine::typeString(type).toAscii());

  return e;
}
