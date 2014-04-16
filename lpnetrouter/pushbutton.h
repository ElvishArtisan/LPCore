// pushbutton.h
//
// Pushbutton for the LPPanel widget
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
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


#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QtCore/QTimer>
#include <QtGui/QColor>
#include <QtGui/QPushButton>

#define PUSHBUTTON_FLASH_INTERVAL 500

class PushButton : public QPushButton
{
  Q_OBJECT
 public:
  PushButton(QWidget *parent=0);
  void setBackgroundColor(const QColor &color);
  void clearBackgroundColor();
  void setFlashingColor(const QColor &color);
  void setFlashing(bool state);

 private slots:
  void timerData();

 private:
  QTimer *button_timer;
  QColor button_background_color;
  QColor button_flash_color;
  bool button_flash_state;
  bool button_flash_active;
};


#endif  // PUSHBUTTON_H
