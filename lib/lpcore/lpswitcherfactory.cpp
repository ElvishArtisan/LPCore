// lpswitcherfactory.cpp
//
// Factory function for LPSwitcher devices.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpswitcherfactory.cpp,v 1.5 2013/09/09 16:42:57 cvs Exp $
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

#include "lpswitcherfactory.h"
#include "switcher_am16.h"
#include "switcher_btss82.h"
#include "switcher_gpio.h"
#include "switcher_zephyrclassic.h"
#include "switcher_zephyrxtreme.h"

LPSwitcher *LPSwitcherFactory(int id,LPSwitcher::Type type,QObject *parent)
{
  LPSwitcher *ret=NULL;

  switch(type) {
  case LPSwitcher::TypeAm16:
    ret=new Am16(id,parent);
    break;

  case LPSwitcher::TypeBtSs82:
    ret=new BtSs82(id,parent);
    break;

  case LPSwitcher::TypeGpio:
    ret=new Gpio(id,parent);
    break;

  case LPSwitcher::TypeZephyrClassic:
    ret=new ZephyrClassic(id,parent);
    break;

  case LPSwitcher::TypeZephyrXtreme:
    ret=new ZephyrXtreme(id,parent);
    break;

  case LPSwitcher::TypeLast:
    break;
  }

  if(ret==NULL) {
    syslog(LOG_WARNING,"LPSwitcherFactory: requested unknown switcher type %d",
	   type);
  }

  return ret;
}
