// switcher_zephyrxtreme.cpp
//
// LPSwitcher implementation for the Telos Zephyr Xtreme
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: switcher_zephyrxtreme.cpp,v 1.2 2013/09/09 18:21:55 cvs Exp $
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

#include "switcher_zephyrxtreme.h"

bool zephyxtreme_supported_xmt_algos[LPSwitcher::XmtAlgoLast]=
  {true,   // G.722
   true,   // MPEG L2 Stereo
   true,   // MPEG L2 Joint Stereo
   true,   // MPEG L2 Mono 128
   true,   // MPEG L2 Mono 64
   true,   // MPEG L2 Half 64
   true,   // MPEG L3 Stereo
   true,   // MPEG L3 Joint Stereo
   true,   // MPEG L3 Mono 128
   true,   // MPEG L3 Dual/Mono
   true,   // AAC-LD Stereo
   true,   // AAC-LD Joint Stereo
   true,   // AAC-LD Joint Stereo 64
   true,   // AAC-LD Mono 128
   true,   // AAC-LD Mono 64
   true,   // AAC Joint Stereo 64
   true,   // AAC Stereo
   true,   // AAC Joint Stereo
   true,   // AAC Mono 128
   true    // AAC Mono 64
  };

bool zephyxtreme_supported_rcv_algos[LPSwitcher::RcvAlgoLast]=
  {true,   // G.722
   true,   // MPEG L2
   true,   // MPEG L2 Mono 64
   false,  // MPEG L2 Half 64
   true,   // MPEG L3
   true,   // MPEG L3 Mono
   true,   // AAC-LD
   true,   // AAC-LD Mono 64 
   true,   // AAC
   true,   // AAC 64
   true    // Auto
  };

ZephyrXtreme::ZephyrXtreme(int id,QObject *parent)
  : LPSwitcher(id,LPSwitcher::TypeZephyrXtreme,parent)
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
  zep_device->setSpeed(ZEPHYRXTREME_SPEED);
  zep_device->setWordLength(ZEPHYRXTREME_WORD_LENGTH);
  zep_device->setParity(ZEPHYRXTREME_PARITY);
  zep_device->setFlowControl(ZEPHYRXTREME_FLOW_CONTROL);
  connect(zep_device,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  zep_socket=new QTcpSocket(this);
  connect(zep_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(zep_socket,SIGNAL(disconnected()),this,SLOT(disconnectedData()));
  connect(zep_socket,SIGNAL(readyRead()),this,SLOT(socketReadyRead()));

  zep_poll_timer=new QTimer(this);
  connect(zep_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
}


ZephyrXtreme::~ZephyrXtreme()
{
  delete zep_poll_timer;
  delete zep_device;
  delete zep_socket;
}


bool ZephyrXtreme::open(const QString &device)
{
  zep_device->setDeviceName(device);
  if(!zep_device->open(QIODevice::ReadWrite)) {
    return false;
  }
  zep_device->write("login\r\n",7);
  zep_poll_timer->start(ZEPHYRXTREME_POLL_INTERVAL);

  return true;
}


void ZephyrXtreme::connectToHost(const QString &hostname,
				 const QString &password,uint16_t port)
{
  if(port==0) {
    port=ZEPHYRXTREME_DEFAULT_TCP_PORT;
  }
  zep_hostname=hostname;
  zep_password=password;
  if(password.isEmpty()) {
    zep_password=ZEPHYRXTREME_DEFAULT_PASSWORD;
  }
  zep_port=port;

  zep_socket->connectToHost(hostname,port);
}


int ZephyrXtreme::inputs()
{
  return ZEPHYRXTREME_INPUTS;
}


int ZephyrXtreme::outputs()
{
  return ZEPHYRXTREME_OUTPUTS;
}


int ZephyrXtreme::gpis()
{
  return ZEPHYRXTREME_GPIS;
}


int ZephyrXtreme::gpos()
{
  return ZEPHYRXTREME_GPOS;
}


int ZephyrXtreme::lines()
{
  return ZEPHYRXTREME_LINES;
}


bool ZephyrXtreme::xmtSupports(LPSwitcher::XmtAlgo algo) const
{
  return zephyxtreme_supported_xmt_algos[algo];
}


bool ZephyrXtreme::rcvSupports(LPSwitcher::RcvAlgo algo) const
{
  return zephyxtreme_supported_rcv_algos[algo];
}


LPSwitcher::XmtAlgo ZephyrXtreme::xmtAlgorithm() const
{
  return zep_xmt_algo;
}


void ZephyrXtreme::setXmtAlgorithm(LPSwitcher::XmtAlgo algo)
{
  QString cmd;

  switch(algo) {
  case LPSwitcher::XmtG722:
    cmd="encomode G722";
    break;

  case LPSwitcher::XmtMpegL2Mono64:
    cmd="encomode L2mono64";
    break;

  case LPSwitcher::XmtMpegL2Mono128:
    cmd="encomode L2mono128";
    break;

  case LPSwitcher::XmtMpegL2Stereo:
    cmd="encomode L2stereo";
    break;

  case LPSwitcher::XmtMpegL2JointStereo:
    cmd="encomode L2joint";
    break;

  case LPSwitcher::XmtMpegL2Half64:
    cmd="encomode L2half64";
    break;

  case LPSwitcher::XmtMpegL3DualMono:
    cmd="encomode L3dual";
    break;

  case LPSwitcher::XmtMpegL3Stereo:
    cmd="encomode L3stereo";
    break;

  case LPSwitcher::XmtMpegL3JointStereo:
    cmd="encomode L3joint";
    break;

  case LPSwitcher::XmtMpegL3Mono128:
    cmd="encomode L3mono128";
    break;

  case LPSwitcher::XmtAacLdStereo:
    cmd="encomode AACLDstereo";
    break;

  case LPSwitcher::XmtAacLdJointStereo:
    cmd="encomode AACLDjoint";
    break;

  case LPSwitcher::XmtAacLdJointStereo64:
    cmd="encomode AACLDjoint64";
    break;

  case LPSwitcher::XmtAacLdMono128:
    cmd="encomode AACLDmono128";
    break;

  case LPSwitcher::XmtAacLdMono64:
    cmd="encomode AACLDmono64";
    break;

  case LPSwitcher::XmtAacJointStereo64:
    cmd="encomode AACjoint64";
    break;

  case LPSwitcher::XmtAacStereo:
    cmd="encomode AACstereo";
    break;

  case LPSwitcher::XmtAacJointStereo:
    cmd="encomode AACjoint";
    break;

  case LPSwitcher::XmtAacMono128:
    cmd="encomode AACmono128";
    break;

  case LPSwitcher::XmtAacMono64:
    cmd="encomode AACmono64";
    break;

  case LPSwitcher::XmtAlgoLast:
    break;
  }
  WriteToDevice(cmd);
}


LPSwitcher::RcvAlgo ZephyrXtreme::rcvAlgorithm() const
{
  return zep_rcv_algo;
}


void ZephyrXtreme::setRcvAlgorithm(LPSwitcher::RcvAlgo algo)
{
  QString cmd;

  switch(algo) {
  case LPSwitcher::RcvG722:
    cmd="decomode G722";
    break;

  case LPSwitcher::RcvMpegL2:
  case LPSwitcher::RcvMpegL2Half64:
    cmd="decomode L2";
    break;

  case LPSwitcher::RcvMpegL2Mono64:
    cmd="decomode L2M64";
    break;

  case LPSwitcher::RcvMpegL3Mono:
    cmd="decomode L3D/M";
    break;

  case LPSwitcher::RcvMpegL3:
    cmd="decomode L3";
    break;

  case LPSwitcher::RcvAacLd:
    cmd="decomode AACLD";
    break;

  case LPSwitcher::RcvAacLdMono64:
    cmd="decomode AACLDM64";
    break;

  case LPSwitcher::RcvAac:
    cmd="decomode AAC";
    break;

  case LPSwitcher::RcvAac64:
    cmd="decomode AAC64";
    break;

  case LPSwitcher::RcvAuto:
    cmd="decomode Auto";
    break;

  case LPSwitcher::RcvAlgoLast:
    break;
  }
  WriteToDevice(cmd);
}


LPSwitcher::ChannelRate ZephyrXtreme::channelRate() const
{
  return zep_channel_rate;
}


void ZephyrXtreme::setChannelRate(LPSwitcher::ChannelRate rate)
{
  QString cmd;

  switch(rate) {
  case LPSwitcher::Channel56Kbit:
    cmd="bitrate 56";
    break;

  case LPSwitcher::Channel64Kbit:
    cmd="bitrate 64";
    break;

  case LPSwitcher::ChannelRateLast:
    break;    
  }
  WriteToDevice(cmd);
}


int ZephyrXtreme::sampleRate() const
{
  return zep_sample_rate;
}


void ZephyrXtreme::setSampleRate(int rate)
{
  QString cmd;

  switch(rate) {
  case 32000:
    cmd="sfq 32";
    break;

  case 44100:
    cmd="sfq 44";
    break;

  case 48000:
    cmd="sfq 48";
    break;
  }
  WriteToDevice(cmd);
}


int ZephyrXtreme::crosspoint(int output)
{
  return 0;
}


void ZephyrXtreme::setCrosspoint(int output,int input)
{
}


bool ZephyrXtreme::gpiState(int gpi)
{
  return false;
}


void ZephyrXtreme::pulseGpo(int gpo)
{
}


bool ZephyrXtreme::silenceSense(int chan)
{
  return false;
}


void ZephyrXtreme::dialLine(int line,const QString &str)
{
  QString msg=QString().sprintf("isdncall %d ",line)+str+" data";
  WriteToDevice(msg);
  zep_pending_string[line]=str;
}


void ZephyrXtreme::dropLine(int line)
{
  QString msg=QString().sprintf("isdndisc %d",line);
  WriteToDevice(msg);
  zep_pending_string[line]="";
}


LPSwitcher::LineState ZephyrXtreme::lineState(int line)
{
  return zep_line_state[line];
}


QString ZephyrXtreme::dialedString(int line) const
{
  return zep_dialed_string[line];
}


bool ZephyrXtreme::isLocked() const
{
  return zep_locked;
}


void ZephyrXtreme::connectedData()
{
  QString msg="login "+zep_password+"\r\n";

  zep_socket->write(msg.toAscii(),msg.length());
  zep_poll_timer->start(ZEPHYRXTREME_POLL_INTERVAL);
}


void ZephyrXtreme::disconnectedData()
{
  zep_poll_timer->stop();
}


void ZephyrXtreme::socketReadyRead()
{
  char data[1024];
  int n;

  while((n=zep_socket->read(data,1024))>0) {
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


void ZephyrXtreme::readyReadData()
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


void ZephyrXtreme::pollData()
{
  WriteToDevice("zstat");
}


void ZephyrXtreme::ProcessStatus(const QString &str)
{
  QStringList f0=str.split(":");
  QString rhs;
  bool ok=false;
  int rate=0;

  //  printf("ProcessStatus: %s\n",(const char *)str.toAscii());

  if(str.left(16).toLower()==">>calling number") {
    QStringList f1=str.right(str.length()-17).split(" ");
    if(f1.size()>=4) {
      int line=f1[3].toInt(&ok)-1;
      if(ok&&(line<2)) {
	zep_dialed_string[line]=f1[0].trimmed();
	emit dialedStringChanged(line,zep_dialed_string[line]);
      }
      else {
	syslog(LOG_WARNING,"zephyr xtreme: unrecognized string \"%s\"",
	       (const char *)str.toAscii());
      }
    }
  }

  if(f0.size()>=2) {
    for(int i=1;i<f0.size();i++) {
      rhs+=(f0[i]+":");
    }
    rhs=rhs.left(rhs.size()-1).trimmed();
    if(f0[0].toLower()=="lines status 1 & 2............") {
      QStringList f1=rhs.split(" ",QString::SkipEmptyParts);
      if(f1.size()==2) {
	for(int i=0;i<2;i++) {
	  if(f1[i].toLower().left(4)=="conn") {
	    SetState(i,LPSwitcher::StateConnected);
	    if(!zep_pending_string[i].isEmpty()) {
	      zep_dialed_string[i]=zep_pending_string[i];
	      zep_pending_string[i]="";
	      emit dialedStringChanged(i,zep_dialed_string[i]);
	    }
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
    if(f0[0].toLower()=="decoder status................") {
      bool locked=f0[1].trimmed().toLower()=="locked";
      if(locked!=zep_locked) {
	zep_locked=locked;
	emit lockChanged(0,locked);
	printf("lockChanged(0,%d)\n",locked);
	emit lockChanged(1,locked);
	printf("lockChanged(1,%d)\n",locked);
      }
    }
    if(f0[0].toLower()=="transmitter mode..............") {
      if(rhs.toLower()=="l3 dual/mono") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL3DualMono);
      }
      if(rhs.toLower()=="l3 stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL3Stereo);
      }
      if(rhs.toLower()=="l3 j-stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL3JointStereo);
      }
      if(rhs.toLower()=="l3 mono 128") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL3Mono128);
      }
      if(rhs.toLower()=="g.722") {
	SetXmtAlgorithm(LPSwitcher::XmtG722);
      }
      if(rhs.toLower()=="l2 mono 64") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Mono64);
      }
      if(rhs.toLower()=="l2 mono 128") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Mono128);
      }
      if(rhs.toLower()=="l2 j-stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2JointStereo);
      }
      if(rhs.toLower()=="l2 stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Stereo);
      }
      if(rhs.toLower()=="l2 half 64") {
	SetXmtAlgorithm(LPSwitcher::XmtMpegL2Half64);
      }
      if(rhs.toLower()=="aac-ld mono 64") {
	SetXmtAlgorithm(LPSwitcher::XmtAacLdMono64);
      }
      if(rhs.toLower()=="aac-ld stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtAacLdStereo);
      }
      if(rhs.toLower()=="aac-ld j-stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtAacLdJointStereo);
      }
      if(rhs.toLower()=="aac j-stereo 64") {
	SetXmtAlgorithm(LPSwitcher::XmtAacLdJointStereo64);
      }
      if(rhs.toLower()=="aac-ld mono 128") {
	SetXmtAlgorithm(LPSwitcher::XmtAacLdMono128);
      }
      if(rhs.toLower()=="aac-ld mono 64") {
	SetXmtAlgorithm(LPSwitcher::XmtAacLdMono64);
      }
      if(rhs.toLower()=="aac j-stereo 64") {
	SetXmtAlgorithm(LPSwitcher::XmtAacJointStereo64);
      }
      if(rhs.toLower()=="aac stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtAacStereo);
      }
      if(rhs.toLower()=="aac j-stereo") {
	SetXmtAlgorithm(LPSwitcher::XmtAacJointStereo);
      }
      if(rhs.toLower()=="aac mono 128") {
	SetXmtAlgorithm(LPSwitcher::XmtAacMono128);
      }
      if(rhs.toLower()=="aac mono 64") {
	SetXmtAlgorithm(LPSwitcher::XmtAacMono64);
      }
    }
    if(f0[0].toLower()=="receiver mode.................") {
      if(rhs.toLower()=="l3 dual/mono") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL3Mono);
      }
      if(rhs.toLower()=="l3") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL3);
      }
      if(rhs.toLower()=="g.722") {
	SetRcvAlgorithm(LPSwitcher::RcvG722);
      }
      if(rhs.toLower()=="l2") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL2);
      }
      if(rhs.toLower()=="l2 mono 64") {
	SetRcvAlgorithm(LPSwitcher::RcvMpegL2Mono64);
      }
      if(rhs.toLower()=="aac-ld") {
	SetRcvAlgorithm(LPSwitcher::RcvAacLd);
      }
      if(rhs.toLower()=="aac-ld mono 64") {
	SetRcvAlgorithm(LPSwitcher::RcvAacLdMono64);
      }
      if(rhs.toLower()=="aac") {
	SetRcvAlgorithm(LPSwitcher::RcvAac);
      }
      if(rhs.toLower()=="aac 64") {
	SetRcvAlgorithm(LPSwitcher::RcvAac64);
      }
      if(rhs.toLower()=="auto") {
	SetRcvAlgorithm(LPSwitcher::RcvAuto);
      }
    }
    if(f0[0].toLower()=="rates.........................") {
      rate=rhs.toInt(&ok);
      if(ok) {
	SetSampleRate(rate);
      }
    }
    if(f0[0].toLower()=="bit rates.....................") {
      QStringList f1=rhs.split(" ");
      rate=f1[0].toInt(&ok);
      if(ok) {
	switch(rate) {
	case 56:
	  SetChannelRate(LPSwitcher::Channel56Kbit);
	  break;

	case 64:
	  SetChannelRate(LPSwitcher::Channel64Kbit);
	  break;
	}
      }
    }
  }
}


void ZephyrXtreme::SetState(int line,LPSwitcher::LineState state)
{
  if(state!=zep_line_state[line]) {
    zep_line_state[line]=state;
    emit lineStateChanged(line,state);
  }
}


void ZephyrXtreme::SetXmtAlgorithm(LPSwitcher::XmtAlgo algo)
{
  if(algo!=zep_xmt_algo) {
    zep_xmt_algo=algo;
    emit xmtAlgorithmChanged(algo);
  }
}


void ZephyrXtreme::SetRcvAlgorithm(LPSwitcher::RcvAlgo algo)
{
  if(algo!=zep_rcv_algo) {
    zep_rcv_algo=algo;
    emit rcvAlgorithmChanged(algo);
  }
}


void ZephyrXtreme::SetChannelRate(LPSwitcher::ChannelRate rate)
{
  if(rate!=zep_channel_rate) {
    zep_channel_rate=rate;
    zep_pending_channel_rate=LPSwitcher::ChannelRateLast;
    emit channelRateChanged(rate);
  }
}


void ZephyrXtreme::SetSampleRate(int rate)
{
  if(rate!=zep_sample_rate) {
    zep_sample_rate=rate;
    zep_pending_sample_rate=0;
    emit sampleRateChanged(rate);
  }
}


void ZephyrXtreme::SendRates()
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
  WriteToDevice(cmd);
}


void ZephyrXtreme::WriteToDevice(const QString &cmd)
{
  if(!cmd.isEmpty()) {
    if(zep_hostname.isEmpty()) {
      zep_device->write((cmd+"\r\n").toAscii(),cmd.length()+2);
    }
    else {
      zep_socket->write((cmd+"\r\n").toAscii(),cmd.length()+2);
    }
  }
}
