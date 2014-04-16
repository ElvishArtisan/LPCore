// ae_engine.h
//
// Engine Driver for Audio Engines
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ae_engine.h,v 1.2 2013/12/31 19:31:05 cvs Exp $ engine.h,v 1.13 2013/08/20 16:55:39 cvs Exp $
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

#ifndef AE_ENGINE_H
#define AE_ENGINE_H

#include "engine.h"

class AeEngine : public Engine
{
  Q_OBJECT;
 public:
  AeEngine(LPConfig *config,unsigned n,QObject *parent=NULL);
  ~AeEngine();
  Engine::Type type() const;
  bool isReset() const;
  bool open();
  int write(const QByteArray &data) const;
  int write(const char *data,int len) const;
  void reset();

 private slots:
  void messageReceivedData(const LPMessage &msg);
  void readyReadData();
  void pingData();
  void resetData();

 private:
  bool engine_is_reset;
  LPTTYDevice *engine_device;
  LPParser *engine_parser;
  QTimer *engine_reset_timer;
  QTimer *engine_ping_timer;
};


#endif  // AE_ENGINE_H
