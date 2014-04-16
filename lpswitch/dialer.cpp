// dialer.cpp
//
// Dialer Widget
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: dialer.cpp,v 1.3 2013/09/07 00:30:55 cvs Exp $
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

#include <QtCore/QSignalMapper>
#include <QtGui/QPainter>

#include "dialer.h"

Dialer::Dialer(LPSwitcher *switcher,QWidget *parent)
  : QWidget(parent)
{
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  dial_switcher=switcher;
  connect(dial_switcher,SIGNAL(lineStateChanged(int,LPSwitcher::LineState)),
	  this,SLOT(lineStateChangedData(int,LPSwitcher::LineState)));
  connect(dial_switcher,SIGNAL(lockChanged(int,bool)),
	  this,SLOT(lockChangedData(int,bool)));
  connect(dial_switcher,SIGNAL(dialedStringChanged(int,const QString &)),
	  this,SLOT(dialedStringChangedData(int,const QString &)));
  connect(dial_switcher,SIGNAL(xmtAlgorithmChanged(LPSwitcher::XmtAlgo)),
	  this,SLOT(xmtAlgorithmChangedData(LPSwitcher::XmtAlgo)));
  connect(dial_switcher,SIGNAL(rcvAlgorithmChanged(LPSwitcher::RcvAlgo)),
	  this,SLOT(rcvAlgorithmChangedData(LPSwitcher::RcvAlgo)));
  connect(dial_switcher,SIGNAL(channelRateChanged(LPSwitcher::ChannelRate)),
	  this,SLOT(channelRateChangedData(LPSwitcher::ChannelRate)));
  connect(dial_switcher,SIGNAL(sampleRateChanged(int)),
	  this,SLOT(sampleRateChangedData(int)));

  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(dialData(int)));
  for(int i=0;i<dial_switcher->lines();i++) {
    dial_state_labels.
      push_back(new QLabel(LPSwitcher::lineStateString(dial_switcher->
						       lineState(i)),this));
    dial_dialed_string_labels.
      push_back(new QLabel(dial_switcher->dialedString(i),this));
    dial_dial_buttons.push_back(new QPushButton(this));
    connect(dial_dial_buttons.back(),SIGNAL(clicked()),mapper,SLOT(map()));
    mapper->setMapping(dial_dial_buttons.back(),i);

    dial_lock_labels.
      push_back(new QLabel(tr("Line")+QString().sprintf(" %d ",i+1)+
			   tr("Lock"),this));
    dial_lock_labels.back()->setFont(label_font);
    dial_lock_labels.back()->setDisabled(true);

    dial_lock_lights.push_back(new StatusLight(this));
    dial_lock_lights.back()->setDisabled(true);

    lineStateChangedData(i,dial_switcher->lineState(i));
  }


  dial_dial_label=new QLabel(tr("Dial String")+":",this);
  dial_dial_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dial_dial_label->setFont(label_font);
  dial_dial_edit=new QLineEdit(this);

  //
  // Transmit Mode
  //
  dial_xmt_mode_label=new QLabel("Xmt:",this);
  dial_xmt_mode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dial_xmt_mode_label->setFont(label_font);
  dial_xmt_mode_edit=new QLineEdit(this);
  dial_xmt_mode_edit->setReadOnly(true);

  dial_xmt_mode_box=new QComboBox(this);
  for(int i=0;i<LPSwitcher::XmtAlgoLast;i++) {
    LPSwitcher::XmtAlgo xmtalgo=(LPSwitcher::XmtAlgo)i;
    if(dial_switcher->xmtSupports(xmtalgo)) {
      dial_xmt_mode_box->insertItem(-1,LPSwitcher::xmtAlgoString(xmtalgo),i);
    }
  }
  connect(dial_xmt_mode_box,SIGNAL(activated(int)),
	  this,SLOT(xmtAlgorithmActivatedData(int)));

  //
  // Receive Mode
  //
  dial_rcv_mode_label=new QLabel("Rcv:",this);
  dial_rcv_mode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dial_rcv_mode_label->setFont(label_font);
  dial_rcv_mode_edit=new QLineEdit(this);
  dial_rcv_mode_edit->setReadOnly(true);

  dial_rcv_mode_box=new QComboBox(this);
  for(int i=0;i<LPSwitcher::RcvAlgoLast;i++) {
    LPSwitcher::RcvAlgo rcvalgo=(LPSwitcher::RcvAlgo)i;
    if(dial_switcher->rcvSupports(rcvalgo)) {
      dial_rcv_mode_box->insertItem(-1,LPSwitcher::rcvAlgoString(rcvalgo),i);
    }
  }
  connect(dial_rcv_mode_box,SIGNAL(activated(int)),
	  this,SLOT(rcvAlgorithmActivatedData(int)));

  //
  // Channel Rate
  //
  dial_channel_rate_label=new QLabel("Channel Rate:",this);
  dial_channel_rate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dial_channel_rate_label->setFont(label_font);
  dial_channel_rate_edit=new QLineEdit(this);
  dial_channel_rate_edit->setReadOnly(true);

  dial_channel_rate_box=new QComboBox(this);
  for(int i=0;i<LPSwitcher::ChannelRateLast;i++) {
    dial_channel_rate_box->insertItem(-1,LPSwitcher::channelRateString((LPSwitcher::ChannelRate)i),i);
  }
  connect(dial_channel_rate_box,SIGNAL(activated(int)),
	  this,SLOT(channelRateActivatedData(int)));

  //
  // Sample Rate
  //
  dial_sample_rate_label=new QLabel("Sample Rate:",this);
  dial_sample_rate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dial_sample_rate_label->setFont(label_font);
  dial_sample_rate_edit=new QLineEdit(this);
  dial_sample_rate_edit->setReadOnly(true);

  dial_sample_rate_box=new QComboBox(this);
  dial_sample_rate_box->insertItem(-1,tr("32 kHz"),32000);
  dial_sample_rate_box->insertItem(-1,tr("48 kHz"),48000);
  connect(dial_sample_rate_box,SIGNAL(activated(int)),
	  this,SLOT(sampleRateActivatedData(int)));
}


Dialer::~Dialer()
{
  for(int i=0;i<dial_switcher->lines();i++) {
    delete dial_state_labels[i];
    delete dial_dialed_string_labels[i];
    delete dial_dial_buttons[i];
  }
  delete dial_dial_edit;
}


QSize Dialer::sizeHint() const
{
  return QSize(490,126+dial_switcher->lines()*26);
}


void Dialer::dialData(int line)
{
  switch(dial_switcher->lineState(line)) {
  case LPSwitcher::StateDisconnecting:
  case LPSwitcher::StateOffline:
  case LPSwitcher::StateProgressing:
    break;

  case LPSwitcher::StateIdle:
    dial_switcher->dialLine(line,dial_dial_edit->text());
    break;

  case LPSwitcher::StateConnected:
    dial_switcher->dropLine(line);
    break;
  }
}


void Dialer::xmtAlgorithmActivatedData(int n)
{
  dial_switcher->
    setXmtAlgorithm((LPSwitcher::XmtAlgo)dial_xmt_mode_box->itemData(dial_xmt_mode_box->currentIndex()).toInt());
}


void Dialer::rcvAlgorithmActivatedData(int n)
{
  dial_switcher->
    setRcvAlgorithm((LPSwitcher::RcvAlgo)dial_rcv_mode_box->itemData(dial_rcv_mode_box->currentIndex()).toInt());
}


void Dialer::channelRateActivatedData(int n)
{
  dial_switcher->
    setChannelRate((LPSwitcher::ChannelRate)dial_channel_rate_box->itemData(dial_channel_rate_box->currentIndex()).toInt());
}


void Dialer::sampleRateActivatedData(int n)
{
  dial_switcher->
    setSampleRate(dial_sample_rate_box->itemData(dial_sample_rate_box->currentIndex()).toInt());
}


void Dialer::lineStateChangedData(int line,LPSwitcher::LineState state)
{
  dial_state_labels[line]->setText(LPSwitcher::lineStateString(state));

  switch(state) {
  case LPSwitcher::StateOffline:
    dial_dial_buttons[line]->setText(tr("Dial"));
    dial_dial_buttons[line]->setDisabled(true);
    dial_lock_lights[line]->setDisabled(true);
    dial_lock_labels[line]->setDisabled(true);
    break;

case LPSwitcher::StateDisconnecting:
  case LPSwitcher::StateIdle:
    dial_dial_buttons[line]->setText(tr("Dial"));
    dial_dial_buttons[line]->setEnabled(true);
    dial_lock_lights[line]->setDisabled(true);
    dial_lock_labels[line]->setDisabled(true);
    break;


  case LPSwitcher::StateConnected:
  case LPSwitcher::StateProgressing:
    dial_dial_buttons[line]->setText(tr("Drop"));
    dial_dial_buttons[line]->setEnabled(true);
    dial_lock_lights[line]->setEnabled(true);
    dial_lock_labels[line]->setEnabled(true);
    break;
  }
}


void Dialer::lockChangedData(int line,bool state)
{
  printf("lockChangedData(%d,%d)\n",line,state);
  dial_lock_lights[line]->setStatus(state);
}


void Dialer::dialedStringChangedData(int line,const QString &str)
{
  dial_dialed_string_labels[line]->setText(str);
}


void Dialer::xmtAlgorithmChangedData(LPSwitcher::XmtAlgo algo)
{
  dial_xmt_mode_edit->setText(LPSwitcher::xmtAlgoString(algo));
  for(int i=0;i<dial_xmt_mode_box->count();i++) {
    if((LPSwitcher::XmtAlgo)dial_xmt_mode_box->itemData(i).toInt()==algo) {
      dial_xmt_mode_box->setCurrentIndex(i);
    }
  }
}


void Dialer::rcvAlgorithmChangedData(LPSwitcher::RcvAlgo algo)
{
  dial_rcv_mode_edit->setText(LPSwitcher::rcvAlgoString(algo));
  for(int i=0;i<dial_rcv_mode_box->count();i++) {
    if((LPSwitcher::RcvAlgo)dial_rcv_mode_box->itemData(i).toInt()==algo) {
      dial_rcv_mode_box->setCurrentIndex(i);
    }
  }
}


void Dialer::channelRateChangedData(LPSwitcher::ChannelRate rate)
{
  dial_channel_rate_edit->setText(LPSwitcher::channelRateString(rate));
  for(int i=0;i<dial_channel_rate_box->count();i++) {
    if((LPSwitcher::ChannelRate)dial_channel_rate_box->itemData(i).toInt()==rate) {
      dial_channel_rate_box->setCurrentIndex(i);
    }
  }
}


void Dialer::sampleRateChangedData(int rate)
{
  dial_sample_rate_edit->setText(QString().sprintf("%d kHz",rate/1000));
  for(int i=0;i<dial_sample_rate_box->count();i++) {
    if(dial_sample_rate_box->itemData(i).toInt()==rate) {
      dial_sample_rate_box->setCurrentIndex(i);
    }
  }
}


void Dialer::resizeEvent(QResizeEvent *e)
{
  for(int i=0;i<dial_switcher->lines();i++) {
    dial_state_labels[i]->setGeometry(5,3+i*26,100,20);
    dial_dialed_string_labels[i]->setGeometry(115,3+i*26,115,20);
    dial_dial_buttons[i]->setGeometry(240,1+i*26,60,24);
    QSize s=dial_lock_lights[i]->sizeHint();
    dial_lock_lights[i]->setGeometry(310,1+i*26,s.width(),s.height());
    dial_lock_labels[i]->setGeometry(315+s.width(),1+i*26,150,s.height());
  }
  dial_dial_label->setGeometry(0,5+dial_switcher->lines()*26,70,20);
  dial_dial_edit->setGeometry(75,5+dial_switcher->lines()*26,290,20);

  dial_xmt_mode_label->setGeometry(0,25+dial_switcher->lines()*26,40,20);
  dial_xmt_mode_edit->setGeometry(40,25+dial_switcher->lines()*26,220,20);
  dial_xmt_mode_box->setGeometry(265,25+dial_switcher->lines()*26,220,20);

  dial_rcv_mode_label->setGeometry(0,47+dial_switcher->lines()*26,40,20);
  dial_rcv_mode_edit->setGeometry(40,47+dial_switcher->lines()*26,220,20);
  dial_rcv_mode_box->setGeometry(265,47+dial_switcher->lines()*26,220,20);

  dial_channel_rate_label->setGeometry(0,69+dial_switcher->lines()*26,90,20);
  dial_channel_rate_edit->setGeometry(95,69+dial_switcher->lines()*26,80,20);
  dial_channel_rate_box->setGeometry(180,69+dial_switcher->lines()*26,110,20);

  dial_sample_rate_label->setGeometry(0,91+dial_switcher->lines()*26,90,20);
  dial_sample_rate_edit->setGeometry(95,91+dial_switcher->lines()*26,80,20);
  dial_sample_rate_box->setGeometry(180,91+dial_switcher->lines()*26,110,20);
}


void Dialer::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);

  p->setPen(Qt::black);
  p->setBrush(Qt::black);

  p->drawLine(0,0,230,0);
  p->drawLine(230,0,230,26*dial_switcher->lines());
  p->drawLine(230,26*dial_switcher->lines(),0,26*dial_switcher->lines());
  p->drawLine(0,26*dial_switcher->lines(),0,0);

  delete p;
}
