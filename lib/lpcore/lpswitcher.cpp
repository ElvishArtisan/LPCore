// lpswitcher.cpp
//
// Abstract base class for switcher devices in LPCore
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpswitcher.cpp,v 1.6 2013/09/09 16:42:56 cvs Exp $
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

#include "lpswitcher.h"

LPSwitcher::LPSwitcher(int id,LPSwitcher::Type type,QObject *parent)
  : QObject(parent)
{
  switcher_id=id;
  switcher_type=type;
}


int LPSwitcher::id() const
{
  return switcher_id;
}


LPSwitcher::Type LPSwitcher::type() const
{
  return switcher_type;
}


bool LPSwitcher::open(const QString &device)
{
  return false;
}


void LPSwitcher::connectToHost(const QString &hostname,const QString &password,
			       uint16_t port)
{
}


int LPSwitcher::inputs()
{
  return 0;
}


int LPSwitcher::outputs()
{
  return 0;
}


int LPSwitcher::gpis()
{
  return 0;
}


int LPSwitcher::gpos()
{
  return 0;
}


int LPSwitcher::lines()
{
  return 0;
}


bool LPSwitcher::xmtSupports(LPSwitcher::XmtAlgo algo) const
{
  return false;
}


bool LPSwitcher::rcvSupports(LPSwitcher::RcvAlgo algo) const
{
  return false;
}


LPSwitcher::XmtAlgo LPSwitcher::xmtAlgorithm() const
{
  return LPSwitcher::XmtG722;
}


void LPSwitcher::setXmtAlgorithm(LPSwitcher::XmtAlgo algo)
{
}


LPSwitcher::RcvAlgo LPSwitcher::rcvAlgorithm() const
{
  return LPSwitcher::RcvG722;
}


void LPSwitcher::setRcvAlgorithm(LPSwitcher::RcvAlgo algo)
{
}


LPSwitcher::ChannelRate LPSwitcher::channelRate() const
{
  return LPSwitcher::Channel64Kbit;
}


void LPSwitcher::setChannelRate(LPSwitcher::ChannelRate rate)
{
}


int LPSwitcher::sampleRate() const
{
  return 32000;
}


void LPSwitcher::setSampleRate(int rate)
{
}


int LPSwitcher::crosspoint(int output)
{
  return 0;
}


void LPSwitcher::setCrosspoint(int output,int input)
{
}


bool LPSwitcher::gpiState(int gpi)
{
  return false;
}


void LPSwitcher::pulseGpo(int gpo)
{
}


bool LPSwitcher::silenceSense(int chan)
{
  return false;
}


void LPSwitcher::dialLine(int line,const QString &str)
{
}


void LPSwitcher::dropLine(int line)
{
}


LPSwitcher::LineState LPSwitcher::lineState(int line)
{
  return LPSwitcher::StateOffline;
}


QString LPSwitcher::dialedString(int line) const
{
  return QString();
}


bool LPSwitcher::isLocked() const
{
  return false;
}


QString LPSwitcher::typeString(LPSwitcher::Type type)
{
  QString ret=tr("Unknown");

  switch(type) {
  case LPSwitcher::TypeAm16:
    ret=tr("360 Systems AM16");
    break;

  case LPSwitcher::TypeBtSs82:
    ret=tr("BroadcastTools SS8.2");
    break;

  case LPSwitcher::TypeGpio:
    ret=tr("GPIO Card");
    break;

  case LPSwitcher::TypeZephyrClassic:
    ret=tr("Telos Zephyr Classic");
    break;

  case LPSwitcher::TypeZephyrXtreme:
    ret=tr("Telos Zephyr Xtreme");
    break;

  case LPSwitcher::TypeLast:
    break;
  }

  return ret;
}


QString LPSwitcher::lineStateString(LPSwitcher::LineState state)
{
  QString ret=tr("Unknown");

  switch(state) {
  case LPSwitcher::StateOffline:
    ret=tr("Offline");
    break;

  case LPSwitcher::StateIdle:
    ret=tr("Idle");
    break;

  case LPSwitcher::StateProgressing:
    ret=tr("Progressing");
    break;

  case LPSwitcher::StateConnected:
    ret=tr("Connected");
    break;

  case LPSwitcher::StateDisconnecting:
    ret=tr("Disconnecting");
    break;
  }

  return ret;
}


QString LPSwitcher::xmtAlgoString(LPSwitcher::XmtAlgo algo)
{
  QString ret=tr("Unknown");

  switch(algo) {
  case LPSwitcher::XmtG722:
    ret=tr("G.722");
    break;

  case LPSwitcher::XmtMpegL2Mono128:
    ret=tr("MPEG Layer 2 128 kb Mono");
    break;

  case LPSwitcher::XmtMpegL2Mono64:
    ret=tr("MPEG Layer 2 64 kb Mono");
    break;

  case LPSwitcher::XmtMpegL2Stereo:
    ret=tr("MPEG Layer 2 Stereo");
    break;

  case LPSwitcher::XmtMpegL2JointStereo:
    ret=tr("MPEG Layer 2 Joint Stereo");
    break;

  case LPSwitcher::XmtMpegL2Half64:
    ret=tr("MPEG Layer 2 Mono Half 64");
    break;

  case LPSwitcher::XmtMpegL3DualMono:
    ret=tr("MPEG Layer 3 Dual/Mono");
    break;

  case LPSwitcher::XmtMpegL3Stereo:
    ret=tr("MPEG Layer 3 Stereo");
    break;

  case LPSwitcher::XmtMpegL3JointStereo:
    ret=tr("MPEG Layer 3 Joint Stereo");
    break;

  case LPSwitcher::XmtMpegL3Mono128:
    ret=tr("MPEG Layer 3 Mono 128");
    break;

  case LPSwitcher::XmtAacLdMono64:
    ret=tr("AAC-LD Mono 64");
    break;

  case LPSwitcher::XmtAacLdStereo:
    ret=tr("AAC-LD Stereo");
    break;

  case LPSwitcher::XmtAacLdJointStereo:
    ret=tr("AAC-LD Joint Stereo");
    break;

  case LPSwitcher::XmtAacLdJointStereo64:
    ret=tr("AAC-LD Joint Stereo 64");
    break;

  case LPSwitcher::XmtAacLdMono128:
    ret=tr("AAC-LD Mono 128");
    break;

  case LPSwitcher::XmtAacJointStereo64:
    ret=tr("AAC Joint Stereo 64");
    break;

  case LPSwitcher::XmtAacStereo:
    ret=tr("AAC Stereo");
    break;

  case LPSwitcher::XmtAacJointStereo:
    ret=tr("AAC Joint Stereo");
    break;

  case LPSwitcher::XmtAacMono128:
    ret=tr("AAC Mono 128");
    break;

  case LPSwitcher::XmtAacMono64:
    ret=tr("AAC Mono 64");
    break;

  case LPSwitcher::XmtAlgoLast:
    break;
  }

  return ret;
}


QString LPSwitcher::rcvAlgoString(LPSwitcher::RcvAlgo algo)
{
  QString ret=tr("Unknown");

  switch(algo) {
  case LPSwitcher::RcvG722:
    ret=tr("G.722");
    break;

  case LPSwitcher::RcvMpegL2:
    ret=tr("MPEG L2");
    break;

  case LPSwitcher::RcvMpegL2Mono64:
    ret=tr("MPEG L2 Mono 64");
    break;

  case LPSwitcher::RcvMpegL2Half64:
    ret=tr("MPEG L2 Half 64");
    break;

  case LPSwitcher::RcvMpegL3:
    ret=tr("MPEG L3");
    break;

  case LPSwitcher::RcvMpegL3Mono:
    ret=tr("MPEG L3 Mono");
    break;

  case LPSwitcher::RcvAacLd:
    ret=tr("AAC-LD");
    break;

  case LPSwitcher::RcvAacLdMono64:
    ret=tr("AAC-LD Mono 64");
    break;

  case LPSwitcher::RcvAac:
    ret=tr("AAC");
    break;

  case LPSwitcher::RcvAac64:
    ret=tr("AAC 64");
    break;

  case LPSwitcher::RcvAuto:
    ret=tr("Automatic");
    break;

  case LPSwitcher::RcvAlgoLast:
    break;
  }

  return ret;
}


QString LPSwitcher::channelRateString(LPSwitcher::ChannelRate rate)
{
  QString ret=tr("Unknown");

  switch(rate) {
  case LPSwitcher::Channel56Kbit:
    ret=tr("56 kbit/sec");
    break;

  case LPSwitcher::Channel64Kbit:
    ret=tr("64 kbit/sec");
    break;

  case LPSwitcher::ChannelRateLast:
    break;
  }

  return ret;
}
