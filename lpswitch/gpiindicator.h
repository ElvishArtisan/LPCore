// gpiindicator.h
//
// GPI Indicator Widget
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpiindicator.h,v 1.1 2013/09/09 18:13:48 cvs Exp $
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

#ifndef GPIINDICATOR_H
#define GPIINDICATOR_H

#include <vector>

#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QWidget>

#include <lpcore/lpconfig.h>
#include <lpcore/lpswitcher.h>

#include "statuslight.h"

#define GPIINDICATOR_COL_COUNT 24
#define GPIINDICATOR_COL_INTERVAL 20

class GpiIndicator : public QWidget
{
 Q_OBJECT;
 public:
  GpiIndicator(LPSwitcher *switcher,QWidget *parent=0);
  ~GpiIndicator();
  QSize sizeHint() const;

 private slots:
  void gpiChangedData(int id,int gpi,bool state);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  std::vector<QLabel *> gpi_state_labels;
  std::vector<StatusLight *> gpi_state_lights;
  LPSwitcher *gpi_switcher;
};


#endif  // GPIINDICATOR_H
