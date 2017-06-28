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
 
#include "ZDLAdvancedMultiplayerDialog.h"
#include "ZDLConfigurationManager.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QFont>

ZDLAdvancedMultiplayerDialog::ZDLAdvancedMultiplayerDialog(ZDLWidget *parent):QDialog(parent){
	setWindowTitle(ZDL_ENGINE_NAME " Advanced Multiplayer Settings");
	QVBoxLayout *box = new QVBoxLayout(this);
	QGridLayout *form = new QGridLayout();
	//form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,this);
	
	extratic = new QCheckBox("On/Off", this);
	netmode = new QComboBox(this);	
	portNo = new QLineEdit(this);
	dupmode = new QComboBox(this);
	enable = new QCheckBox("Yes/No", this);
		
	netmode->addItem("Not Specified");
	netmode->addItem("0 (Classic)");
	netmode->addItem("1 (Client/Server Model)");
	
	dupmode->addItem("Not Specified");
	dupmode->addItem("1");
	dupmode->addItem("2");
	dupmode->addItem("3");
	dupmode->addItem("4");
	dupmode->addItem("5");
	dupmode->addItem("6");
	dupmode->addItem("7");
	dupmode->addItem("8");
	dupmode->addItem("9");
	
	connect(btnBox, SIGNAL(accepted()), this, SLOT(save()));
	connect(btnBox, SIGNAL(rejected()), this, SLOT(close()));
	
	form->addWidget(new QLabel("Extratic:",this),0,0);
	form->addWidget(extratic,0,1);
	form->addWidget(new QLabel("Net Mode:",this),1,0);
	form->addWidget(netmode,1,1);
	form->addWidget(new QLabel("Port:",this),2,0);
	form->addWidget(portNo,2,1);
	form->addWidget(new QLabel("Dup:",this),3,0);
	form->addWidget(dupmode,3,1);
	form->addWidget(new QLabel("Enable:",this),4,0);
	form->addWidget(enable,4,1);
	
	QLabel *top = new QLabel(ZDL_ENGINE_NAME,this);
	QFont font;
	font.setPointSize(32);
	top->setFont(font);
	top->setAlignment(Qt::AlignHCenter);
	
	QLabel *adv = new QLabel("Advanced Multiplayer Settings", this);
	adv->setAlignment(Qt::AlignHCenter);
	
	QLabel *messageOne = new QLabel("If no port is given, the default will be used.", this);
	QLabel *messageTwo = new QLabel("Settings will only be applied to a multiplayer game and when enabled.", this);
	messageOne->setWordWrap(true);
	messageTwo->setWordWrap(true);
	
	
	box->addWidget(top);
	box->addWidget(adv);
	box->addLayout(form);
	box->addWidget(messageOne);
	box->addWidget(messageTwo);
	box->addWidget(btnBox);
	readConfig();
}

void ZDLAdvancedMultiplayerDialog::close(){
	done(0);
}

void ZDLAdvancedMultiplayerDialog::readConfig(){
	int status;
	LOGDATAO() << "Loading config" << endl;
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	QString tEnabled = "disabled";
	if(zconf->hasValue("zdl.net","advenabled")){
		tEnabled = zconf->getValue("zdl.net","advenabled",&status);
		LOGDATAO() << "Has enabled" << endl;
	}
	LOGDATAO() << "Enabled: " << tEnabled << endl;
	
	QString tExtratic = "disabled";
	if(zconf->hasValue("zdl.net","extratic")){
		tExtratic = zconf->getValue("zdl.net","extratic",&status);
	}

	QString tPort = "";
	if(zconf->hasValue("zdl.net","port")){
		tPort = zconf->getValue("zdl.net","port",&status);
	}

	QString tNetMode = "0";
	if(zconf->hasValue("zdl.net","netmode")){
		tNetMode = zconf->getValue("zdl.net","netmode",&status);
	}

	QString tDup = "0";
	if(zconf->hasValue("zdl.net","dup")){
		tDup = zconf->getValue("zdl.net","dup",&status);
	}
		
	if(tEnabled == "enabled"){
		enable->setCheckState(Qt::Checked);
	}else{
		enable->setCheckState(Qt::Unchecked);
	}
	
	if(tExtratic == "enabled"){
		extratic->setCheckState(Qt::Checked);
	}else{
		extratic->setCheckState(Qt::Unchecked);
	}
	
	portNo->setText(tPort);
	
	bool ok;
	int nDup = tDup.toInt(&ok, 10);
	int nNetMode = tNetMode.toInt(&ok, 10);
	if(nDup > 9){
		nDup = 9;
	}else if(nDup <= 0){
		nDup = 0;
	}
	
	if(nNetMode > 2){
		nNetMode = 2;
	}else if(nNetMode < 0){
		nNetMode = 0;
	}
	dupmode->setCurrentIndex(nDup);
	netmode->setCurrentIndex(nNetMode);
}

void ZDLAdvancedMultiplayerDialog::save(){
	LOGDATAO() << "Saving config" << endl;
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	if(enable->isChecked()){
		zconf->setValue("zdl.net", "advenabled", "enabled");
	}else{
		zconf->setValue("zdl.net", "advenabled", "disabled");
	}
	if(extratic->isChecked()){
		zconf->setValue("zdl.net", "extratic", "enabled");
	}else{
		zconf->setValue("zdl.net", "extratic", "disabled");
	}
	
	if(portNo->text().length() > 0){
		zconf->setValue("zdl.net", "port", portNo->text());
	}else{
		if(zconf->hasValue("zdl.net","port")){
			zconf->deleteValue("zdl.net","port");
		}
	}
	
	
	if(dupmode->currentIndex() > 0){
		zconf->setValue("zdl.net", "dup", dupmode->currentIndex());
	}else{
		if(zconf->hasValue("zdl.net","dup")){
			zconf->deleteValue("zdl.net","dup");
		}
	}
	
	if(netmode->currentIndex() > 0){
		zconf->setValue("zdl.net", "netmode", netmode->currentIndex());
	}else{
		if(zconf->hasValue("zdl.net","netmode")){
			zconf->deleteValue("zdl.net","netmode");
		}
	}
	
	
	done(1);
}
