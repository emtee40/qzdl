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
 
#ifndef _ZDLCOMMON_H_
#define _ZDLCOMMON_H_
#include <vector>
#include "zdlcommon.h"
#include "zdlline.hpp"
#include "zdlsection.hpp"
#include "zdlconf.hpp"
#include "zdlvariables.hpp"
using namespace std;
#define ZDL_FLAG_NAMELESS	0x00001

#ifdef Q_WS_WIN

#define ZDL_VERSION_STRING	"3.0.5.3q"
#define ZDL_VERSION_ID		23

#else

#define ZDL_VERSION_STRING	"3.0.5.3q"
#define ZDL_VERSION_ID		20

#endif

#define ZDL_UDPATE_SERVER	"update.vectec.net"
#define ZDL_ENGINE_NAME		"ZDLSharp"
#define ZDL_UID				"XXXX-XXXX-XXXX-XXXX"

extern QDebug *zdlDebug;


#if defined(ZDL_BLACKBOX)
#include <QtCore>
#define LOGDATA() (*zdlDebug) << (QDateTime::currentDateTime().toString("[yyyy:MM:dd/hh:mm:ss.zzz]").append("@").append(__PRETTY_FUNCTION__).append("@").append(__FILE__).append(":").append(QString::number(__LINE__)).append("\t"))
#define LOGDATAO() (*zdlDebug) << (QDateTime::currentDateTime().toString("[yyyy:MM:dd/hh:mm:ss.zzz]").append("@").append(__PRETTY_FUNCTION__).append("@").append(__FILE__).append(":").append(QString::number(__LINE__)).append("#this=").append(DPTR(this)).append("\t"))
#if UINTPTR_MAX == 0xffffffff
#warning 32bit
#define DPTR(ptr) QString("0x").append(QString("%1").arg((qulong)ptr, 0, 8, 16)
#else
#define DPTR(ptr) QString("0x").append(QString::number((qulonglong)ptr,16))
#endif
#else
#define LOGDATA() (*zdlDebug)
#define LOGDATAO() (*zdlDebug)
#define DPTR(ptr) QString("")
#endif


#endif
