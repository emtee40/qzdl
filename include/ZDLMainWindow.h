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
 
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_


#include <QApplication>
#include <QMainWindow>
#include <QStringList>
#include <QString>
#include <QAction>
#include "ZDLWidget.h"
#include "ZDLInterface.h"
#include "ZDLSettingsTab.h"

class ZDLMainWindow: public QMainWindow{
    Q_OBJECT
public:
	ZDLMainWindow( QWidget *parent=0);
	~ZDLMainWindow();
	void startRead();
	void writeConfig();
	QStringList getArguments();
	QString getExecutable();
	void handleImport();
	QString getWindowTitle();
public slots:
	void launch();
	void quit();
	void tabChange(int index);
	void badLaunch();
protected:
	ZDLInterface* intr;
	ZDLSettingsTab* settings;
	
	int procerr;
	QAction *qact2;
};
#endif
