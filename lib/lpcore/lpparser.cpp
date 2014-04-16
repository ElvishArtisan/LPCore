// lpparser.cpp
//
// Parser for LP Messages
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpparser.cpp,v 1.5 2013/08/07 20:56:09 cvs Exp $
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

#include "lpparser.h"

LPParser::LPParser(QObject *parent)
  : QObject(parent)
{
  lp_ptr=0;
  lp_istate=0;
  lp_body_count=0;
}


void LPParser::writeData(const QByteArray &data)
{
  for(int i=0;i<data.size();i++) {
    switch(lp_istate) {
    case 0:   // Start of Message
      if((data[i]==0x02)||(data[i]==0x04)) {
	lp_buffer[0]=data[i];
	lp_ptr=1;
	lp_istate=1;
      }
      break;

    case 1:   // Message Length
      lp_buffer[1]=data[i];
      lp_body_count=0xFF&data[i];
      lp_ptr=2;
      lp_istate=2;
      break;

    case 2:   // Message Body
      lp_buffer[lp_ptr++]=data[i];
      if(--lp_body_count==0) {
	if(lp_buffer[0]==0x02) {
	  emit messageReceived(LPMessage(lp_buffer,lp_ptr));
	}
	if(lp_buffer[0]==0x04) {
	  emit metadataReceived(QByteArray(lp_buffer,lp_ptr));
	}
	lp_ptr=0;
	lp_istate=0;
      }
      break;

    default:
      lp_istate=0;
      break;
    }
  }
}


void LPParser::writeData(const char *data,int len)
{
  writeData(QByteArray(data,len));
}


bool LPParser::validate(const QByteArray &data)
{
  if(data.size()<3) {
    return false;
  }
  return (data[0]==0x02)&&((0xFF&data[1])==(data.size()-2));
}


bool LPParser::validate(const char *data,int len)
{
  return LPParser::validate(QByteArray(data,len));
}


QString LPParser::dump(const QByteArray &data)
{
  return LPMessage::dump(data);
}


QString LPParser::dump(const char *data,int len)
{
  return LPMessage::dump(QByteArray(data,len));
}
