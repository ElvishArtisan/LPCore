// switcher_am16.cpp
//
// LPSwitcher implementation for the 360 Systems AM16
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_am16.cpp,v 1.4 2013/11/06 01:35:23 cvs Exp $
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "switcher_am16.h"

Am16::Am16(int id,QObject *parent)
  : LPSwitcher(id,LPSwitcher::TypeAm16,parent)
{
  am_midi_socket=-1;
  am_notifier=NULL;
  am_sysex_active=false;
  am_data_ptr=0;
  am_poll_pending=false;
  for(int i=0;i<AM16_OUTPUTS;i++) {
    am_xpoint_map[i]=0;
  }

  //
  // Timeout Timer
  //
  am_timeout_timer=new QTimer(this);
  am_timeout_timer->setSingleShot(true);
  connect(am_timeout_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));

  //
  // Poll Timer
  //
  am_poll_timer=new QTimer(this);
  am_poll_timer->setSingleShot(false);
  connect(am_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
}


Am16::~Am16()
{
  if(am_notifier!=NULL) {
    delete am_notifier;
  }
  if(am_midi_socket>=0) {
    close(am_midi_socket);
  }
  delete am_timeout_timer;
  delete am_poll_timer;
}


bool Am16::open(const QString &device)
{
  long sockopt;

  if((am_midi_socket=::open(device.toAscii(),O_RDWR))<0) {
    syslog(LOG_WARNING,"unable to open MIDI device at \"%s\"",
	   (const char *)device.toAscii());
    return false;
  }
  sockopt=fcntl(am_midi_socket,F_GETFL,0);
  sockopt|=O_NONBLOCK;
  fcntl(am_midi_socket,F_SETFL,sockopt);

  am_notifier=new QSocketNotifier(am_midi_socket,QSocketNotifier::Read,this);
  connect(am_notifier,SIGNAL(activated(int)),this,SLOT(readyReadData(int)));

  am_poll_timer->start(AM16_POLL_INTERVAL);

  return true;
}


void Am16::connectToHost(const QString &hostname,const QString &password,
			 uint16_t port)
{
}


int Am16::inputs()
{
  return AM16_INPUTS;
}


int Am16::outputs()
{
  return AM16_OUTPUTS;
}


int Am16::gpis()
{
  return AM16_GPIS;
}


int Am16::gpos()
{
  return AM16_GPOS;
}


int Am16::lines()
{
  return AM16_LINES;
}


int Am16::crosspoint(int output)
{
  return am_xpoint_map[output];
}


void Am16::setCrosspoint(int output,int input)
{
  //
  // Save the desired values
  //
  am_pending_inputs.push_back(input);
  am_pending_outputs.push_back(output);

  //
  // Request the current crosspoint map
  //
  if(am_pending_inputs.size()==1) {
    pollData();
  }
}


bool Am16::gpiState(int gpi)
{
  return false;
}


void Am16::pulseGpo(int gpo)
{
}


bool Am16::silenceSense(int chan)
{
  return false;
}


void Am16::readyReadData(int sock)
{
  char data[1024];
  int n;

  while((n=read(sock,data,1024))>0) {
    for(int i=0;i<n;i++) {
      if(am_sysex_active) {
	am_data_buffer[am_data_ptr++]=data[i];
	if((0xFF&data[i])==AM16_SYSEX_END) {
	  ProcessMessage(am_data_buffer,am_data_ptr);
	  am_data_ptr=0;
	  am_sysex_active=false;
	}
      }
      else {
	if((0xFF&data[i])==AM16_SYSEX_START) {
	  am_data_buffer[am_data_ptr++]=data[i];
	  am_sysex_active=true;
	}
      }
    }
  }
}


void Am16::timeoutData()
{
  syslog(LOG_WARNING,
	 "AM16 driver: timed out waiting for crosspoint map, %lu event(s) lost",
	 am_pending_inputs.size());
  am_pending_inputs.clear();
  am_pending_outputs.clear();
  am_poll_pending=false;
}


void Am16::pollData()
{
  char data[256];

  if(!am_poll_pending) {
    data[0]=AM16_SYSEX_START;
    data[1]=0x00;
    data[2]=0x00;
    data[3]=AM16_SYSTEMS_ID;
    data[4]=AM16_DEVICE_NUMBER;
    data[5]=AM16_DEVICE_ADDRESS;
    data[6]=0x07;   // Request Program NN
    data[7]=AM16_PATCH_NUMBER;
    data[8]=AM16_SYSEX_END;
    write(am_midi_socket,data,9);
    am_timeout_timer->start(AM16_TIMEOUT_INTERVAL);
    am_poll_pending=true;
  }
}


void Am16::ProcessMessage(char *msg,int len)
{
  QString str;

  if((len<7)||((0xFF&msg[3])!=AM16_SYSTEMS_ID)||
     ((0xFF&msg[4])!=AM16_DEVICE_NUMBER)||
     ((0xFF&msg[5])!=AM16_DEVICE_ADDRESS)) {
    return;
  }

  switch(0xFF&msg[6]) {
  case 0x08:   // Receive program map
    if(am_pending_inputs.size()>0) {
      am_timeout_timer->stop();

      //
      // Apply Changes
      //
      for(int i=(int)am_pending_inputs.size()-1;i>=0;i--) {
	msg[8+am_pending_outputs[i]]=0xFF&(am_pending_inputs[i]+1);
      }
    }

    //
    // Update Local State
    //
    for(int i=0;i<AM16_OUTPUTS;i++) {
      if((msg[8+i]-1)!=(0xFF&am_xpoint_map[i])) {
	am_xpoint_map[i]=0xFF&(msg[8+i]-1);
	emit crosspointChanged(id(),i,am_xpoint_map[i]);
      }
    }

    if(am_pending_inputs.size()>0) {
      //
      // Clear Pending Events
      //
      am_pending_inputs.clear();
      am_pending_outputs.clear();

      //
      // Send to Programs
      //
      write(am_midi_socket,msg,len);
      msg[7]++;
      write(am_midi_socket,msg,len);

      //
      // Toggle Active Programs
      //
      msg[0]=0xC1;   // Channel 1
      msg[1]=AM16_PATCH_NUMBER+1;
      write(am_midi_socket,msg,2);
      msg[1]=AM16_PATCH_NUMBER;
      write(am_midi_socket,msg,2);
    }
    am_poll_pending=false;
    break;

  case 0x0B:   // ACK / NCK
    switch(0xFF&msg[7]) {
    case 0:
      // ACK -- command was successful!
      break;

    case 0x7E:
      syslog(LOG_NOTICE,"AM16 driver: data error");
      break;

    case 0x7F:
      syslog(LOG_NOTICE,
	  "AM16 driver: memory protect mode is on, cannot change crosspoints");
      break;

    default:
      syslog(LOG_NOTICE,"AM16 driver: received unknown ACK code [%d]",
	     0xFF&msg[7]);
      break;
    }
    break;

  default:
    for(int i=0;i<len;i++) {
      str+=QString().sprintf("%02X ",0xFF&msg[i]);
    }
    syslog(LOG_DEBUG,"AM16 driver: received unrecognized MIDI message [%s]",
	   (const char *)str.toAscii());
    break;
  }
}
