// pushbutton.cpp
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

#include <stdio.h>

#include "pushbutton.h"

PushButton::PushButton(QWidget *parent)
  : QPushButton(parent)
{
  button_flash_state=false;
  button_flash_active=false;

  button_timer=new QTimer(this);
  button_timer->setSingleShot(false);
  connect(button_timer,SIGNAL(timeout()),this,SLOT(timerData()));
}


void PushButton::setBackgroundColor(const QColor &color)
{
  button_background_color=color;
  if(!button_flash_active) {
    setStyleSheet("background-color: "+color.name());
  }
}


void PushButton::clearBackgroundColor()
{
  button_background_color=QColor();
  if(!button_flash_active) {
    setStyleSheet("");
  }
}


void PushButton::setFlashingColor(const QColor &color)
{
  button_flash_color=color;
}


void PushButton::setFlashing(bool state)
{
  if(state&&(!button_flash_active)) {
    setStyleSheet("background-color: "+button_flash_color.name());
    button_timer->start(PUSHBUTTON_FLASH_INTERVAL);
    button_flash_state=true;
  }
  if((!state)&&button_flash_active) {
    if(button_background_color.isValid()) {
      setStyleSheet("background-color: "+button_background_color.name());
    }
    else {
      setStyleSheet("");
    }
    button_timer->stop();
  }
  button_flash_active=state;
}


void PushButton::timerData()
{
  if(button_flash_state) {
    setStyleSheet("");
  }
  else {
    setStyleSheet("background-color: "+button_flash_color.name());
  }
  button_flash_state=!button_flash_state;
}

