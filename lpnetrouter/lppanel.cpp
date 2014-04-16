// lppanel.cpp
//
// Switcher Control Panel Applet for LPNetRouter
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

#include <stdlib.h>

#include <QtCore/QFile>
#include <QtCore/QSignalMapper>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <lpcore/lpcmdswitch.h>

#include "lpnetrouter.h"
#include "lppanel.h"

//
// Icons
//
//#include "../icons/lpanels-22x22.xpm"

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  lp_first_connection=false;
  lp_active_source=0;
  lp_armed_source=0;
  lp_initial_armed_source=0;
  lp_output_number=0;
  lp_active_input=-1;
  lp_armed_input=-1;

  QString hostname="localhost";
  bool ok=false;

  //
  // Get Defaults
  //
  LPProfile *p=new LPProfile();
  p->setSource(LPNETROUTER_CONF_FILE);
  hostname=p->stringValue("Server","Hostname","localhost");
  delete p;

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lppanel",LPPANEL_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--hostname") {
      hostname=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--output") {
      lp_output_number=cmd->value(i).toInt(&ok)-1;
      cmd->setProcessed(i,true);
      if((!ok)||(lp_output_number<0)) {
	QMessageBox::warning(this,"LPPanel",
			     tr("Invalid output")+" \""+cmd->value(i)+
			     "\" specified.");
	exit(256);
      }
    }
    if(cmd->key(i)=="--arm") {
      lp_initial_armed_source=cmd->value(i).toInt(&ok);
      cmd->setProcessed(i,true);
      if((!ok)||(lp_initial_armed_source<=0)) {
	QMessageBox::warning(this,tr("LPPanel"),
			     tr("Invalid source number specified!"));
	exit(256);
      }
    }
    if(!cmd->processed(i)) {
      QMessageBox::warning(this,"LPPanel",
			   tr("Invalid argument")+" \""+cmd->key(i)+"\".");
      exit(256);
    }
  }
  delete cmd;

  //
  // Button Mapper
  //
  lp_button_mapper=new QSignalMapper(this);
  connect(lp_button_mapper,SIGNAL(mapped(int)),
	  this,SLOT(buttonClickedData(int)));

  /*
  //
  // Create And Set Icon
  //
  lp_lpanels_map=new QPixmap(lpanels_22x22_xpm);
  setIcon(*lp_lpanels_map);

  //
  // Set Window Parameters
  //
  if(lp_config->stayOnTop()) {
    setWFlags(Qt::WStyle_StaysOnTop);
  }
  */
  //
  // Create Fonts
  //
  QFont button_font=QFont("helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Connect To Server
  //
  lp_socket=new QTcpSocket(this);
  connect(lp_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(lp_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(lp_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  lp_socket->connectToHost(hostname,LPNETROUTER_PANEL_TCP_PORT);
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,300);
  //  return QSize(10+90*lp_config->sourceQuantity(),100);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::connectedData()
{
  SendToServer("GC");
}


void MainWidget::readyReadData()
{
  int n;
  char data[1500];

  while((n=lp_socket->read(data,1500))>0) {
    for(int i=0;i<n;i++) {
      if(data[i]=='!') {
	//printf("RECV: %s\n",(const char *)lp_socket_buffer.toAscii());
	ProcessMessage(lp_socket_buffer.split(" "));
	lp_socket_buffer="";
      }
      else {
	lp_socket_buffer+=data[i];
      }
    }
  }
}


void MainWidget::errorData(QAbstractSocket::SocketError err)
{
}


void MainWidget::buttonClickedData(int n)
{
  if(n==lp_armed_input) {
    SendToServer(QString().sprintf("SX %d %d",lp_output_number+1,n+1));
  }
  else {
    SendToServer(QString().sprintf("AX %d %d",lp_output_number+1,n+1));
  }
}


void MainWidget::watchdogStatusChangedData(int id,bool state)
{
}


void MainWidget::ProcessMessage(const QStringList &msg)
{
  bool ok=false;
  QString str;

  if(msg[0]=="AX") {
    if((msg[1].toInt()-1)==lp_output_number) {
      if((msg[2].toInt()-1)!=lp_armed_input) {
	lp_armed_input=msg[2].toInt()-1;
	for(unsigned i=0;i<lp_buttons.size();i++) {
	  lp_buttons[i]->setFlashing(false);
	}
	if((lp_armed_input<(int)lp_buttons.size())&&
	   (lp_armed_input!=lp_active_input)) {
	  lp_buttons[lp_armed_input]->setFlashing(true);
	}
      }
    }
  }

  if(msg[0]=="GC") {
    switch(msg.size()) {
    case 1:    // End of updates
      break;

    case 2:
    case 3:
      break;

    default:
      if(msg[1]=="S") {    // Geometry
	lp_inputs=msg[3].toInt();
	for(int i=0;i<lp_inputs;i++) {
	  lp_buttons.push_back(new PushButton(this));
	  lp_buttons.back()->setFlashingColor(LPPANEL_BUTTON_COLOR);
	  connect(lp_buttons.back(),SIGNAL(clicked()),
		  lp_button_mapper,SLOT(map()));
	  lp_button_mapper->setMapping(lp_buttons.back(),i);
	  lp_buttons.back()->show();
	  lp_buttons.back()->
	    setGeometry(10+i*(10+LPPANEL_BUTTON_WIDTH),10,
			LPPANEL_BUTTON_WIDTH,LPPANEL_BUTTON_HEIGHT);
	}
	setGeometry(geometry().x(),geometry().y(),
		    10+lp_inputs*(10+LPPANEL_BUTTON_WIDTH),
		    20+LPPANEL_BUTTON_HEIGHT);
	setMinimumWidth(geometry().width());
	setMaximumWidth(geometry().width());
	setMinimumHeight(geometry().height());
	setMaximumHeight(geometry().height());
      }

      if(msg[1]=="I") {    // Inputs
	unsigned input=msg[2].toUInt(&ok)-1;
	if(ok&&(input<=lp_buttons.size())) {
	  for(int i=3;i<msg.size();i++) {
	    str+=msg[i]+" ";
	  }
	  str=str.left(str.length()-1);
	  lp_buttons[input]->setText(str);
	}
      }

      if(msg[1]=="O") {    // Outputs
	if((msg[2].toInt()-1)==lp_output_number) {
	  for(int i=3;i<msg.size();i++) {
	    str+=msg[i]+" ";
	  }
	  setWindowTitle(str.left(str.length()-1));
	}
      }
      break;
    }
  }

  if(msg[0]=="SX") {
    if((msg[1].toInt()-1)==lp_output_number) {
      if(msg[2].toInt()!=(lp_active_input-1)) {
	lp_active_input=msg[2].toInt()-1;
	for(unsigned i=0;i<lp_buttons.size();i++) {
	  lp_buttons[i]->clearBackgroundColor();
	}
	if(lp_active_input<(int)lp_buttons.size()) {
	  lp_buttons[lp_active_input]->setBackgroundColor(LPPANEL_BUTTON_COLOR);
	  lp_buttons[lp_active_input]->setFlashing(false);
	}
      }
    }
  }
}


void MainWidget::SendToServer(const QString &msg)
{
  //printf("SENDING: %s\n",(const char *)msg.toAscii());
  lp_socket->write((msg+"!").toAscii(),msg.length()+1);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  Qt::WFlags flags=0;

  //
  // Start Event Loop
  //
  /*
  LPConfig *config=LoadConfig(qApp->argc(),qApp->argv());
  if(config->stayOnTop()) {
    flags=Qt::WStyle_StaysOnTop;
  }
  MainWidget *w=new MainWidget(config,NULL,"main",flags);
  */
  MainWidget *w=new MainWidget();
  //a.setMainWidget(w);
  w->setGeometry(w->geometry().x(),w->geometry().y(),
		 w->sizeHint().width(),w->sizeHint().height());
  w->show();
  return a.exec();
}
