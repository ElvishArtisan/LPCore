// lpprofileline.cpp
//
// A container class for profile lines.
//
// (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lpprofile.cpp,v 1.2 2013/07/23 14:13:09 cvs Exp $
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

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>

#include "lpprofile.h"

LPProfileLine::LPProfileLine()
{
  clear();
}


QString LPProfileLine::tag() const
{
  return line_tag;
}


void LPProfileLine::setTag(QString tag)
{
  line_tag=tag;
}


QString LPProfileLine::value() const
{
  return line_value;
}


void LPProfileLine::setValue(QString value)
{
  line_value=value;
}


void LPProfileLine::clear()
{
  line_tag="";
  line_value="";
}


LPProfileSection::LPProfileSection()
{
  clear();
}


QString LPProfileSection::name() const
{
  return section_name;
}


void LPProfileSection::setName(QString name)
{
  section_name=name;
}


bool LPProfileSection::getValue(QString tag,QString *value) const
{
  for(unsigned i=0;i<section_line.size();i++) {
    if(section_line[i].tag()==tag) {
      *value=section_line[i].value();
      return true;
    }
  }
  return false;
}


void LPProfileSection::addValue(QString tag,QString value)
{
  section_line.push_back(LPProfileLine());
  section_line.back().setTag(tag);
  section_line.back().setValue(value);
}


void LPProfileSection::clear()
{
  section_name="";
  section_line.resize(0);
}


LPProfile::LPProfile()
{
}


QString LPProfile::source() const
{
  return profile_source;
}


bool LPProfile::setSource(const QString &filename)
{
  QString section;
  int offset;

  profile_source=filename;
  profile_section.resize(0);
  profile_section.push_back(LPProfileSection());
  profile_section.back().setName("");
  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::ReadOnly)) {
    delete file;
    return false;
  }
  QTextStream *text=new QTextStream(file);
  QString line=text->readLine().trimmed();
  while(!line.isNull()) {
    if((line.left(1)!=";")&&(line.left(1)!="#")) {
      if((line.left(1)=="[")&&(line.right(1)=="]")) {
	section=line.mid(1,line.length()-2);
	profile_section.push_back(LPProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=line.indexOf('='))!=-1)) {
//      else if(((offset=line.indexOf('='))!=-1)&&(!section.isEmpty())) {
	profile_section.back().
	  addValue(line.left(offset),
		   line.right(line.length()-offset-1).trimmed());
      }
    }
    line=text->readLine().trimmed();
  }
  delete text;
  delete file;
  return true;
}


bool LPProfile::setSource(std::vector<QString> *values)
{
  QString section;
  int offset;

  profile_section.resize(0);
  profile_section.push_back(LPProfileSection());
  profile_section.back().setName("");
  for(unsigned i=0;i<values->size();i++) {
    if((values->at(i).left(1)!=";")&&(values->at(i).left(1)!="#")) {
      if((values->at(i).left(1)=="[")&&(values->at(i).right(1)=="]")) {
	section=values->at(i).mid(1,values->at(i).length()-2);
	profile_section.push_back(LPProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=values->at(i).indexOf('='))!=-1)) {
	profile_section.back().
	  addValue(values->at(i).left(offset),
		   values->at(i).right(values->at(i).length()-offset-1).
		   trimmed());
      }
    }
  }
  return true;
}


QString LPProfile::stringValue(const QString &section,const QString &tag,
			      const QString &default_str,bool *ok) const
{
  QString result;

  for(unsigned i=0;i<profile_section.size();i++) {
    if(profile_section[i].name()==section) {
      if(profile_section[i].getValue(tag,&result)) {
	if(ok!=NULL) {
	  *ok=true;
	}
	return result;
      }
      if(ok!=NULL) {
	*ok=false;
      }
      return default_str;
    }
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_str;
}


int LPProfile::intValue(const QString &section,const QString &tag,
		       int default_value,bool *ok) const
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,10);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


int LPProfile::hexValue(const QString &section,const QString &tag,
		       int default_value,bool *ok) const
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,16);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


float LPProfile::floatValue(const QString &section,const QString &tag,
			   float default_value,bool *ok) const
{
  bool valid;

  float result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


double LPProfile::doubleValue(const QString &section,const QString &tag,
			    double default_value,bool *ok) const
{
  bool valid;

  double result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


bool LPProfile::boolValue(const QString &section,const QString &tag,
			 bool default_value,bool *ok) const
{
  bool valid;

  QString str=stringValue(section,tag,"",&valid).toLower();
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if((str=="yes")||(str=="true")||(str=="on")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return true;
  }
  if((str=="no")||(str=="false")||(str=="off")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return false;
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_value;
}


QTime LPProfile::timeValue(const QString &section,const QString &tag,
			   const QTime &default_value,bool *ok)
{
  QStringList fields;
  bool ok1=false;
  QString str=stringValue(section,tag,"",&ok1);
  QTime ret(default_value);

  if(ok1) {
    fields=str.split(":");
    if(fields.size()==2) {
      ret.setHMS(fields[0].toInt(),fields[1].toInt(),0);
    }
    if(fields.size()==3) {
      ret.setHMS(fields[0].toInt(),fields[1].toInt(),fields[2].toInt());
    }
  }
  
  return ret;
}


QHostAddress LPProfile::addressValue(const QString &section,const QString &tag,
				     const QHostAddress &default_value,bool *ok)
{
  return QHostAddress(stringValue(section,tag,default_value.toString(),ok));
}


QHostAddress LPProfile::addressValue(const QString &section,const QString &tag,
				     const QString &default_value,bool *ok)
{
  return addressValue(section,tag,QHostAddress(default_value),ok);
}


void LPProfile::clear()
{
  profile_source="";
  profile_section.resize(0);
}
