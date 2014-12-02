// switcher_engine.h
//
// Engine Driver for LPCore Switcher Devices
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_engine.h,v 1.1 2013/12/31 19:31:06 cvs Exp $
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

#ifndef SWITCHER_ENGINE_H
#define SWITCHER_ENGINE_H

#include <lpcore/lpparser.h>
#include <lpcore/lpswitcher.h>

#include "engine.h"

#define SWITCHER_ENGINE_GPI_CHAN 0x02
#define SWITCHER_ENGINE_GPO_CHAN 0x01

class SwitcherEngine : public Engine
{
  Q_OBJECT;
 public:
  SwitcherEngine(LPConfig *config,unsigned n,QObject *parent=NULL);
  ~SwitcherEngine();
  Engine::Type type() const;
  bool isReset() const;
  bool open();
  int write(const QByteArray &data) const;
  int write(const char *data,int len) const;

 protected:
  void resetDevice();

 private slots:
  void messageReceivedData(const LPMessage &msg);
  void gpiChangedData(int id,int line,bool state);

 private:
  LPParser *switch_parser;
  LPSwitcher *switch_switcher;
  QString switch_device;
  bool switch_is_reset;
};


#endif  // SWITCHER_ENGINE_H
