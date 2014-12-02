// engine.h
//
// Base Class for LP Engine Drivers
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: engine.h,v 1.15 2013/12/31 19:31:06 cvs Exp $
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

#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QTimer>

#include <lpcore/lpconfig.h>
#include <lpcore/lpmessage.h>
#include <lpcore/lpparser.h>
#include <lpcore/lpttydevice.h>

class SourceDevice
{
 public:
  SourceDevice(int device);
  int device() const;
  void setDevice(int dev);
  int surface() const;
  void setSurface(int surf);
  int mode() const;
  void setMode(int mode);
  int faderLevel() const;
  void setFaderLevel(int lvl);
  bool busState(int bus) const;
  void setBusState(int bus,bool state);

 private:
  int src_device;
  int src_surface;
  int src_mode;
  int src_fader_level;
  bool src_bus_states[LPCORE_MAX_BUSSES];
};


class Engine : public QObject
{
  Q_OBJECT;
 public:
  enum Type {LogitekAeType=0,SwitcherType=1};
  Engine(LPConfig *config,unsigned n,QObject *parent=NULL);
  ~Engine();
  void reset();
  virtual Engine::Type type() const=0;
  virtual bool isReset() const=0;
  virtual bool open()=0;
  virtual int write(const QByteArray &data) const=0;
  virtual int write(const char *data,int len) const=0;
  int engineNumber() const;
  SourceDevice *sourceDevice(uint8_t chan);
  SourceDevice *sourceDevice(int device,int surface);
  void setSourceDevice(uint8_t chan,int device);
  void setSourceMode(uint8_t chan,int mode);
  void setSourceFaderLevel(uint8_t chan,int fader_lvl);
  void setSourceBusState(uint8_t chan,int bus,bool state);
  int destinationChannel(int device,int surface);
  static QString typeString(Engine::Type type);

 signals:
  void messageReceived(int engine,const QByteArray &data);
  void resetFinished(int engine,bool state);

 protected:
  virtual void resetDevice()=0;
  LPConfig *config();

 private:
  std::vector<SourceDevice *> engine_source_devices;
  int engine_number;
};


#endif  // ENGINE_H
