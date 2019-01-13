/*
 * This file is part of qZDL
 * Copyright (C) 2007-2010  Cody Harris
 * Copyright (C) 2018  Lcferrum
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

#include "libwad.h"

struct wadinfo_t {
	char identification[4];		                 
	int numlumps;
	int infotableofs;
};

struct filelump_t {
	int	filepos;
	int	size;
	char name[8];
};

DoomWad::DoomWad(const QString &file):
	map_names(), file(file)
{}

bool DoomWad::open(){
	QFile wad(file);
	map_names.clear();

	if (wad.open(QIODevice::ReadOnly)) {
		wadinfo_t header;

		if (wad.read((char*)&header, sizeof(wadinfo_t))==sizeof(wadinfo_t)&&wad.seek(header.infotableofs)) {
			filelump_t *fileinfo=new filelump_t[header.numlumps];
			size_t length=sizeof(filelump_t)*header.numlumps;

			if (wad.read((char*)fileinfo, length)==length) {
				char prev_lump_name[9]={};

				for (int i=0; i<header.numlumps; i++) {
					if (!strncmp(fileinfo[i].name, "THINGS", 8)||!strncmp(fileinfo[i].name, "TEXTMAP", 8)) {
						if (prev_lump_name[0]) {
							map_names<<QString(prev_lump_name).toUpper();
							prev_lump_name[0]='\0';
						}
					} else {
						strncpy(prev_lump_name, fileinfo[i].name, 8);
					}
				}
			}

			delete[] fileinfo;
		}

		wad.close();
	}

	return map_names.length();
}

QStringList DoomWad::getMapNames() {
	return map_names;
}

bool DoomWad::isCompressed() {
	return false;
}
