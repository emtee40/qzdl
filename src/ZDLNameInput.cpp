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
 
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QFileDialog>
#include "ZDLNameInput.h"
#include "ZDLConfigurationManager.h"
#include <string>

QString getLastDir(){
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	if(!zconf){
		return QString();
	}
	QString lastDir;
	if (zconf->hasValue("zdl.general", "lastDir")) {
		int ok = 0;
		lastDir = zconf->getValue("zdl.general", "lastDir", &ok);
	}
	return lastDir;
}

void saveLastDir(QString fileName){
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	if(!zconf){
		return;
	}
	QFileInfo fi(fileName);
	zconf->setValue("zdl.general", "lastDir", fi.absolutePath());
}

ZDLNameInput::ZDLNameInput(QWidget *parent):QDialog(parent){
	QVBoxLayout *lays = new QVBoxLayout(this);
	QHBoxLayout *files = new QHBoxLayout();
	
	QHBoxLayout *buttons = new QHBoxLayout();
	
	QPushButton *btnOK = new QPushButton("OK", this);
	QPushButton *btnCancel = new QPushButton("Cancel", this);
	
	lname = new QLineEdit(this);
	lfile = new QLineEdit(this);
	btnBrowse = new QPushButton("Browse...", this);
	
	lays->addWidget(new QLabel("Name", this));
	lays->addWidget(lname);
	
	lays->addWidget(new QLabel("File", this));
	
	files->addWidget(lfile);
	files->addWidget(btnBrowse);
	
	lays->addLayout(files);
	
	buttons->addWidget(btnOK);
	buttons->addWidget(btnCancel);
	
	lays->addLayout(buttons);
	
	connect(btnBrowse, SIGNAL(clicked()), this, SLOT(browse()));
	connect(btnOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

void ZDLNameInput::browse(){
	QString filter = filters.join(";;");
	QString lastDir = getLastDir();
	QString fileName = QFileDialog::getOpenFileName(this, "Add File", lastDir, filter);
	if(!fileName.isNull() && !fileName.isEmpty()){
		lfile->setText(fileName);
		saveLastDir(fileName);
	}
}

void ZDLNameInput::fromUrl(QUrl url){
	lfile->setText(url.path());
}

void ZDLNameInput::basedOff(ZDLNameListable *listable){
	if (listable){
		lfile->setText(listable->getFile());
		lname->setText(listable->getName());
	}
}

void ZDLNameInput::setFilter(QStringList inFilters){
	filters = inFilters;
}

QString ZDLNameInput::getName() {
	if (lname->text().length() > 0) {
		return lname->text();
	} else {
		return lfile->text();
	}
}

QString ZDLNameInput::getFile() {
	return lfile->text();
}
