// gpiindicator.cpp
//
// GPI Indicator Widget
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpiindicator.cpp,v 1.1 2013/09/09 18:13:48 cvs Exp $
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

#include "gpiindicator.h"

GpiIndicator::GpiIndicator(LPSwitcher *switcher,QWidget *parent)
  : QWidget(parent)
{
  gpi_switcher=switcher;
  connect(gpi_switcher,SIGNAL(gpiChanged(int,int,bool)),
	  this,SLOT(gpiChangedData(int,int,bool)));

  for(int i=0;i<switcher->gpis();i++) {
    gpi_state_labels.push_back(new QLabel(QString().sprintf("%d",i+1),this));
    gpi_state_labels.back()->setAlignment(Qt::AlignCenter);
    gpi_state_lights.push_back(new StatusLight(this));
  }
}


GpiIndicator::~GpiIndicator()
{
  for(unsigned i=0;i<gpi_state_lights.size();i++) {
    delete gpi_state_lights[i];
  }
  for(unsigned i=0;i<gpi_state_labels.size();i++) {
    delete gpi_state_labels[i];
  }
}


QSize GpiIndicator::sizeHint() const
{
  if(gpi_switcher->gpis()==0) {
    return QSize(0,0);
  }
  if(gpi_switcher->gpis()<GPIINDICATOR_COL_COUNT) {
    return QSize(GPIINDICATOR_COL_INTERVAL*gpi_switcher->gpis(),
		 40*((gpi_switcher->gpis()/GPIINDICATOR_COL_COUNT)+1));
  }
  else {
    return QSize(GPIINDICATOR_COL_COUNT*GPIINDICATOR_COL_INTERVAL,
		 40*((gpi_switcher->gpis()/GPIINDICATOR_COL_COUNT)+1));
  }
}


void GpiIndicator::gpiChangedData(int id,int gpi,bool state)
{
  if(id==gpi_switcher->id()) {
    gpi_state_lights[gpi]->setStatus(state);
  }
}


void GpiIndicator::resizeEvent(QResizeEvent *e)
{
  for(unsigned i=0;i<gpi_state_labels.size();i++) {
    int row=i/GPIINDICATOR_COL_COUNT;
    int col=i%GPIINDICATOR_COL_COUNT;

    gpi_state_labels[i]->setGeometry(20*col,40*row,20,20);
    gpi_state_lights[i]->setGeometry(20*col,20+40*row,20,20);
  }
}
