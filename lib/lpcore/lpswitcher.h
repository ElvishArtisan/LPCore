// lpswitcher.h
//
// Abstract base class for switcher devices in LPCore
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpswitcher.h,v 1.6 2013/09/09 16:42:56 cvs Exp $
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

#ifndef LPSWITCHER_H
#define LPSWITCHER_H

#include <stdint.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <lpcore/lpttydevice.h>

class LPSwitcher : public QObject
{
 Q_OBJECT;
 public:
 enum Type {TypeBtSs82=0,TypeAm16=1,TypeZephyrClassic=2,TypeZephyrXtreme=3,
	    TypeGpio=4,TypeDummy=5,TypeSas16000=6,TypeLast=7};
  enum LineState {StateOffline=0,StateIdle=1,StateProgressing=2,
		  StateConnected=3,StateDisconnecting=4};
  enum XmtAlgo {XmtG722=0,XmtMpegL2Stereo=1,XmtMpegL2JointStereo=2,
		XmtMpegL2Mono128=3,XmtMpegL2Mono64=4,XmtMpegL2Half64=5,
		XmtMpegL3Stereo=6,XmtMpegL3JointStereo=7,XmtMpegL3Mono128=8,
		XmtMpegL3DualMono=9,XmtAacLdStereo=10,XmtAacLdJointStereo=11,
		XmtAacLdJointStereo64=12,
		XmtAacLdMono128=13,XmtAacLdMono64=14,XmtAacJointStereo64=15,
		XmtAacStereo=16,XmtAacJointStereo=17,XmtAacMono128=18,
		XmtAacMono64=19,XmtAlgoLast=20};

  enum RcvAlgo {RcvG722=0,RcvMpegL2=1,RcvMpegL2Mono64=2,RcvMpegL2Half64=3,
		RcvMpegL3=4,RcvMpegL3Mono=5,RcvAacLd=6,RcvAacLdMono64=7,
		RcvAac=8,RcvAac64=9,RcvAuto=10,RcvAlgoLast=11};

  enum ChannelRate {Channel56Kbit=0,Channel64Kbit=1,ChannelRateLast=2};
  LPSwitcher(int id,LPSwitcher::Type type,QObject *parent=0);
  int id() const;
  LPSwitcher::Type type() const;
  virtual bool open(const QString &device)=0;
  virtual void connectToHost(const QString &hostname,const QString &password="",
			     uint16_t port=0)=0;
  virtual int inputs()=0;
  virtual int outputs()=0;
  virtual int gpis()=0;
  virtual int gpos()=0;
  virtual int lines()=0;
  virtual bool xmtSupports(LPSwitcher::XmtAlgo algo) const;
  virtual bool rcvSupports(LPSwitcher::RcvAlgo algo) const;
  virtual LPSwitcher::XmtAlgo xmtAlgorithm() const;
  virtual void setXmtAlgorithm(LPSwitcher::XmtAlgo algo);
  virtual LPSwitcher::RcvAlgo rcvAlgorithm() const;
  virtual void setRcvAlgorithm(LPSwitcher::RcvAlgo algo);
  virtual LPSwitcher::ChannelRate channelRate() const;
  virtual void setChannelRate(LPSwitcher::ChannelRate rate);
  virtual int sampleRate() const;
  virtual void setSampleRate(int rate);
  virtual int crosspoint(int output);
  virtual void setCrosspoint(int output,int input);
  virtual bool gpiState(int gpi);
  virtual void pulseGpo(int gpo);
  virtual bool silenceSense(int chan);
  virtual void dialLine(int line,const QString &str);
  virtual void dropLine(int line);
  virtual LPSwitcher::LineState lineState(int line);
  virtual QString dialedString(int line) const;
  virtual bool isLocked() const;
  static QString typeString(LPSwitcher::Type type);
  static QString lineStateString(LPSwitcher::LineState state);
  static QString xmtAlgoString(LPSwitcher::XmtAlgo algo);
  static QString rcvAlgoString(LPSwitcher::RcvAlgo algo);
  static QString channelRateString(LPSwitcher::ChannelRate rate);

 signals:
  void crosspointChanged(int id,int output,int input);
  void gpiChanged(int id,int gpi,bool state);
  void silenceSenseChanged(int id,int chan,bool state);
  void lineStateChanged(int line,LPSwitcher::LineState state);
  void dialedStringChanged(int line,const QString &str);
  void lockChanged(int line,bool state);
  void xmtAlgorithmChanged(LPSwitcher::XmtAlgo algo);
  void rcvAlgorithmChanged(LPSwitcher::RcvAlgo algo);
  void channelRateChanged(LPSwitcher::ChannelRate rate);
  void sampleRateChanged(int rate);

 private:
  int switcher_id;
  LPSwitcher::Type switcher_type;
};


#endif  // LPSWITCHER_H
