// switcher_btss82.cpp
//
// LPSwitcher implementation for the BroadcastTools SS8.2
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_btss82.cpp,v 1.5 2014/03/10 22:21:00 cvs Exp $
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

#include "switcher_btss82.h"

BtSs82::BtSs82(int id,QObject *parent)
  : LPSwitcher(id,LPSwitcher::TypeBtSs82,parent)
{
  for(unsigned i=0;i<BTSS82_OUTPUTS;i++) {
    bt_crosspoints[i]=0;
  }
  for(unsigned i=0;i<BTSS82_GPIS;i++) {
    bt_gpi_states[i]=false;
  }
  bt_silence_sense_states[0]=false;
  bt_silence_sense_states[1]=false;

  bt_device=new LPTTYDevice(this);
  bt_device->setSpeed(BTSS82_SPEED);
  bt_device->setWordLength(BTSS82_WORD_LENGTH);
  bt_device->setParity(BTSS82_PARITY);
  bt_device->setFlowControl(BTSS82_FLOW_CONTROL);
  connect(bt_device,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  bt_poll_counter=0;
  bt_poll_timer=new QTimer(this);
  connect(bt_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
}


BtSs82::~BtSs82()
{
  delete bt_poll_timer;
  delete bt_device;
}


bool BtSs82::open(const QString &device)
{
  bt_device->setDeviceName(device);
  if(!bt_device->open(QIODevice::ReadWrite)) {
    return false;
  }
  bt_poll_timer->start(BTSS82_POLL_INTERVAL);

  return true;
}


void BtSs82::connectToHost(const QString &hostname,const QString &password,
			   uint16_t port)
{
}


int BtSs82::inputs()
{
  return BTSS82_INPUTS;
}


int BtSs82::outputs()
{
  return BTSS82_OUTPUTS;
}


int BtSs82::gpis()
{
  return BTSS82_GPIS;
}


int BtSs82::gpos()
{
  return BTSS82_GPOS;
}


int BtSs82::lines()
{
  return BTSS82_LINES;
}


int BtSs82::crosspoint(int output)
{
  return bt_crosspoints[output];
}


void BtSs82::setCrosspoint(int output,int input)
{
  bt_device->
    write(QString().sprintf("*%d%02d%d",BTSS82_DEVICE_ID,input+1,output+1).toAscii(),5);
}


bool BtSs82::gpiState(int gpi)
{
  return bt_gpi_states[gpi];
}


void BtSs82::pulseGpo(int gpo)
{
  bt_device->write(QString().sprintf("*%dOR%dP",BTSS82_DEVICE_ID,gpo+1).toAscii(),6);
}


bool BtSs82::silenceSense(int chan)
{
  return bt_silence_sense_states[chan];
}


void BtSs82::readyReadData()
{
  char data[1024];
  int n;

  while((n=bt_device->read(data,1024))>0) {
    for(int i=0;i<n;i++) {
      switch(data[i]) {
      case 13:
	ProcessStatus(bt_buffer);
	bt_buffer="";
	break;

      case 10:
	break;

      default:
	bt_buffer+=data[i];
	break;
      }
    }
  }
}


void BtSs82::pollData()
{
  switch((bt_poll_counter++)%3) {
  case 0:
    bt_device->write(QString().sprintf("*%dSL",BTSS82_DEVICE_ID).toAscii(),4);
    break;

  case 1:
    bt_device->write(QString().sprintf("*%dSPA",BTSS82_DEVICE_ID).toAscii(),5);
    break;

  case 2:
    bt_device->write(QString().sprintf("*%dSS",BTSS82_DEVICE_ID).toAscii(),4);
    break;
  }
}


void BtSs82::ProcessStatus(const QString &buf)
{
  if(buf.left(3)==QString().sprintf("S%dL",BTSS82_DEVICE_ID)) {
    ProcessCrosspointStatus(buf);
  }
  if(buf.left(3)==QString().sprintf("S%dP",BTSS82_DEVICE_ID)) {
    ProcessPipStatus(buf);
  }
  if(buf.left(3)==QString().sprintf("S%dS",BTSS82_DEVICE_ID)) {
    ProcessSilenceSenseStatus(buf);
  }
}


void BtSs82::ProcessPipStatus(const QString &buf)
{
  QStringList f1=buf.split(",");
  if(f1.size()==18) {
    for(int i=0;i<BTSS82_GPIS;i++) {
      if((f1[i+2]=="1")!=bt_gpi_states[i]) {
	bt_gpi_states[i]=f1[i+2]=="1";
	emit gpiChanged(id(),i,bt_gpi_states[i]);
      }
    }
  }
}


void BtSs82::ProcessCrosspointStatus(const QString &buf)
{
  int output=buf.mid(3,1).toInt()-1;
  QStringList f1=buf.split(",");

  if(f1.size()==9) {
    for(int i=0;i<BTSS82_INPUTS;i++) {
      if((f1[i+1]=="1")&&(bt_crosspoints[output]!=i)) {
	bt_crosspoints[output]=i;
	emit crosspointChanged(id(),output,i);
      }
    }
  }
}


void BtSs82::ProcessSilenceSenseStatus(const QString &buf)
{
  QStringList f1=buf.split(",");

  if(f1.size()==3) {
    for(int i=0;i<2;i++) {
      if((f1[i+1]=="1")!=bt_silence_sense_states[i]) {
	bt_silence_sense_states[i]=f1[i+1]=="1";
	emit silenceSenseChanged(id(),i,bt_silence_sense_states[i]);
      }
    }
  }
}
