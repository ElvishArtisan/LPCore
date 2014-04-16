// lpswitch.h
//
// lpswitch(1) switcher control applet
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lpswitch.h,v 1.5 2013/09/09 18:13:48 cvs Exp $
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

#ifndef LPSWITCH_H
#define LPSWITCH_H

#include <vector>

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

#include <lpcore/lpconfig.h>
#include <lpcore/lpswitcher.h>

#include "dialer.h"
#include "gpiindicator.h"

#define LPSWITCH_USAGE "[options]\n\n--hostname=<hostname>\n     Connect to the device at <hostname>\n\n--device=<device>\n     Connect to the switcher device at <device>\n\n--type=<type-num>\n     Switcher driver to use.  The following values are recognized:\n          0 - Broadcast Tools SS 8.2\n          1 - 360 Systems AM16/B\n          2 - Telos Zephyr Digital Network Audio Transceiver\n          3 - Telos Zephyr XStream Digital Network Audio Transceiver\n          4 - GPIO driver-driven GPIO devices\n"

class MainWidget : public QWidget
{
 Q_OBJECT;
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void switchData(int output);
  void switchValueChangedData(int value);
  void crosspointChangedData(int id,int output,int input);
  void pulseGpoData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *main_type_label;
  QLineEdit *main_type_edit;
  QLabel *main_device_label;
  QLineEdit *main_device_edit;
  QLabel *main_output_label;
  QLabel *main_input_label;
  QLabel *main_switch_label;
  std::vector<QLabel *> main_output_labels;
  std::vector<QLineEdit *> main_input_edits;
  std::vector<QSpinBox *> main_change_spins;
  std::vector<QPushButton *> main_change_buttons;
  QSpinBox *main_gpo_spin;
  QPushButton *main_gpo_button;
  Dialer *main_dialer;
  GpiIndicator *main_gpi_indicator;
  LPSwitcher *main_switcher;
};


#endif  // LPSWITCH_H
