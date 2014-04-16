// switcher_zephyrclassic.cpp
//
// LPSwitcher implementation for the BroadcastTools SS8.2
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_zephyrclassic.cpp,v 1.4 2013/09/09 18:21:55 cvs Exp $
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

#include <syslog.h>

#include <QtCore/QStringList>

#include "switcher_zephyrclassic.h"

bool zephyclassic_supported_xmt_algos[LPSwitcher::XmtAlgoLast]=
  {true,   // G.722
   true,   // MPEG L2 Stereo
   true,   // MPEG L2 Joint Stereo
   true,   // MPEG L2 Mono 128
   true,   // MPEG L2 Mono 64
   true,   // MPEG L2 Half 64
   true,   // MPEG L3 Stereo
   true,   // MPEG L3 Joint Stereo
   false,  // MPEG L3 Mono 128
   true,   // MPEG L3 Dual/Mono
   false,  // AAC-LD Stereo
   false,  // AAC-LD Joint Stereo
   false,  // AAC-LD Joint Stereo 64
   false,  // AAC-LD Mono 128
   false,  // AAC-LD Mono 64
   false,  // AAC Joint Stereo 64
   false,  // AAC Stereo
   false,  // AAC Joint Stereo
   false,  // AAC Mono 128
   false   // AAC Mono 64
  };

bool zephyclassic_supported_rcv_algos[LPSwitcher::RcvAlgoLast]=
  {true,   // G.722
   true,   // MPEG L2
   true,   // MPEG L2 Mono 64
   true,   // MPEG L2 Half 64
   true,   // MPEG L3
   true,   // MPEG L3 Mono
   false,  // AAC-LD
   false,  // AAC-LD Mono 64 
   false,  // AAC
   false,  // AAC 64
   false   // Auto
  };

ZephyrClassic::ZephyrClassic(int id,QObject *parent)
  : LPSwitcher(id,LPSwitcher::TypeZephyrClassic,parent)
{
  zep_locked=false;
  zep_xmt_algo=LPSwitcher::XmtAlgoLast;
  zep_rcv_algo=LPSwitcher::RcvAlgoLast;
  zep_channel_rate=LPSwitcher::ChannelRateLast;
  zep_pending_channel_rate=LPSwitcher::ChannelRateLast;
  zep_sample_rate=0;
  zep_pending_sample_rate=0;
  for(int i=0;i<2;i++) {
    zep_line_state[i]=LPSwitcher::StateOffline;
    zep_dialed_string[i]="";
    zep_pending_string[i]="";
  }

  zep_device=new LPTTYDevice(this);
  zep_device->setSpeed(ZEPHYRCLASSIC_SPEED);
  zep_device->setWordLength(ZEPHYRCLASSIC_WORD_LENGTH);
  zep_device->setParity(ZEPHYRCLASSIC_PARITY);
  zep_device->setFlowControl(ZEPHYRCLASSIC_FLOW_CONTROL);
  connect(zep_device,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  zep_poll_timer=new QTimer(this);
  connect(zep_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
}


ZephyrClassic::~ZephyrClassic()
{
  delete zep_poll_timer;
  delete zep_device;
}


bool ZephyrClassic::open(const QString &device)
{
  zep_device->setDeviceName(device);
  if(!zep_device->open(QIODevice::ReadWrite)) {
    return false;
  }
  zep_device->write("login\r\n",7);
  zep_poll_timer->start(ZEPHYRCLASSIC_POLL_INTERVAL);

  return true;
}


void ZephyrClassic::connectToHost(const QString &hostname,
				  const QString &password,uint16_t port)
{
}


int ZephyrClassic::inputs()
{
  return ZEPHYRCLASSIC_INPUTS;
}


int ZephyrClassic::outputs()
{
  return ZEPHYRCLASSIC_OUTPUTS;
}


int ZephyrClassic::gpis()
{
  return ZEPHYRCLASSIC_GPIS;
}


int ZephyrClassic::gpos()
{
  return ZEPHYRCLASSIC_GPOS;
}


int ZephyrClassic::lines()
{
  return ZEPHYRCLASSIC_LINES;
}


bool ZephyrClassic::xmtSupports(LPSwitcher::XmtAlgo algo) const
{
  return zephyclassic_supported_xmt_algos[algo];
}


bool ZephyrClassic::rcvSupports(LPSwitcher::RcvAlgo algo) const
{
  return zephyclassic_supported_rcv_algos[algo];
}


LPSwitcher::XmtAlgo ZephyrClassic::xmtAlgorithm() const
{
  return zep_xmt_algo;
}


void ZephyrClassic::setXmtAlgorithm(LPSwitcher::XmtAlgo algo)
{
  QString cmd;

  switch(algo) {
  case LPSwitcher::XmtG722:
    cmd="txmode G722";
    break;

  case LPSwitcher::XmtMpegL2Mono64:
    cmd="txmode L2MO";
    break;

  case LPSwitcher::XmtMpegL2Mono128:
    cmd="txmode L2M128";
    break;

  case LPSwitcher::XmtMpegL2Stereo:
    cmd="txmode L2DU";
    break;

  case LPSwitcher::XmtMpegL2JointStereo:
    cmd="txmode L2JO";
    break;

  case LPSwitcher::XmtMpegL2Half64:
    break;

  case LPSwitcher::XmtMpegL3DualMono:
    cmd="txmode L3DU";
    break;

  case LPSwitcher::XmtMpegL3Stereo:
    cmd="txmode L3ST";
    break;

  case LPSwitcher::XmtMpegL3JointStereo:
    cmd="txmode L3JO";
    break;

  case LPSwitcher::XmtMpegL3Mono128:
  case LPSwitcher::XmtAacLdStereo:
  case LPSwitcher::XmtAacLdJointStereo:
  case LPSwitcher::XmtAacLdJointStereo64:
  case LPSwitcher::XmtAacLdMono128:
  case LPSwitcher::XmtAacLdMono64:
  case LPSwitcher::XmtAacJointStereo64:
  case LPSwitcher::XmtAacStereo:
  case LPSwitcher::XmtAacJointStereo:
  case LPSwitcher::XmtAacMono128:
  case LPSwitcher::XmtAacMono64:
  case LPSwitcher::XmtAlgoLast:
    break;
  }

  if(!cmd.isEmpty()) {
    zep_device->write((cmd+"\r\n").toAscii(),cmd.length()+2);
  }
}


LPSwitcher::RcvAlgo ZephyrClassic::rcvAlgorithm() const
{
  return zep_rcv_algo;
}


void ZephyrClassic::setRcvAlgorithm(LPSwitcher::RcvAlgo algo)
{
  QString cmd;

  switch(algo) {
  case LPSwitcher::RcvG722:
    cmd="rxmode G722";
    break;

  case LPSwitcher::RcvMpegL2:
  case LPSwitcher::RcvMpegL2Mono64:
    cmd="rxmode L2";
    break;

  case LPSwitcher::RcvMpegL3Mono:
    cmd="rxmode L3MO";
    break;

  case LPSwitcher::RcvMpegL3:
    cmd="rxmode L3ST";
    break;

  case LPSwitcher::RcvMpegL2Half64:
  case LPSwitcher::RcvAacLd:
  case LPSwitcher::RcvAacLdMono64:
  case LPSwitcher::RcvAac:
  case LPSwitcher::RcvAac64:
  case LPSwitcher::RcvAuto:
  case LPSwitcher::RcvAlgoLast:
    break;
  }

  if(!cmd.isEmpty()) {
    zep_device->write((cmd+"\r\n").toAscii(),cmd.length()+2);
  }
}


LPSwitcher::ChannelRate ZephyrClassic::channelRate() const
{
  return zep_channel_rate;
}


void ZephyrClassic::setChannelRate(LPSwitcher::ChannelRate rate)
{
  zep_pending_channel_rate=rate;
  SendRates();
}


int ZephyrClassic::sampleRate() const
{
  return zep_sample_rate;
}


void ZephyrClassic::setSampleRate(int rate)
{
  zep_pending_sample_rate=rate;
  SendRates();
}


int ZephyrClassic::crosspoint(int output)
{
  return 0;
}


void ZephyrClassic::setCrosspoint(int output,int input)
{
}


bool ZephyrClassic::gpiState(int gpi)
{
  return false;
}


void ZephyrClassic::pulseGpo(int gpo)
{
}


bool ZephyrClassic::silenceSense(int chan)
{
  return false;
}


void ZephyrClassic::dialLine(int line,const QString &str)
{
  QString msg=QString().sprintf("conn %d ",line+1)+str+"\r\n";
  zep_device->write(msg.toAscii(),msg.length());
  zep_pending_string[line]=str;
}


void ZephyrClassic::dropLine(int line)
{
  QString msg=QString().sprintf("disc %d ",line+1)+"\r\n";
  zep_device->write(msg.toAscii(),msg.length());
  zep_pending_string[line]="";
}


LPSwitcher::LineState ZephyrClassic::lineState(int line)
{
  return zep_line_state[line];
}


QString ZephyrClassic::dialedString(int line) const
{
  return zep_dialed_string[line];
}


bool ZephyrClassic::isLocked() const
{
  return zep_locked;
}


void ZephyrClassic::readyReadData()
{
  char data[1024];
  int n;

  while((n=zep_device->read(data,1024))>0) {
    for(int i=0;i<n;i++) {
      switch(data[i]) {
      case 13:
	ProcessStatus(zep_buffer);
	zep_buffer="";
	break;

      case 10:
	break;

      default:
	zep_buffer+=data[i];
	break;
      }
    }
  }
}


void ZephyrClassic::pollData()
{
  zep_device->write("stat\r\n",6);
}


void ZephyrClassic::ProcessStatus(const QString &str)
{
  QStringList f0=str.split(":");
  QString rhs;
  bool ok=false;

  //printf("ProcessStatus: %s\n",(const char *)str.toAscii());

  if(str.left(16).toLower()==">>calling number") {
    QStringList f1=str.right(str.length()-17).split(" ");
    if(f1.size()>=4) {
      int line=f1[3].toInt(&ok)-1;
      if(ok&&(line<2)) {
	zep_dialed_string[line]=f1[0].trimmed();
	emit dialedStringChanged(line,zep_dialed_string[line]);
      }
      else {
	syslog(LOG_WARNING,"zephyr classic: unrecognized string \"%s\"",
	       (const char *)str.toAscii());
      }
    }
  }

  if(f0.size()>=2) {
    for(int i=1;i<f0.size();i++) {
      rhs+=(f0[i]+":");
    }
    rhs=rhs.left(rhs.size()-1).trimmed();
    if(f0[0].toLower()=="line status 1 & 2..") {
      QStringList f1=rhs.split(" ",QString::SkipEmptyParts);
      if(f1.size()==2) {
	for(int i=0;i<2;i++) {
	  if(f1[i].contains(":")) {
	    SetState(i,LPSwitcher::StateConnected);
	    if(!zep_pending_string[i].isEmpty()) {
	      zep_dialed_string[i]=zep_pending_string[i];
	      zep_pending_string[i]="";
	      emit dialedStringChanged(i,zep_dialed_string[i]);
	    }
	  }
	  else {
	    if(f1[i].toLower().left(4)=="conn") {
	      SetState(i,LPSwitcher::StateProgressing);
	    }
	    else {
	      if(f1[i].toLower().left(4)=="disc") {
		SetState(i,LPSwitcher::StateDisconnecting);
	      }
	      else {
		if(f1[i].toLower()=="ready") {
		  SetState(i,LPSwitcher::StateIdle);
		}
		else {
		  SetState(i,LPSwitcher::StateOffline);
		}
		zep_dialed_string[i]="";
		emit dialedStringChanged(i,"");
	      }
	    }
	  }
	}
      }
    }
    if(f0[0].toLower()=="decoder status.....") {
      bool locked=f0[1].trimmed().toLower()=="locked";
      if(locked!=zep_locked) {
	zep_locked=locked;
	emit lockChanged(0,locked);
	emit lockChanged(1,locked);
      }
    }
    if(f0[0].toLower()=="transmitter mode...") {
      if(rhs.toLower()=="l3-dual") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL3DualMono);
      }
      if(rhs.toLower()=="l3-stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL3Stereo);
      }
      if(rhs.toLower()=="l3-jstereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL3JointStereo);
      }
      if(rhs.toLower()=="g.722") {
	SetXmtAlgorithm(LPSwitcher::XmtG722);
      }
      if(rhs.toLower()=="l2-mono") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Mono64);
      }
      if(rhs.toLower()=="l2-mono128") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Mono128);
      }
      if(rhs.toLower()=="l2-jstereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2JointStereo);
      }
      if(rhs.toLower()=="l2-stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Stereo);
      }
      if(rhs.toLower()=="l2-half/24") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Half64);
      }
    }
    if(f0[0].toLower()=="receiver mode......") {
      if(rhs.toLower()=="l3-mono") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL3Mono);
      }
      if(rhs.toLower()=="l3-stereo") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL3);
      }
      if(rhs.toLower()=="g.722") {
	SetRcvAlgorithm(LPSwitcher::RcvG722);
      }
      if(rhs.toLower()=="l2-half/24") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL2Half64);
      }
      if(rhs.toLower()=="l2") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL2);
      }
    }
    if(f0[0].toLower()=="rates..............") {
      QStringList f1=rhs.split("/");
      if(f1.size()==2) {
	if(f1[0].toLower()=="56kbps") {
	  SetChannelRate(LPSwitcher::Channel56Kbit);
	}
	if(f1[0].toLower()=="64kbps") {
	  SetChannelRate(LPSwitcher::Channel64Kbit);
	}
	if(f1[1].toLower()=="32khz") {
	  SetSampleRate(32000);
	}
	if(f1[1].toLower()=="48khz") {
	  SetSampleRate(48000);
	}
      }
    }
  }
}


void ZephyrClassic::SetState(int line,LPSwitcher::LineState state)
{
  if(state!=zep_line_state[line]) {
    zep_line_state[line]=state;
    emit lineStateChanged(line,state);
  }
}


void ZephyrClassic::SetXmtAlgorithm(LPSwitcher::XmtAlgo algo)
{
  if(algo!=zep_xmt_algo) {
    zep_xmt_algo=algo;
    emit xmtAlgorithmChanged(algo);
  }
}


void ZephyrClassic::SetRcvAlgorithm(LPSwitcher::RcvAlgo algo)
{
  if(algo!=zep_rcv_algo) {
    zep_rcv_algo=algo;
    emit rcvAlgorithmChanged(algo);
  }
}


void ZephyrClassic::SetChannelRate(LPSwitcher::ChannelRate rate)
{
  if(rate!=zep_channel_rate) {
    zep_channel_rate=rate;
    zep_pending_channel_rate=LPSwitcher::ChannelRateLast;
    emit channelRateChanged(rate);
  }
}


void ZephyrClassic::SetSampleRate(int rate)
{
  if(rate!=zep_sample_rate) {
    zep_sample_rate=rate;
    zep_pending_sample_rate=0;
    emit sampleRateChanged(rate);
  }
}


void ZephyrClassic::SendRates()
{
  QString cmd;
  LPSwitcher::ChannelRate crate=zep_pending_channel_rate;
  if(crate==LPSwitcher::ChannelRateLast) {
    crate=zep_channel_rate;
  }
  int srate=zep_pending_sample_rate;
  if(srate==0) {
    srate=zep_sample_rate;
  }

  switch(crate) {
  case LPSwitcher::Channel56Kbit:
    cmd=QString().sprintf("bitrate 56/%d",srate/1000);
    break;

  case LPSwitcher::Channel64Kbit:
    cmd=QString().sprintf("bitrate 64/%d",srate/1000);
    break;

  case LPSwitcher::ChannelRateLast:
    break;
  }

  if(!cmd.isEmpty()) {
    zep_device->write((cmd+"\r\n").toAscii(),cmd.length()+2);
  }
}
