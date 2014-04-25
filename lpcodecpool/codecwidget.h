// codecwidget.h
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


#ifndef CODECWIDGET_H
#define CODECWIDGET_H

#include <QtCore/QTimer>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

class CodecWidget : public QFrame
{
  Q_OBJECT
 public:
  CodecWidget(unsigned port_num,QWidget *parent=0);
  ~CodecWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setPortName(const QString &str);
  void setCodecName(unsigned codec_num,const QString &str);
  void setCodec(unsigned codec_num);

 signals:
  void codecChanged(unsigned port_num,unsigned codec_num);

 private slots:
  void takeClickedData();
  void resetData();
  void codecChangedData(int codec);

 private:
  QLabel *codec_label_label;
  QLabel *codec_state_label;
  QComboBox *codec_codec_box;
  QPushButton *codec_take_button;
  unsigned codec_port_number;
  unsigned codec_codec_number;
  QTimer *codec_reset_timer;
};


#endif  // LPCODECWIDGET_H
