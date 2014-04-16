// enginefactory.h
//
// Create an Engine instance.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: enginefactory.h,v 1.2 2013/12/31 19:31:06 cvs Exp $
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

#ifndef ENGINEFACTORY_H
#define ENGINEFACTORY_H

#include "engine.h"

Engine *EngineFactory(LPConfig *config,unsigned n,QObject *parent=NULL);


#endif  // ENGINEFACTORY_H
