// lpcore.cpp
//
// lpcore(1) Monitor
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpcore.cpp,v 1.12 2013/11/21 19:47:28 cvs Exp $
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

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <lpcore/lpcmdswitch.h>

#include "lpcore.h"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  QString hostname="localhost";
  unsigned port=LPCORE_CONNECTION_TCP_PORT;
  QString username;
  QString password;
  bool track_state=false;
  bool ok=false;

  //
  // Fonts
  //
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Read Command Options
  //
  LPCmdSwitch *cmd=
    new LPCmdSwitch(qApp->argc(),qApp->argv(),"lpcore",LPCORE_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--hostname") {
      hostname=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--port") {
      port=cmd->value(i).toUInt(&ok);
      if((!ok)||(port>0xFFFF)) {
	fprintf(stderr,"invalid port value\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--username") {
      username=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--password") {
      password=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--track-state") {
      track_state=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"unknown option \"%s\"\n",
	      (const char *)cmd->key(i).toAscii());
      exit(256);
    }
  }
  delete cmd;

  //
  // Open Syslog
  //
  openlog("lpcore",LOG_PERROR,LOG_USER);

  //
  // Connection
  //
  lp_connection=new LPConnection(track_state,this);
  connect(lp_connection,SIGNAL(messageReceived(const LPMessage &)),
	  this,SLOT(messageReceivedData(const LPMessage &)));
  connect(lp_connection,SIGNAL(watchdogStateChanged(bool)),
	  this,SLOT(watchdogStateChangedData(bool)));
  connect(lp_connection,SIGNAL(loggedIn(LPConnection::Result)),
	  this,SLOT(loggedInData(LPConnection::Result)));
  connect(lp_connection,
	  SIGNAL(socketError(QAbstractSocket::SocketError,const QString &)),
	  this,
	  SLOT(socketErrorData(QAbstractSocket::SocketError,const QString &)));

  //
  // Context Selector
  //
  lp_context_label=new QLabel(tr("Context")+":",this);
  lp_context_label->setFont(label_font);
  lp_context_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lp_context_box=new QComboBox(this);
  lp_context_box->insertItem(-1,tr("Global"));
  lp_context_box->insertItem(-1,tr("Isolated"));
  lp_context_box->setCurrentIndex(1);
  connect(lp_context_box,SIGNAL(activated(int)),
	  this,SLOT(contextChangedData(int)));

  lp_clear_button=new QPushButton(tr("Clear"),this);
  lp_clear_button->setFont(label_font);

  //
  // Send Widget
  //
  lp_send_edit=new QLineEdit(this);
  lp_send_edit->setDisabled(true);
  connect(lp_send_edit,SIGNAL(returnPressed()),this,SLOT(returnPressedData()));

  //
  // Receive Widget
  //
  lp_recv_edit=new QTextEdit(this);
  lp_recv_edit->setReadOnly(true);
  connect(lp_clear_button,SIGNAL(clicked()),lp_recv_edit,SLOT(clear()));

  contextChangedData(lp_context_box->currentIndex());
  lp_connection->connectToHost(hostname,port,username,password);

  UpdateWindowTitle();
}


QSize MainWidget::sizeHint() const
{
  return QSize(640,480);
}


void MainWidget::returnPressedData()
{
  char data[1024];
  int ptr=0;
  QString str;
  QString text;
  int istate=0;
  bool ok=false;

  if(lp_send_edit->text().isEmpty()) {
    WriteToHistory("&nbsp;");
    return;
  }

  for(int i=0;i<lp_send_edit->text().length();i++) {
    char c=lp_send_edit->text().toUpper().toAscii()[i];
    switch(istate) {
    case 0:
      switch(c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
	str=c;
        istate=1;
        break;

      case '\'':
	text="";
	istate=2;
	break;
      }
      break;

    case 1:
      switch(c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
	str+=c;
        data[ptr++]=0xFF&str.toInt(&ok,16);
        istate=0;
        break;
      }
      break;

    case 2:
      switch(c) {
      case '\'':
	for(int j=0;j<text.length();j++) {
	  data[ptr++]=text.toAscii()[j];
	}
	if(((0xFF&data[2])==0x5C)&&((0xFF&data[1])==0)) {  // Calculate Length
	  data[1]=0xFF&(8+text.length());
	}
	istate=0;
	break;

      default:
	text+=lp_send_edit->text().toAscii()[i];
	break;
      }
      break;

    default:
      istate=0;
      break;
    }
  }
  if((istate!=0)||(!LPParser::validate(data,ptr))) {
    QMessageBox::warning(this,tr("LPCore - Data Entry Error"),
			 tr("Invalid data entered!"));
    return;
  }
  lp_connection->writeMessage(data,ptr);
  if(lp_connection->context()==LPConnection::Isolated) {
    WriteToHistory(QString("<font color=\"")+LPCORE_SEND_COLOR+"\">"+
		   LPParser::dump(data,ptr)+"  <font color=\""+
		   LPCORE_NOTATION_COLOR+"\">"+
		   LPMessage(data,ptr).decode()+
		   "</font>");
  }
  lp_send_edit->clear();
}


void MainWidget::contextChangedData(int n)
{
  lp_connection->setContext((LPConnection::Context)n);
}


void MainWidget::messageReceivedData(const LPMessage &msg)
{
  QString str;

  for(int i=0;i<msg.size();i++) {
    str+=QString().sprintf("%02X ",0xFF&msg[i]);
  }
  str=str.left(str.length()-1);
  WriteToHistory(QString("<font color=\"")+LPCORE_RECEIVE_COLOR+"\">"+str+
		 "</font>  <font color=\""+LPCORE_NOTATION_COLOR+"\">"+
		 msg.decode()+"</font>");
}


void MainWidget::watchdogStateChangedData(bool state)
{
  lp_send_edit->setEnabled(state);
}


void MainWidget::loggedInData(LPConnection::Result res)
{
  UpdateWindowTitle();
}


void MainWidget::socketErrorData(QAbstractSocket::SocketError err,
				 const QString &str)
{
  QMessageBox::warning(this,tr("LPCore - Network Error"),str);
  exit(256);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  lp_context_label->setGeometry(10,5,70,20);
  lp_context_box->setGeometry(85,5,100,20);
  lp_clear_button->setGeometry(size().width()-60,2,50,26);
  lp_recv_edit->setGeometry(0,30,size().width(),size().height()-50);
  lp_send_edit->setGeometry(0,size().height()-20,size().width(),20);
}


void MainWidget::WriteToHistory(const QString &text)
{
  lp_recv_edit->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
  lp_recv_edit->insertHtml(text+"<br>");
}


void MainWidget::UpdateWindowTitle()
{
  QString str;
  if(lp_connection->profileName().isEmpty()) {
    str+=tr("Status")+": "+tr("Raw Connection");
  }
  else {
    str+=tr("Profile")+": "+lp_connection->profileName()+", ";
    str+=tr("Status")+": "+LPConnection::resultText(lp_connection->loginResult());
  }
  setWindowTitle(tr("LPCore Monitor")+"  ["+str+"]");
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  MainWidget *w=new MainWidget();
  w->show();
  return a.exec();
}
