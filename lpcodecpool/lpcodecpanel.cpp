// lpcodecpanel.cpp
//
// Control Panel Applet for LPCodecPool
//
//   (C) Copyright 2013-2014 Fred Gleason <fredg@paravelsystems.com>
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
#include <lpcore/lpprofile.h>

#include "lpcodecpool.h"
#include "lpcodecpanel.h"

//
// Icons
//
#include "../icons/lpcodecpool-16x16.xpm"

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
  QString hostname="localhost";

  //
  // Get Defaults
  //
  LPProfile *p=new LPProfile();
  p->setSource(LPCODECPOOL_CONF_FILE);
  hostname=p->stringValue("Server","Hostname","localhost");
  delete p;

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpcodecpanel",LPCODECPANEL_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--hostname") {
      hostname=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      QMessageBox::warning(this,"LpCodecPanel",
			   tr("Invalid argument")+" \""+cmd->key(i)+"\".");
      exit(256);
    }
  }
  delete cmd;

  //
  // Create And Set Icon
  //
  setWindowIcon(QPixmap(lpcodecpool_16x16_xpm));

  //
  // Connect To Server
  //
  lp_socket=new QTcpSocket(this);
  connect(lp_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(lp_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(lp_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  lp_socket->connectToHost(hostname,LPCODECPOOL_PANEL_TCP_PORT);
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,300);
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


void MainWidget::codecChangedData(unsigned port_num,unsigned codec_num)
{
  SendToServer(QString().sprintf("SX %u %u",codec_num,port_num+1));
}


void MainWidget::errorData(QAbstractSocket::SocketError err)
{
}


void MainWidget::watchdogStatusChangedData(int id,bool state)
{
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  for(unsigned i=0;i<lp_codec_widgets.size();i++) {
    lp_codec_widgets[i]->setGeometry(i*lp_codec_widgets[i]->sizeHint().width(),
				     0,lp_codec_widgets[i]->sizeHint().width(),
				     lp_codec_widgets[i]->sizeHint().height());
  }
}


void MainWidget::ProcessMessage(const QStringList &msg)
{
  bool ok=false;
  QString str;
  unsigned port;
  unsigned codecs;
  bool state=false;

  if(msg[0]=="GC") {
    switch(msg.size()) {
    case 1:    // End of updates
      break;

    case 2:
    case 3:
      break;

    default:
      if(msg[1]=="S") {    // Geometry
	//
	// Set Title
	//
	for(int i=4;i<msg.size();i++) {
	  str+=msg[i]+" ";
	}
	setWindowTitle(str);

	//
	// Create Codec Widgets
	//
	codecs=msg[3].toUInt(&ok);
	if(ok&&(codecs>0)) {
	  for(unsigned i=0;i<codecs;i++) {
	    lp_codec_widgets.push_back(new CodecWidget(i,this));
	    connect(lp_codec_widgets.back(),
		    SIGNAL(codecChanged(unsigned,unsigned)),
		    this,SLOT(codecChangedData(unsigned,unsigned)));
	    lp_codec_widgets.back()->show();
	  }
	  setGeometry(geometry().x(),geometry().y(),
		      codecs*lp_codec_widgets.back()->sizeHint().width(),
		      lp_codec_widgets.back()->sizeHint().height());
	}
	setMinimumWidth(geometry().width());
	setMaximumWidth(geometry().width());
	setMinimumHeight(geometry().height());
	setMaximumHeight(geometry().height());
      }

      if(msg[1]=="O") {
	codecs=msg[2].toUInt(&ok);
	if(ok&&(codecs>0)) {
	  for(unsigned i=0;i<lp_codec_widgets.size();i++) {
	    lp_codec_widgets[i]->setCodecName(codecs,msg[3]);
	  }
	}
      }

      if(msg[1]=="B") {
	codecs=msg[2].toUInt(&ok);
	state=msg[3].toUInt()!=0;
	for(unsigned i=0;i<lp_codec_widgets.size();i++) {
	  lp_codec_widgets[i]->setBusy(codecs,state);
	}
      }

      if(msg[1]=="I") {
	port=msg[2].toUInt(&ok)-1;
	if(ok&&(port<lp_codec_widgets.size())) {
	  for(int i=3;i<msg.size();i++) {
	    str+=msg[i]+" ";
	  }
	  lp_codec_widgets[port]->setPortName(str);
	}
      }
      break;
    }
  }

  if(msg[0]=="SX") {
    port=msg[2].toUInt(&ok)-1;
    if(ok&&(port<lp_codec_widgets.size())) {
      lp_codec_widgets[port]->setCodec(msg[1].toInt());
    }
  }

  if(msg[0]=="MB") {
    for(int i=1;i<msg.size();i++) {
      str+=msg[i]+" ";
    }
    QMessageBox::warning(this,"LPCodecPanel - "+tr("Message"),str);
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
  MainWidget *w=new MainWidget();

  w->setGeometry(w->geometry().x(),w->geometry().y(),
		 w->sizeHint().width(),w->sizeHint().height());
  w->show();
  return a.exec();
}
