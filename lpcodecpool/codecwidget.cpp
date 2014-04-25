// codecwidget.cpp
//
// Codec Control Widget
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of
//   the License, or (at your option) any later version.
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

#include <stdio.h>

#include "codecwidget.h"

CodecWidget::CodecWidget(unsigned port_num,QWidget *parent)
  : QFrame(parent)
{
  codec_port_number=port_num;
  codec_codec_number=0;

  setFrameStyle(QFrame::Box|QFrame::Plain);
  setLineWidth(1);

  //
  // Fonts
  //
  QFont button_font("helvetica",16,QFont::Bold);
  button_font.setPixelSize(16);

  codec_label_label=new QLabel(this);
  codec_label_label->setFont(button_font);
  codec_label_label->setAlignment(Qt::AlignCenter);
  codec_label_label->setGeometry(5,0,sizeHint().width()-10,
				 sizeHint().height()/4-10);

  codec_state_label=new QLabel(this);
  codec_state_label->setAlignment(Qt::AlignCenter);
  codec_state_label->setGeometry(5,sizeHint().height()/4-10,
				 sizeHint().width()-10,
				 sizeHint().height()/4-10);
  codec_state_label->setFrameStyle(QFrame::Box|QFrame::Raised);
  codec_state_label->setLineWidth(2);

  codec_codec_box=new QComboBox(this);
  codec_codec_box->setGeometry(5,sizeHint().height()/2-10,
			       sizeHint().width()-10,
			       sizeHint().height()/4-10);
  codec_codec_box->insertItem(0,"-- "+tr("OFF")+" --");
  connect(codec_codec_box,SIGNAL(activated(int)),
	  this,SLOT(codecChangedData(int)));

  codec_take_button=new QPushButton(tr("Take"),this);
  codec_take_button->setFont(button_font);
  codec_take_button->setGeometry(5,3*sizeHint().height()/4-10,
				 sizeHint().width()-10,
				 sizeHint().height()/4);
  connect(codec_take_button,SIGNAL(clicked()),this,SLOT(takeClickedData()));

  codec_reset_timer=new QTimer(this);
  codec_reset_timer->setSingleShot(true);
  connect(codec_reset_timer,SIGNAL(timeout()),this,SLOT(resetData()));
}


CodecWidget::~CodecWidget()
{
  delete codec_take_button;
  delete codec_codec_box;
  delete codec_state_label;
  delete codec_label_label;
}


QSize CodecWidget::sizeHint() const
{
  return QSize(150,200);
}


QSizePolicy CodecWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void CodecWidget::setPortName(const QString &str)
{
  codec_label_label->setText(str);
}


void CodecWidget::setCodecName(unsigned codec_num,const QString &str)
{
  codec_codec_box->insertItem(codec_num,str);
}


void CodecWidget::setCodec(unsigned codec_num)
{
  codec_codec_box->setCurrentIndex(codec_num);
  codec_state_label->setText(codec_codec_box->currentText());
  codec_codec_number=codec_num;
  codec_reset_timer->stop();
}


void CodecWidget::takeClickedData()
{
  if((codec_codec_box->currentIndex()>0)&&
     (codec_codec_box->currentIndex()!=(int)codec_codec_number)) {
    emit codecChanged(codec_port_number,codec_codec_box->currentIndex());
  }
  codec_reset_timer->stop();
}


void CodecWidget::resetData()
{
  codec_codec_box->setCurrentIndex(codec_codec_number);
}


void CodecWidget::codecChangedData(int codec)
{
  if(codec==0) {
    codec_codec_box->setCurrentIndex(codec_codec_number);
  }
  codec_reset_timer->start(15000);
}
