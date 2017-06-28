/*
 * This file is part of qZDL
 * Copyright (C) 2007-2010  Cody Harris
 * 
 * qZDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef _ZADVANCEDMULTIPLAYERDIALOG_H_
#define _ZADVANCEDMULTIPLAYERDIALOG_H_

#include <QObject>
#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include "ZDLWidget.h"

class ZDLAdvancedMultiplayerDialog: public QDialog{
Q_OBJECT
public: 
	ZDLAdvancedMultiplayerDialog(ZDLWidget *parent=0);
private:
	QCheckBox *enable;
	QCheckBox *extratic;
	QComboBox *netmode;
	QLineEdit *portNo;
	QComboBox *dupmode;
private slots:
	void save();
	void close();
	void readConfig();
};

#endif

