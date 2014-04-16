// lpprofile.h
//
// Class for reading INI configuration files.
//
// (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lpprofile.h,v 1.2 2013/07/23 14:13:10 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef LPPROFILE_H
#define LPPROFILE_H

#include <vector>

#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtNetwork/QHostAddress>

class LPProfileLine
{
 public:
  LPProfileLine();
  QString tag() const;
  void setTag(QString tag);
  QString value() const;
  void setValue(QString value);
  void clear();

 private:
  QString line_tag;
  QString line_value;
};


class LPProfileSection
{
 public:
  LPProfileSection();
  QString name() const;
  void setName(QString name);
  bool getValue(QString tag,QString *value) const;
  void addValue(QString tag,QString value);
  void clear();

 private:
  QString section_name;
  std::vector<LPProfileLine> section_line;
};


class LPProfile
{
 public:
  LPProfile();
  QString source() const;
  bool setSource(const QString &filename);
  bool setSource(std::vector<QString> *values);
  QString stringValue(const QString &section,const QString &tag,
		      const QString &default_value="",bool *ok=0) const;
  int intValue(const QString &section,const QString &tag,
	       int default_value=0,bool *ok=0) const;
  int hexValue(const QString &section,const QString &tag,
	       int default_value=0,bool *ok=0) const;
  float floatValue(const QString &section,const QString &tag,
		   float default_value=0.0,bool *ok=0) const;
  double doubleValue(const QString &section,const QString &tag,
		    double default_value=0.0,bool *ok=0) const;
  bool boolValue(const QString &section,const QString &tag,
		 bool default_value=false,bool *ok=0) const;
  QTime timeValue(const QString &section,const QString &tag,
		  const QTime &default_value=QTime(),bool *ok=0);
  QHostAddress addressValue(const QString &section,const QString &tag,
			    const QHostAddress &default_value=QHostAddress(),
			    bool *ok=0);
  QHostAddress addressValue(const QString &section,const QString &tag,
			    const QString &default_value="",bool *ok=0);
  void clear();

 private:
  QString profile_source;
  std::vector<LPProfileSection> profile_section;
};


#endif  // LPPROFILE_H
