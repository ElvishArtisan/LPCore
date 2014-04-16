// lpswitch.cpp
//
// lpswitch(1) Monitor
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpswitch.cpp,v 1.6 2013/09/09 18:13:48 cvs Exp $
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

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

#include <QtCore/QSignalMapper>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <lpcore/lpcmdswitch.h>
#include <lpcore/lpswitcherfactory.h>

#include "lpswitch.h"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  bool ok=false;
  LPSwitcher::Type type=LPSwitcher::TypeBtSs82;
  QString device="";
  QString hostname="";

  openlog("lpswitch",LOG_PERROR,LOG_USER);

  //
  // Fonts
  //
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont button_font("helvetica",10,QFont::Bold);
  button_font.setPixelSize(10);

  setWindowTitle("LPSwitch");

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpswitch",LPSWITCH_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--device") {
      device=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--hostname") {
      hostname=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--type") {
      type=(LPSwitcher::Type)cmd->value(i).toInt(&ok);
      if((!ok)||(type>=LPSwitcher::TypeLast)) {
	QMessageBox::warning(this,tr("LPSwitch - Error"),
			     tr("Invalid type specified."));
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      QMessageBox::warning(this,tr("LPSwitch - Error"),
			   tr("Unrecognized option")+" \""+cmd->key(i)+"\".");
      exit(256);
    }
  }
  delete cmd;
  if(device.isEmpty()&&hostname.isEmpty()) {
    QMessageBox::warning(this,tr("LPSwitch - Error"),
			 tr("You must specify a device or hostname."));
    exit(256);
  }
  if((!device.isEmpty())&&(!hostname.isEmpty())) {
    QMessageBox::warning(this,tr("LPSwitch - Error"),
	    tr("the --device and --hostname options are mutually exclusive."));
    exit(256);
  }

  //
  // Switch Driver
  //
  if((main_switcher=LPSwitcherFactory(0,type,this))==NULL) {
    QMessageBox::warning(this,tr("LPSwitch - Error"),
			 tr("Unable to create switcher driver."));
    exit(256);
  }
  connect(main_switcher,SIGNAL(crosspointChanged(int,int,int)),
	  this,SLOT(crosspointChangedData(int,int,int)));
  if(!device.isEmpty()) {
    if(!main_switcher->open(device)) {
      QMessageBox::warning(this,tr("LPSwitch - Error"),
			   tr("Unable to open device")+" \""+device+"\".");
      exit(256);
    }
  }
  if(!hostname.isEmpty()) {
    main_switcher->connectToHost(hostname);
  }

  //
  // Switcher Type
  //
  main_type_label=new QLabel("Type:",this);
  main_type_label->setFont(label_font);
  main_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  main_type_edit=new QLineEdit(this);
  main_type_edit->setReadOnly(true);
  main_type_edit->setText(LPSwitcher::typeString(type));

  //
  // Switcher Device
  //
  main_device_label=new QLabel("Device:",this);
  main_device_label->setFont(label_font);
  main_device_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  main_device_edit=new QLineEdit(this);
  main_device_edit->setReadOnly(true);
  main_device_edit->setText(device);

  //
  // Header
  //
  main_output_label=new QLabel(tr("Output"),this);
  main_output_label->setFont(label_font);
  main_output_label->setAlignment(Qt::AlignCenter);

  main_input_label=new QLabel(tr("Input"),this);
  main_input_label->setFont(label_font);
  main_input_label->setAlignment(Qt::AlignCenter);

  main_switch_label=new QLabel(tr("Switch"),this);
  main_switch_label->setFont(label_font);
  main_switch_label->setAlignment(Qt::AlignCenter);

  if((main_switcher->inputs()==0)||(main_switcher->outputs()==0)) {
    main_output_label->hide();
    main_input_label->hide();
    main_switch_label->hide();
  }

  //
  // Output Controls
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(switchData(int)));
  for(int i=0;i<main_switcher->outputs();i++) {
    main_output_labels.push_back(new QLabel(QString().sprintf("%d",i+1),this));
    main_output_labels.back()->setAlignment(Qt::AlignCenter);

    main_input_edits.push_back(new QLineEdit(this));
    main_input_edits.back()->setReadOnly(true);
    main_input_edits.back()->
      setText(QString().sprintf("%d",main_switcher->crosspoint(i)+1));

    main_change_buttons.push_back(new QPushButton(tr("Switch"),this));
    main_change_buttons.back()->setFont(button_font);
    main_change_buttons.back()->setDisabled(true);
    connect(main_change_buttons.back(),SIGNAL(clicked()),mapper,SLOT(map()));
    mapper->setMapping(main_change_buttons.back(),i);

    main_change_spins.push_back(new QSpinBox(this));
    main_change_spins.back()->setRange(1,main_switcher->inputs());
    connect(main_change_spins.back(),SIGNAL(valueChanged(int)),
	    this,SLOT(switchValueChangedData(int)));
  }

  //
  // GPO Controls
  //
  main_gpo_spin=new QSpinBox(this);
  main_gpo_spin->setRange(1,main_switcher->gpos());
  main_gpo_spin->setDisabled(main_switcher->gpos()==0);

  main_gpo_button=new QPushButton(tr("Pulse GPO"),this);
  main_gpo_button->setFont(button_font);
  main_gpo_button->setDisabled(main_switcher->gpos()==0);
  connect(main_gpo_button,SIGNAL(clicked()),this,SLOT(pulseGpoData()));
  if(main_switcher->gpos()==0) {
    main_gpo_spin->hide();
    main_gpo_button->hide();
  }


  //
  // Dialer
  //
  main_dialer=new Dialer(main_switcher,this);
  main_dialer->setDisabled(main_switcher->lines()==0);
  if(main_switcher->lines()==0) {
    main_dialer->hide();
  }

  //
  // GPI Indicator
  //
  main_gpi_indicator=new GpiIndicator(main_switcher,this);
  if(main_switcher->gpis()==0) {
    main_gpi_indicator->hide();
  }

  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());
}


QSize MainWidget::sizeHint() const
{
  int h=70;
  if((main_switcher->inputs()>0)&&(main_switcher->outputs()>0)) {
    h+=38+28*main_switcher->outputs();
  }

  if(main_switcher->gpos()>0) {
    h+=40;
  }

  if(main_switcher->gpis()>0) {
    h+=main_gpi_indicator->sizeHint().height();
  }

  if(main_switcher->lines()>0) {
    h+=main_dialer->sizeHint().height();
  }

  return QSize(520,h);
}


void MainWidget::switchData(int output)
{
  main_switcher->setCrosspoint(output,main_change_spins[output]->value()-1);
}


void MainWidget::switchValueChangedData(int value)
{
  for(unsigned i=0;i<main_change_spins.size();i++) {
    main_change_buttons[i]->
      setDisabled(main_change_spins[i]->value()==
		  (main_switcher->crosspoint(i)+1));
  }
}


void MainWidget::pulseGpoData()
{
  main_switcher->pulseGpo(main_gpo_spin->value()-1);
}


void MainWidget::crosspointChangedData(int id,int output,int input)
{
  if(output>main_switcher->outputs()) {
    QMessageBox::warning(this,tr("LPSwitch - Error"),
			 tr("Non-existing output referenced!"));
    return;
  }
  main_input_edits[output]->setText(QString().sprintf("%d",input+1));
  for(unsigned i=0;i<main_change_spins.size();i++) {
    main_change_spins[i]->setValue(main_switcher->crosspoint(i)+1);
    main_change_buttons[i]->setDisabled(true);
  }
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  main_type_label->setGeometry(10,10,60,20);
  main_type_edit->setGeometry(75,10,size().width()-85,20);

  main_device_label->setGeometry(10,32,60,20);
  main_device_edit->setGeometry(75,32,size().width()-85,20);

  int y=60;

  if((main_switcher->inputs()>0)&&(main_switcher->outputs()>0)) {
    main_output_label->setGeometry(85,y,50,20);
    main_input_label->setGeometry(145,y,50,20);
    main_switch_label->setGeometry(265,y,50,20);

    for(int i=0;i<main_switcher->outputs();i++) {
      main_output_labels[i]->setGeometry(85,82+i*28,50,20);
      main_input_edits[i]->setGeometry(145,82+i*28,50,20);
      main_change_buttons[i]->setGeometry(205,79+i*28,50,26);
      main_change_spins[i]->setGeometry(265,79+i*28,50,20);
    }
    y+=38+28*main_switcher->outputs();
  }

  if(main_switcher->gpos()>0) {
    main_gpo_button->setGeometry(145,y,100,26);
    main_gpo_spin->setGeometry(260,y+2,50,20);
    y+=40;
  }

  if(main_switcher->gpis()>0) {
    int x=(size().width()-main_gpi_indicator->sizeHint().width())/2;
    main_gpi_indicator->setGeometry(x,y,
				    main_gpi_indicator->sizeHint().width(),
				    main_gpi_indicator->sizeHint().height());
    y+=main_gpi_indicator->sizeHint().height();
  }

  main_dialer->setGeometry(15,y,
			   main_dialer->sizeHint().width(),
			   main_dialer->sizeHint().height());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  MainWidget *w=new MainWidget();
  w->show();
  return a.exec();
}
