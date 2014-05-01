// clicklabel.h
//
// A QLabel with click-through
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

#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QtGui/QLabel>

class ClickLabel : public QLabel
{
  Q_OBJECT
 public:
  ClickLabel(QWidget *parent=0);
  ~ClickLabel();

 signals:
  void clicked();

 protected:
  void mousePressEvent(QMouseEvent *e);
};


#endif  // LPCLICKLABEL_H
