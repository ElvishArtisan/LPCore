// lpparser.h
//
// Parser for LP Messages
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpparser.h,v 1.3 2013/08/07 20:56:09 cvs Exp $
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

#ifndef LPPARSER_H
#define LPPARSER_H

#include <stdint.h>

#include <QtCore/QObject>
#include <QtCore/QByteArray>

#include <lpcore/lpmessage.h>

class LPParser : public QObject
{
  Q_OBJECT;
 public:
  LPParser(QObject *parent=NULL);
  void writeData(const QByteArray &data);
  void writeData(const char *data,int len);
  static bool validate(const QByteArray &data);
  static bool validate(const char *data,int len);
  static QString dump(const QByteArray &data);
  static QString dump(const char *data,int len);

 signals:
  void messageReceived(const LPMessage &msg);
  void metadataReceived(const QByteArray &msg);

 private:
  char lp_buffer[1024];
  int lp_ptr;
  int lp_istate;
  int lp_body_count;
};


#endif  // LPPARSER_H
