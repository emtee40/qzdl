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

#include <iostream>
#include <QtGui>
#include <QApplication>
#include <QMainWindow>

#include "ZDLInterface.h"
#include "ZDLMainWindow.h"
#include "ZDLConfigurationManager.h"
#include "ZDLInfoBar.h"
#include "ZDLImportDialog.hpp"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include <QLayout>
#include <QAction>
#include <QMessageBox>

extern QApplication *qapp;
extern QString versionString;

ZDLMainWindow::~ZDLMainWindow(){
	QSize sze = this->size();
	QPoint pt = this->pos();
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	if(zconf){
		QString str = QString("%1,%2").arg(sze.width()).arg(sze.height());
		zconf->setValue("zdl.general", "windowsize", str);
		str = QString("%1,%2").arg(pt.x()).arg(pt.y());
		zconf->setValue("zdl.general", "windowpos", str);
	}
	LOGDATAO() << "Closing main window" << endl;
}

QString ZDLMainWindow::getWindowTitle(){
	QString windowTitle = ZDL_ENGINE_NAME;
	windowTitle += " " ZDL_VERSION_STRING " - ";
	ZDLConfiguration *conf = ZDLConfigurationManager::getConfiguration();
	if(conf){
		QString userConfPath = conf->getPath(ZDLConfiguration::CONF_USER);
		QString currentConf = ZDLConfigurationManager::getConfigFileName();
		if(userConfPath != currentConf){
			windowTitle += ZDLConfigurationManager::getConfigFileName();
		}else{
			windowTitle += "user config";
		}
	}else{
		windowTitle += ZDLConfigurationManager::getConfigFileName();
	}
	LOGDATAO() << "Returning main window title " << windowTitle << endl;
	return windowTitle;

}

ZDLMainWindow::ZDLMainWindow(QWidget *parent): QMainWindow(parent){
	LOGDATAO() << "New main window " << DPTR(this) << endl;
	QString windowTitle = getWindowTitle();
	setWindowTitle(windowTitle);

	setWindowIcon(ZDLConfigurationManager::getIcon());


	setContentsMargins(2,2,2,2);
	layout()->setContentsMargins(2,2,2,2);
	QTabWidget *widget = new QTabWidget(this);

	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	if(zconf){
		int ok = 0;
		bool qtok = false;
		if(zconf->hasValue("zdl.general", "windowsize")){
			QString size = zconf->getValue("zdl.general", "windowsize", &ok);
			if(size.contains(",")){
				QStringList list = size.split(",");
				int w = list[0].toInt(&qtok);
				if(qtok){
					int h = list[1].toInt(&qtok);
					if(qtok){
						LOGDATAO() << "Resizing to w:" << w << " h:" << h << endl;
						this->resize(QSize(w,h));
					}
				}
			}
		}
		if(zconf->hasValue("zdl.general", "windowpos")){
			QString size = zconf->getValue("zdl.general", "windowpos", &ok);
			if(size.contains(",")){
				QStringList list = size.split(",");
				int x = list[0].toInt(&qtok);
				if(qtok){
					int y = list[1].toInt(&qtok);
					if(qtok){
						LOGDATAO() << "Moving to x:" << x << " y:" << y << endl;
						this->move(QPoint(x,y));
					}
				}
			}

		}
	}


	intr = new ZDLInterface(this);
	settings = new ZDLSettingsTab(this);

	setCentralWidget(widget);
	widget->addTab(intr, "Main");
	widget->addTab(settings, "Settings");
	//I haven't started on this yet :)
	//widget->addTab(new ZDLInterface(this), "Notifications");

	QAction *qact = new QAction(widget);
	qact->setShortcut(Qt::Key_Return);
	widget->addAction(qact);
	connect(qact, SIGNAL(triggered()), this, SLOT(launch()));

	qact2 = new QAction(widget);
	qact2->setShortcut(Qt::Key_Escape);
	widget->addAction(qact2);

	connect(qact2, SIGNAL(triggered()), this, SLOT(quit()));

	connect(widget, SIGNAL(currentChanged(int)), this, SLOT(tabChange(int)));
	LOGDATAO() << "Main window created." << endl;
}

void ZDLMainWindow::handleImport(){
#if !defined(NO_IMPORT)
	ZDLConfiguration *conf = ZDLConfigurationManager::getConfiguration();
	if(conf){
		QString userConfPath = conf->getPath(ZDLConfiguration::CONF_USER);
		QString currentConf = ZDLConfigurationManager::getConfigFileName();
		if(userConfPath != currentConf){
			LOGDATAO() << "Not currently using user conf" << endl;
			ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
			if(zconf->hasValue("zdl.general", "donotimportthis")){
				int ok = 0;
				if(zconf->getValue("zdl.general", "donotimportthis", &ok) == "1"){
					LOGDATAO() << "Don't import current config" << endl;
					return;
				}
			}
			QFile userFile(userConfPath);
			ZDLConf userconf;
			if(userFile.exists()){
				LOGDATAO() << "Reading user conf" << endl;
				userconf.readINI(userConfPath);
			}
			if(userconf.hasValue("zdl.general", "nouserconf")){
				int ok = 0;
				if(userconf.getValue("zdl.general", "nouserconf", &ok) == "1"){
					LOGDATAO() << "Do not use user conf" << endl;
					return;
				}
			}
			if(ZDLConfigurationManager::getWhy() == ZDLConfigurationManager::USER_SPECIFIED){
				LOGDATA() << "The user asked for this ini, don't go forward" << endl;
				return;
			}
			if(userFile.size() < 10){
				LOGDATA() << "User conf is small, assuming empty" << endl;
				ZDLImportDialog importd(this);
				importd.exec();
				if(importd.result() == QDialog::Accepted){
					switch(importd.getImportAction()){
						case ZDLImportDialog::IMPORTNOW:
							LOGDATAO() << "Importing now" << endl;
							if(!userFile.exists()){
								QStringList path = userConfPath.split("/");
								path.removeLast();
								QDir dir;
								if(!dir.mkpath(path.join("/"))){
									break;
								}
							}

							zconf->setValue("zdl.general", "importedfrom", currentConf);
							zconf->setValue("zdl.general", "isimported", "1");
							zconf->setValue("zdl.general", "importdate", QDateTime::currentDateTime().toString(Qt::ISODate));

							zconf->writeINI(userConfPath);
							ZDLConfigurationManager::setConfigFileName(userConfPath);
							break;
						case ZDLImportDialog::DONOTIMPORTTHIS:
							LOGDATAO() << "Tagging this config as not importable" << endl;
							zconf->setValue("zdl.general", "donotimportthis", "1");
							break;
						case ZDLImportDialog::NEVERIMPORT:
							LOGDATAO() << "Setting NEVERi IMPORT" << endl;
							userconf.setValue("zdl.general", "nouserconf", "1");
							if(!userFile.exists()){
								QStringList path = userConfPath.split("/");
								path.removeLast();
								QDir dir;
								if(!dir.mkpath(path.join("/"))){
									break;
								}

							}	
							userconf.writeINI(userConfPath);
							break;
						case ZDLImportDialog::ASKLATER:

						case ZDLImportDialog::UNKNOWN:
						default:
							LOGDATAO() << "Not setting anything" << endl;
							break;
					}
				}
			}
		}
	}
#endif
}

void ZDLMainWindow::tabChange(int newTab){
	LOGDATAO() << "Tab changed to " << newTab << endl;
	if(newTab == 0){
		settings->notifyFromParent(NULL);
		intr->readFromParent(NULL);
	}else if (newTab == 1){
		intr->notifyFromParent(NULL);
		settings->readFromParent(NULL);
	}
}

void ZDLMainWindow::quit(){
	LOGDATAO() << "quitting" << endl;
	writeConfig();
	close();
}

void ZDLMainWindow::launch(){
	LOGDATAO() << "Launching" << endl;
	writeConfig();
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();

	QString exec = getExecutable();
	if (exec.length() < 1){
		QMessageBox::critical(this, ZDL_ENGINE_NAME, "Please select a source port");
		return;
	}
	QStringList args = getArguments();
	if (args.join("").length() < 1){
		return;
	}

	if(exec.contains("\\")){
		exec.replace("\\","/");
	}

	//Find the executable
	QStringList executablePath = exec.split("/");

	//Remove the last item, which will be the .exe
	executablePath.removeLast();

	//Re-create the string
	QString workingDirectory = executablePath.join("/");

	//Resolve the path to an absolute directory
	QDir cwd(workingDirectory);
	workingDirectory = cwd.absolutePath();
	LOGDATAO() << "Working directory: " << workingDirectory << endl;
	// Turns on launch confirmation
	/*QMessageBox::StandardButton btnrc = QMessageBox::question(this, "Would you like to continue?","Executable: "+exec+"\n\nArguments: "+args.join(" ")+"\n\nWorking Directory: "+workingDirectory, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	  if(btnrc == QMessageBox::No){
	  ZDLConfigurationManager::setInfobarMessage("Launch aborted.",1);
	  return;
	  }*/
#ifdef Q_WS_WIN
	/* The first argument to CreateProcess must not be quoted */
	/* TODO: Rewrite this code! */
	QString unquotedExec = exec;
	int doquotes = 1;
	if(zconf->hasValue("zdl.general","quotefiles")){
		int ok;
		QString rc = zconf->getValue("zdl.general","quotefiles",&ok);
		if(rc == "disabled"){
			doquotes = 0;
		}
	}
	//If quotefiles is enabled, and the executable contains a space, quote it
	if(doquotes && exec.contains(" ")){
		QString newExec = QString("\"").append(exec).append("\"");
		exec = newExec;
	}

	QString compose = exec + QString(" ") + args.join(" ");
	LOGDATAO() << "Launching: " << compose << endl;
	wchar_t* cmd = (wchar_t*)malloc((compose.length()+1)*sizeof(wchar_t)*4);
	wcscpy(cmd,compose.toStdWString().c_str());
	//swprintf(cmd,L"%ls",compose.toStdWString().c_str());

	wchar_t* execu = (wchar_t*)malloc((unquotedExec.length()+1)*sizeof(wchar_t)*4);
	wchar_t* work = (wchar_t*)malloc((workingDirectory.length()+1)*sizeof(wchar_t)*4);
	//swprintf(execu, L"%ls",exec.toStdWString().c_str());
	wcscpy(execu, unquotedExec.toStdWString().c_str());
	wcscpy(work, workingDirectory.toStdWString().c_str());
	//http://www.goffconcepts.com/techarticles/development/cpp/createprocess.html
	STARTUPINFOW siStartupInfo; 
	PROCESS_INFORMATION piProcessInfo; 
	memset(&siStartupInfo, 0, sizeof(siStartupInfo)); 
	memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
	siStartupInfo.cb = sizeof(siStartupInfo); 

	if(execu == NULL || cmd == NULL || work == NULL){
		ZDLConfigurationManager::setInfobarMessage("Internal error preparing to launch",1);
		return;
	}

	BOOL rc = CreateProcess(execu, cmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, work, &siStartupInfo, &piProcessInfo);
	free(cmd);
	free(execu);
	free(work);
	if(rc == FALSE){
		ZDLConfigurationManager::setInfobarMessage("Failed to launch the process!",1);
		ZDLInfoBar *bar = (ZDLInfoBar*)ZDLConfigurationManager::getInfobar();
		connect(bar,SIGNAL(moreclicked()),this,SLOT(badLaunch()));
	}
#else
	QProcess *proc = new QProcess(this);

	//Set the working directory to that directory
	proc->setWorkingDirectory(workingDirectory);

	proc->setProcessChannelMode(QProcess::ForwardedChannels);
	proc->start(exec, args);
	procerr = proc->error();
#endif
	int stat;
	if (zconf->hasValue("zdl.general", "autoclose")){
		QString append = zconf->getValue("zdl.general", "autoclose",&stat);
		if (append == "1" || append == "true"){
			LOGDATAO() << "Asked to exit... closing" << endl;
			close();
		}
	}

	// 	if(proc->state() != QProcess::NotRunning){
	// 		std::cout << "ERROR!" << std::endl;
	// 		ZDLConfigurationManager::setInfobarMessage("The process ended abnormally.",1);
	// 		ZDLInfoBar *bar = (ZDLInfoBar*)ZDLConfigurationManager::getInfobar();
	// 		connect(bar,SIGNAL(moreclicked()),this,SLOT(badLaunch()));
	// 	}
}

void ZDLMainWindow::badLaunch(){
	LOGDATAO() << "badLaunch message box" << endl;
	if(procerr == QProcess::FailedToStart){
		QMessageBox::warning(NULL,"Failed to Start", "Failed to launch the application executable.",QMessageBox::Ok,QMessageBox::Ok);
	}else if(procerr == QProcess::Crashed){
		QMessageBox::warning(NULL,"Process Crashed", "The application ended abnormally (usually due to a crash or error).",QMessageBox::Ok,QMessageBox::Ok);
	}else{
		QMessageBox::warning(NULL,"Unknown error", "There was a problem running the application.",QMessageBox::Ok,QMessageBox::Ok);
	}
}

QStringList ZDLMainWindow::getArguments(){
	LOGDATAO() << "Getting arguments" << endl;
	QStringList ourString;
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	ZDLSection *section = NULL;

	QString iwadName = "";

	bool ok;
	int stat;
	int doquotes = 1;

	if(zconf->hasValue("zdl.save", "iwad")){
		QString rc = zconf->getValue("zdl.save", "iwad", &stat);
		if (rc.length() > 0){
			iwadName = rc;
		}else{
			QMessageBox::critical(this, ZDL_ENGINE_NAME, "Please select an IWAD");
			return ourString;
		}
	}else{
		QMessageBox::critical(this, ZDL_ENGINE_NAME, "Please select an IWAD");
		return ourString;
	}

	section = zconf->getSection("zdl.iwads");
	if (section){
		QVector<ZDLLine*> fileVctr;
		section->getRegex("^i[0-9]+n$", fileVctr);

		for(int i = 0; i < fileVctr.size(); i++){
			ZDLLine *line = fileVctr[i];
			if(line->getValue().compare(iwadName) == 0){
				QString var = line->getVariable();
				if(var.length() >= 3){
					var = var.mid(1,var.length()-2);
					QVector<ZDLLine*> nameVctr;
					var = QString("i") + var + QString("f");
					section->getRegex("^" + var + "$",nameVctr);
					if(nameVctr.size() == 1){
						ourString << "-iwad";
						ourString << nameVctr[0]->getValue();
					}
				}
			}
		}
	}

	if (zconf->hasValue("zdl.save", "skill")){
		ourString << "-skill";
		ourString << zconf->getValue("zdl.save", "skill", &stat);
	}

	if (zconf->hasValue("zdl.save", "warp")){
		ourString << "+map";
		ourString << zconf->getValue("zdl.save", "warp", &stat);
	}

	if (zconf->hasValue("zdl.save", "dmflags")){
		ourString << "+set";
		ourString << "dmflags";
		ourString << zconf->getValue("zdl.save", "dmflags", &stat);
	}

	if (zconf->hasValue("zdl.save", "dmflags2")){
		ourString << "+set";
		ourString << "dmflags2";
		ourString << zconf->getValue("zdl.save", "dmflags2", &stat);
	}

	section = zconf->getSection("zdl.save");
	QStringList pwads;
	QStringList dhacked;
	if (section){
		QVector<ZDLLine*> fileVctr;
		section->getRegex("^file[0-9]+$", fileVctr);

		if (fileVctr.size() > 0){
			for(int i = 0; i < fileVctr.size(); i++){
				if(fileVctr[i]->getValue().endsWith(".deh",Qt::CaseInsensitive) || fileVctr[i]->getValue().endsWith(".bex",Qt::CaseInsensitive)){
					dhacked << fileVctr[i]->getValue();
				}else{
					pwads << fileVctr[i]->getValue();
				}
			}
		}
	}

	if(pwads.size() > 0){
		ourString << "-file";
		ourString << pwads;
	}

	if(dhacked.size() > 0){
		ourString << "-deh";
		ourString << dhacked;
	}

	if(zconf->hasValue("zdl.save","gametype")){
		QString tGameType = zconf->getValue("zdl.save","gametype",&stat);
		if(tGameType != "0"){
			if (tGameType == "2"){
				ourString << "-deathmath";
			}
			int players = 0;
			if(zconf->hasValue("zdl.save","players")){
				QString tPlayers = zconf->getValue("zdl.save","players",&stat);
				players = tPlayers.toInt(&ok, 10);
			}
			if(players > 0){
				ourString << "-host";
				ourString << QString::number(players);
			}else if(players == 0){
				if(zconf->hasValue("zdl.save","host")){
					ourString << "-join";
					ourString << zconf->getValue("zdl.save","host",&stat);
				}
			}
			if(zconf->hasValue("zdl.save","fraglimit")){
				ourString << "+set";
				ourString << "fraglimit";
				ourString << zconf->getValue("zdl.save","fraglimit",&stat);

			}
		}
	}



	if(zconf->hasValue("zdl.net","advenabled")){
		QString aNetEnabled = zconf->getValue("zdl.net","advenabled",&stat);
		if(aNetEnabled == "enabled"){
			if(zconf->hasValue("zdl.net","port")){
				ourString << "-port";
				ourString << zconf->getValue("zdl.net","port",&stat);
			}
			if(zconf->hasValue("zdl.net","extratic")){
				QString tExtratic = zconf->getValue("zdl.net","extratic",&stat);
				if(tExtratic == "enabled"){
					ourString << "-extratic";
				}
			}
			if(zconf->hasValue("zdl.net","netmode")){
				QString tNetMode = zconf->getValue("zdl.net","netmode",&stat);
				if(tNetMode == "1"){
					ourString << "-netmode";
					ourString << "0";
				}else if(tNetMode == "2"){
					ourString << "-netmode";
					ourString << "1";
				}
			}
			if(zconf->hasValue("zdl.net","dup")){
				QString tDup = zconf->getValue("zdl.net","dup",&stat);
				if(tDup != "0"){
					ourString << "-dup";
					ourString << tDup;
				}
			}
		}
	}
	if(zconf->hasValue("zdl.general","quotefiles")){
		int ok;
		QString rc = zconf->getValue("zdl.general","quotefiles",&ok);
		if(rc == "disabled"){
			doquotes = 0;
		}
	}
	if(doquotes){
		for(int i = 0; i < ourString.size(); i++){
			if(ourString[i].contains(" ")){
				QString newString = QString("\"") + ourString[i] + QString("\"");
				ourString[i] = newString;
			}
		}
	}

	if (zconf->hasValue("zdl.general", "alwaysadd")){
		ourString << zconf->getValue("zdl.general", "alwaysadd", &stat);
	}

	if (zconf->hasValue("zdl.save", "extra")){
		ourString << zconf->getValue("zdl.save", "extra", &stat);
	}
	LOGDATAO() << "args: " << ourString << endl;
	return ourString;
}

QString ZDLMainWindow::getExecutable(){
	LOGDATAO() << "Getting exec" << endl;
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	int stat;
	QString portName = "";
	if(zconf->hasValue("zdl.save", "port")){
		ZDLSection *section = zconf->getSection("zdl.ports");
		portName = zconf->getValue("zdl.save", "port", &stat);
		QVector<ZDLLine*> fileVctr;
		section->getRegex("^p[0-9]+n$", fileVctr);

		for(int i = 0; i < fileVctr.size(); i++){
			ZDLLine *line = fileVctr[i];
			if(line->getValue().compare(portName) == 0){
				QString var = line->getVariable();
				if(var.length() >= 3){
					var = var.mid(1,var.length()-2);
					QVector<ZDLLine*> nameVctr;
					var = QString("p") + var + QString("f");
					section->getRegex("^" + var + "$",nameVctr);
					if(nameVctr.size() == 1){
						LOGDATAO() << "Executable: " << nameVctr[0]->getValue() << endl;
						return QString(nameVctr[0]->getValue());
					}
				}
			}
		}
	}
	LOGDATAO() << "No executable" << endl;
	return QString("");
}


//Pass through functions.
void ZDLMainWindow::startRead(){
	LOGDATAO() << "Starting to read configuration" << endl;
	intr->startRead();
	settings->startRead();
	QString windowTitle = getWindowTitle();
	setWindowTitle(windowTitle);
}

void ZDLMainWindow::writeConfig(){
	LOGDATAO() << "Writing configuration" << endl;
	intr->writeConfig();
	settings->writeConfig();
}
