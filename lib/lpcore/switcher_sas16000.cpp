// switcher_sas16000.cpp
//
// LPSwitcher implementation for SAS User Serial Protocol (2 digit)
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <QtCore/QStringList>

#include "switcher_sas16000.h"

Sas16000::Sas16000(int id,QObject *parent)
  : LPSwitcher(id,LPSwitcher::TypeSas16000,parent)
{
  bt_xmap=false;
  for(unsigned i=0;i<SAS16000_OUTPUTS;i++) {
    bt_crosspoints[i]=0;
  }

  bt_device=new LPTTYDevice(this);
  bt_device->setSpeed(SAS16000_SPEED);
  bt_device->setWordLength(SAS16000_WORD_LENGTH);
  bt_device->setParity(SAS16000_PARITY);
  bt_device->setFlowControl(SAS16000_FLOW_CONTROL);
  connect(bt_device,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


Sas16000::~Sas16000()
{
  delete bt_device;
}


bool Sas16000::open(const QString &device)
{
  bt_device->setDeviceName(device);
  if(!bt_device->open(QIODevice::ReadWrite)) {
    return false;
  }
  bt_device->write(QString().sprintf("%c99",9).toAscii(),3);

  return true;
}


void Sas16000::connectToHost(const QString &hostname,const QString &password,
			   uint16_t port)
{
}


int Sas16000::inputs()
{
  return SAS16000_INPUTS;
}


int Sas16000::outputs()
{
  return SAS16000_OUTPUTS;
}


int Sas16000::gpis()
{
  return 0;
}


int Sas16000::gpos()
{
  return 0;
}


int Sas16000::lines()
{
  return 0;
}


int Sas16000::crosspoint(int output)
{
  return bt_crosspoints[output];
}


void Sas16000::setCrosspoint(int output,int input)
{
  char buf[6];
  sprintf(buf,"\x14%02d%02d",input+1,output+1);
  bt_device->write(buf,5);
}


bool Sas16000::gpiState(int gpi)
{
  return false;
}


void Sas16000::pulseGpo(int gpo)
{
}


bool Sas16000::silenceSense(int chan)
{
  return false;
}


void Sas16000::readyReadData()
{
  char data[1024];
  int n;

  while((n=bt_device->read(data,1024))>0) {
    data[n]=0;
    for(int i=0;i<n;i++) {
      switch(0xFF&data[i]) {
      case 10:
	ProcessStatus(bt_buffer);
	bt_buffer="";
	break;

      case 13:
	break;

      default:
	bt_buffer+=data[i];
	break;
      }
    }
  }
}


void Sas16000::ProcessStatus(const QString &buf)
{
  bool ok=false;

  if(bt_xmap) {
    QStringList f0=buf.split(" ",QString::SkipEmptyParts);
    for(int i=0;i<f0.size();i++) {
      bt_crosspoints[i]=f0[i].toInt()-1;
      emit crosspointChanged(id(),i,f0[i].toInt()-1);
    }
    bt_xmap=false;
  }
  else {
    if(buf=="XMAP") {
      bt_xmap=true;
    }
    else {
      if((buf.length()==6)&&(buf.left(1)=="T")) {
	QStringList f0=buf.right(5).split(":");
	if(f0.size()==2) {
	  unsigned input=f0[0].toUInt(&ok)-1;
	  if(ok) {
	    unsigned output=f0[1].toUInt(&ok)-1;
	    if(ok&&(input<SAS16000_INPUTS)&&(output<SAS16000_OUTPUTS)) {
	      bt_crosspoints[output]=input;
	      emit crosspointChanged(id(),output,input);
	    }
	  }
	}
      }
    }
  }
}
