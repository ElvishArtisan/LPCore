// lpmessage.h
//
// A container class for LP2 messages.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpmessage.h,v 1.6 2013/11/21 17:33:17 cvs Exp $
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

#ifndef LPMESSAGE_H
#define LPMESSAGE_H

#include <QtCore/QByteArray>
#include <QtCore/QString>

class LPMessage : public QByteArray
{
 public:
  enum Lp2Command {CmdPing=0x08,CmdRead=0x09,CmdMeterFull=0x40,
		   CmdMeterEighth=0x41,CmdMeterTwelveth=0x42,CmdMomentary=0x51,
		   CmdTurnOn=0x52,CmdTurnOff=0x53,CmdInputAssign=0x54,
		   CmdInputMode=0x55,CmdFaderLevel=0x56,CmdRampFader=0x57,
		   CmdStoreLevel=0x58,CmdAuxLevel=0x59,CmdEffectsLvl=0x5A,
		   CmdSetIndicator=0x5B,CmdWriteText=0x5C,CmdLoginReq=0xF9};
  enum ChannelMode {ModeStereo=0,ModeMono=1,ModeReverse=2,ModeLeftSum=3,
		    ModeRightSum=4,ModeLeftOnly=5,ModeRightOnly=6,ModeNone=7};
  LPMessage();
  LPMessage(const char *str);
  LPMessage(const char *data,int size);
  LPMessage(const QByteArray &data);
  LPMessage::Lp2Command command() const;
  int engine() const;
  int sourceEngine() const;
  int device() const;
  int sourceDevice() const;
  int surface() const;
  int buss() const;
  int level() const;
  LPMessage::ChannelMode channelMode() const;
  bool isValid() const;
  QString dump() const;
  QString decode() const;
  static QString dump(const QByteArray &data);
  static QString dump(const char *data,int len);
  static QString text(const QByteArray &data);
  static QString text(const char *data,int len);
  static QString commandText(LPMessage::Lp2Command cmd);
  static QString modeText(LPMessage::ChannelMode mode);
};


#endif  // LPMESSAGE_H
