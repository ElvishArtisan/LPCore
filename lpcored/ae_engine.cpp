// ae_engine.cpp
//
// Engine Driver for Audio Engines
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ae_engine.cpp,v 1.2 2013/12/31 19:31:05 cvs Exp $
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

#include "ae_engine.h"

AeEngine::AeEngine(LPConfig *config,unsigned n,QObject *parent)
  : Engine(config,n,parent)
{
  engine_is_reset=false;

  //
  // Message Parser
  //
  engine_parser=new LPParser(this);
  connect(engine_parser,SIGNAL(messageReceived(const LPMessage &)),
	  this,SLOT(messageReceivedData(const LPMessage &)));

  //
  // Ping Timer
  //
  engine_ping_timer=new QTimer(this);
  engine_ping_timer->setSingleShot(true);
  connect(engine_ping_timer,SIGNAL(timeout()),this,SLOT(pingData()));

  //
  // Reset Timer
  //
  engine_reset_timer=new QTimer(this);
  engine_reset_timer->setSingleShot(true);
  connect(engine_reset_timer,SIGNAL(timeout()),this,SLOT(resetData()));

  //
  // Serial Device
  //
  engine_device=new LPTTYDevice(this);
  engine_device->setDeviceName(config->engineSerialDevice(n));
  engine_device->setSpeed(LPCORE_ENGINE_SERIAL_SPEED);
  engine_device->setWordLength(LPCORE_ENGINE_SERIAL_WORD_LENGTH);
  engine_device->setParity(LPCORE_ENGINE_SERIAL_PARITY);
  engine_device->setFlowControl(LPCORE_ENGINE_SERIAL_FLOW_CONTROL);
  connect(engine_device,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


AeEngine::~AeEngine()
{
  delete engine_reset_timer;
  delete engine_device;
}


Engine::Type AeEngine::type() const
{
  return Engine::LogitekAeType;
}


bool AeEngine::isReset() const
{
  return engine_is_reset;
}


bool AeEngine::open()
{
  return engine_device->open(QIODevice::ReadWrite);
}


int AeEngine::write(const QByteArray &data) const
{
  return engine_device->write(data);
}


int AeEngine::write(const char *data,int len) const
{
  return write(QByteArray(data,len));
}


void AeEngine::resetDevice()
{
  uint8_t msg[7]={0x02,0x05,0x84,0x12,0x00,0x01,0xAA};
  write((char *)msg,7);
  engine_reset_timer->start(LPCORE_STARTUP_TIMEOUT_INTERVAL);
}


void AeEngine::messageReceivedData(const LPMessage &msg)
{
  //
  // Check for reset finish
  //
  if((msg.size()==5)&&((0xFF&msg[2])==0xB2)&&((0xFF&msg[3])==0x02)&&
     ((0xFF&msg[4])==0xFF)) {
    if(engine_reset_timer->isActive()) {
      if(!engine_is_reset) {
	engine_is_reset=true;
	emit resetFinished(engineNumber(),true);
      }
      engine_reset_timer->stop();
      engine_ping_timer->start(LPCORE_ENGINE_PING_INTERVAL);
    }
  }

  //
  // Check for ping response
  //
  if((msg.size()==7)&&((0xFF&msg[2])==0x84)) {
    engine_reset_timer->stop();
    engine_ping_timer->start(LPCORE_ENGINE_PING_INTERVAL);
  }

  emit messageReceived(engineNumber(),msg);
}


void AeEngine::readyReadData()
{
  char data[1024];
  int n=0;

  while((n=engine_device->read(data,1024))>0) {
    engine_parser->writeData(data,n);
  }
}


void AeEngine::pingData()
{
  char lp2[256];

  lp2[0]=0x02;
  lp2[1]=0x01;
  lp2[2]=0x04;   // Ping
  write(lp2,3);
  engine_reset_timer->start(LPCORE_STARTUP_TIMEOUT_INTERVAL);
}


void AeEngine::resetData()
{
  if(engine_is_reset) {
    engine_is_reset=false;
    emit resetFinished(engineNumber(),false);
  }
 reset();
}
