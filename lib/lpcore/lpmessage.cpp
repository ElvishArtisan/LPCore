// lpmessage.cpp
//
// A container class for LP2 messages.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpmessage.cpp,v 1.7 2013/11/21 17:33:17 cvs Exp $
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

#include <stdio.h>

#include <QtCore/QObject>

#include "lpmessage.h"

int global_lpmessage_min_sizes[256]=
  {  0,  0,  0,  0,  3,  0,  0,  0,  7,  9,  0,  0,  0,  0,  0,  0,  // 0x00
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x10
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x20
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x30
    11, 11, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x40
     0,  9,  8,  8, 10,  8,  8, 10,  7,  9, 10, 11, 11,  0,  0,  0,  // 0x50
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x60
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x70
     0,  5,  5,  4,  7,  5,  0,  0, 10,  9,  8, 11,  9,  0,  0,  0,  // 0x80
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x90
     0,  7,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  5,  8,  0,  // 0xA0
     0,  8,  5,  5,  6,  5,  6,  0,  7,  0,  0,  0,  0,  0,  0,  0,  // 0xB0
     0,  7, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xC0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xD0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xE0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}; // 0xF0

int global_lpmessage_max_sizes[256]=
  {  0,  0,  0,  0,  3,  0,  0,  0,  7,  9,  0,  0,  0,  0,  0,  0,  // 0x00
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x10
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x20
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x30
    11, 11,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x40
     0,  9,  8,  8, 10,  8,  8, 10,  7,  9, 12, 18,255,  0,  0,  0,  // 0x50
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x60
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x70
     0,  5,255,  4,  7,255,  0,  0,255,255,  8, 11,255,  0,  0,  0,  // 0x80
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x90
     0,  7,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  5,  8,  0,  // 0xA0
     0, 15,  5,  5,  6,  5,255,  0,255,  0,  0,  0,  0,  0,  0,  0,  // 0xB0
     0,  7,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xC0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xD0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xE0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}; // 0xF0

LPMessage::LPMessage()
  : QByteArray()
{
}


LPMessage::LPMessage(const char *str)
  : QByteArray(str)
{
}


LPMessage::LPMessage(const char *data,int size)
  : QByteArray(data,size)
{
}


LPMessage::LPMessage(const QByteArray &data)
  : QByteArray(data)
{
}


LPMessage::Lp2Command LPMessage::command() const
{
  return (LPMessage::Lp2Command)at(2);
}


int LPMessage::engine() const
{
  int ret=-1;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdRead:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdMeterFull:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdMeterEighth:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdMeterTwelveth:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdMomentary:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdTurnOn:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdTurnOff:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdInputAssign:
    ret=0xFF&at(7);
    break;

  case LPMessage::CmdInputMode:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdFaderLevel:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdRampFader:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdStoreLevel:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdAuxLevel:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdEffectsLvl:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdSetIndicator:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdWriteText:
    ret=0xFF&at(3);
    break;

  case LPMessage::CmdLoginReq:
    break;
  }

  return ret;
}


int LPMessage::sourceEngine() const
{
  int ret=-1;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
  case LPMessage::CmdRead:
  case LPMessage::CmdMeterFull:
  case LPMessage::CmdMeterEighth:
  case LPMessage::CmdMeterTwelveth:
  case LPMessage::CmdMomentary:
  case LPMessage::CmdTurnOn:
  case LPMessage::CmdTurnOff:
  case LPMessage::CmdInputMode:
  case LPMessage::CmdFaderLevel:
  case LPMessage::CmdRampFader:
  case LPMessage::CmdStoreLevel:
  case LPMessage::CmdAuxLevel:
  case LPMessage::CmdEffectsLvl:
  case LPMessage::CmdSetIndicator:
  case LPMessage::CmdWriteText:
  case LPMessage::CmdLoginReq:
    break;

  case LPMessage::CmdInputAssign:
    ret=0xFF&at(7);
    break;
  }

  return ret;
}


int LPMessage::device() const
{
  int ret=-1;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
  case LPMessage::CmdRead:
  case LPMessage::CmdMeterFull:
  case LPMessage::CmdMeterEighth:
  case LPMessage::CmdMeterTwelveth:
  case LPMessage::CmdMomentary:
  case LPMessage::CmdTurnOn:
  case LPMessage::CmdTurnOff:
  case LPMessage::CmdInputAssign:
  case LPMessage::CmdInputMode:
  case LPMessage::CmdFaderLevel:
  case LPMessage::CmdRampFader:
  case LPMessage::CmdStoreLevel:
  case LPMessage::CmdAuxLevel:
  case LPMessage::CmdEffectsLvl:
  case LPMessage::CmdSetIndicator:
  case LPMessage::CmdWriteText:
    ret=((0xFF&at(4))<<8)+(0xFF&at(5));
    break;

  case LPMessage::CmdLoginReq:
    break;
  }

  return ret;
}


int LPMessage::sourceDevice() const
{
  int ret=-1;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
  case LPMessage::CmdRead:
  case LPMessage::CmdMeterFull:
  case LPMessage::CmdMeterEighth:
  case LPMessage::CmdMeterTwelveth:
  case LPMessage::CmdMomentary:
  case LPMessage::CmdTurnOn:
  case LPMessage::CmdTurnOff:
  case LPMessage::CmdInputMode:
  case LPMessage::CmdFaderLevel:
  case LPMessage::CmdRampFader:
  case LPMessage::CmdStoreLevel:
  case LPMessage::CmdAuxLevel:
  case LPMessage::CmdEffectsLvl:
  case LPMessage::CmdSetIndicator:
  case LPMessage::CmdWriteText:
  case LPMessage::CmdLoginReq:
    break;

  case LPMessage::CmdInputAssign:
    ret=((0xFF&at(8))<<8)+(0xFF&at(9));
    break;
  }

  return ret;
}


int LPMessage::surface() const
{
  int ret=-1;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
  case LPMessage::CmdRead:
  case LPMessage::CmdMeterFull:
  case LPMessage::CmdMeterEighth:
  case LPMessage::CmdMeterTwelveth:
  case LPMessage::CmdMomentary:
  case LPMessage::CmdTurnOn:
  case LPMessage::CmdTurnOff:
  case LPMessage::CmdInputAssign:
  case LPMessage::CmdInputMode:
  case LPMessage::CmdFaderLevel:
  case LPMessage::CmdRampFader:
  case LPMessage::CmdStoreLevel:
  case LPMessage::CmdAuxLevel:
  case LPMessage::CmdEffectsLvl:
  case LPMessage::CmdSetIndicator:
  case LPMessage::CmdWriteText:
    ret=0xFF&at(6);
    break;

  case LPMessage::CmdLoginReq:
    break;
  }

  return ret;
}


int LPMessage::buss() const
{
  int ret=-1;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
  case LPMessage::CmdRead:
  case LPMessage::CmdMeterFull:
  case LPMessage::CmdMeterEighth:
  case LPMessage::CmdMeterTwelveth:
  case LPMessage::CmdInputAssign:
  case LPMessage::CmdInputMode:
  case LPMessage::CmdFaderLevel:
  case LPMessage::CmdRampFader:
  case LPMessage::CmdStoreLevel:
  case LPMessage::CmdAuxLevel:
  case LPMessage::CmdEffectsLvl:
  case LPMessage::CmdSetIndicator:
  case LPMessage::CmdWriteText:
  case LPMessage::CmdLoginReq:
    break;

  case LPMessage::CmdMomentary:
  case LPMessage::CmdTurnOn:
  case LPMessage::CmdTurnOff:
    ret=0xFF&at(7);
    break;
  }

  return ret;
}


int LPMessage::level() const
{
  int ret=-1;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
  case LPMessage::CmdRead:
  case LPMessage::CmdMeterFull:
  case LPMessage::CmdMeterEighth:
  case LPMessage::CmdMeterTwelveth:
  case LPMessage::CmdMomentary:
  case LPMessage::CmdTurnOn:
  case LPMessage::CmdTurnOff:
  case LPMessage::CmdInputAssign:
  case LPMessage::CmdInputMode:
  case LPMessage::CmdStoreLevel:
  case LPMessage::CmdAuxLevel:
  case LPMessage::CmdEffectsLvl:
  case LPMessage::CmdSetIndicator:
  case LPMessage::CmdWriteText:
  case LPMessage::CmdLoginReq:
    break;

  case LPMessage::CmdFaderLevel:
    ret=0xFF&at(7);
    break;

  case LPMessage::CmdRampFader:
    ret=0xFF&at(8);
    break;
  }

  return ret;
}


LPMessage::ChannelMode LPMessage::channelMode() const
{
  LPMessage::ChannelMode ret=LPMessage::ModeNone;

  switch((LPMessage::Lp2Command)(0xFF&at(2))) {
  case LPMessage::CmdPing:
  case LPMessage::CmdRead:
  case LPMessage::CmdMeterFull:
  case LPMessage::CmdMeterEighth:
  case LPMessage::CmdMeterTwelveth:
  case LPMessage::CmdMomentary:
  case LPMessage::CmdTurnOn:
  case LPMessage::CmdTurnOff:
  case LPMessage::CmdInputAssign:
  case LPMessage::CmdFaderLevel:
  case LPMessage::CmdRampFader:
  case LPMessage::CmdStoreLevel:
  case LPMessage::CmdAuxLevel:
  case LPMessage::CmdEffectsLvl:
  case LPMessage::CmdSetIndicator:
  case LPMessage::CmdWriteText:
  case LPMessage::CmdLoginReq:
    break;

  case LPMessage::CmdInputMode:
    ret=(LPMessage::ChannelMode)(0xFF&at(7));
    break;
  }

  return ret;
}


bool LPMessage::isValid() const
{
  return (size()>=3)&&(at(0)==0x02)&&(at(1)==(size()-2))&&
    (global_lpmessage_min_sizes[(int)at(2)]<=size())&&
    (global_lpmessage_max_sizes[(int)at(2)]>=size());
}


QString LPMessage::dump() const
{
  return LPMessage::dump(data(),size());
}


QString LPMessage::dump(const QByteArray &data)
{
  QString ret;

  for(int i=0;i<data.size();i++) {
    ret+=QString().sprintf("%02X ",0xFF&data[i]);
  }
  ret=ret.left(ret.length()-1);

  return ret;
}


QString LPMessage::dump(const char *data,int len)
{
  return LPMessage::dump(QByteArray(data,len));
}


QString LPMessage::text(const QByteArray &data)
{
  QString ret="";

  if((0xFF&data[2])==0x5C) {
    ret=data.right(data.length()-10);
  }

  return ret;
}


QString LPMessage::text(const char *data,int len)
{
  return LPMessage(QByteArray(data,len));
}


QString LPMessage::decode() const
{
  QString ret=QObject::tr("invalid");
  if(isValid()) {
    ret=LPMessage::commandText(command());
    if(engine()>0) {
      ret+=QString().sprintf(", engine=%d",engine());
    }
    if(device()>=0) {
      ret+=QString().sprintf(", device=0x%04X",device());
    }
    if(surface()>=0) {
      ret+=QString().sprintf(", surface=%d",surface());
    }
    if(buss()>=0) {
      ret+=QString().sprintf(", bus=%d",buss());
    }
    if(sourceEngine()>0) {
      ret+=QString().sprintf(", srcEngine=%d",sourceEngine());
    }
    if(level()>=0) {
      ret+=QString().sprintf(", level=%d",level());
    }
    if(sourceDevice()>=0) {
      ret+=QString().sprintf(", srcDevice=0x%04X",sourceDevice());
    }
    if(channelMode()!=LPMessage::ModeNone) {
      ret+=", mode="+LPMessage::modeText(channelMode());
    }
    if(!LPMessage::text(*this).isEmpty()) {
      ret+=", text='"+LPMessage::text(*this)+"'";
    }
  }
  return ret;
}


QString LPMessage::commandText(LPMessage::Lp2Command cmd)
{
  QString ret=QObject::tr("Unknown");

  switch(0xFF&cmd) {
  case LPMessage::CmdPing:
    ret=QObject::tr("Ping");
    break;

  case LPMessage::CmdRead:
    ret=QObject::tr("Read");
    break;

  case LPMessage::CmdMeterFull:
    ret=QObject::tr("Meter Level (full)");
    break;

  case LPMessage::CmdMeterEighth:
    ret=QObject::tr("Meter Level (1/8)");
    break;

  case LPMessage::CmdMeterTwelveth:
    ret=QObject::tr("Meter Level (1/12)");
    break;

  case LPMessage::CmdMomentary:
    ret=QObject::tr("Momentary Relay");
    break;

  case LPMessage::CmdTurnOn:
    ret=QObject::tr("Turn ON");
    break;

  case LPMessage::CmdTurnOff:
    ret=QObject::tr("Turn OFF");
    break;

  case LPMessage::CmdInputAssign:
    ret=QObject::tr("Input Assign");
    break;

  case LPMessage::CmdInputMode:
    ret=QObject::tr("Input Mode");
    break;

  case LPMessage::CmdFaderLevel:
    ret=QObject::tr("Fader Level");
    break;

  case LPMessage::CmdRampFader:
    ret=QObject::tr("Ramp Fader");
    break;

  case LPMessage::CmdStoreLevel:
    ret=QObject::tr("Store Current Fader Level");
    break;

  case LPMessage::CmdAuxLevel:
    ret=QObject::tr("Auxiliary Levels");
    break;

  case LPMessage::CmdEffectsLvl:
    ret=QObject::tr("Effects Level");
    break;

  case LPMessage::CmdSetIndicator:
    ret=QObject::tr("Set Indicator");
    break;

  case LPMessage::CmdWriteText:
    ret=QObject::tr("Write Text");
    break;

  case LPMessage::CmdLoginReq:
    ret=QObject::tr("Login Required");
    break;
  }

  return ret;
}


QString LPMessage::modeText(LPMessage::ChannelMode mode)
{
  QString ret=QObject::tr("Unknown");

  switch(mode) {
  case ModeStereo:
    ret=QObject::tr("Stereo");
    break;

  case ModeMono:
    ret=QObject::tr("Mono");
    break;

  case ModeReverse:
    ret=QObject::tr("Phase Reversed");
    break;

  case ModeLeftSum:
    ret=QObject::tr("Left to Both");
    break;

  case ModeRightSum:
    ret=QObject::tr("Right to Both");
    break;

  case ModeLeftOnly:
    ret=QObject::tr("Left Only");
    break;

  case ModeRightOnly:
    ret=QObject::tr("Right Only");
    break;

  case ModeNone:
    break;
  }

  return ret;
}
