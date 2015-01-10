/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2015 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef EQEMU_LOGSYS_H
#define EQEMU_LOGSYS_H

#include <iostream>
#include <fstream> 
#include "types.h"

class EQEmuLogSys {
public:
	EQEmuLogSys();
	~EQEmuLogSys();

	enum LogType {
		Status = 0,	 /* This must stay the first entry in this list */
		Normal,		 /* Normal Logs */
		Error,		 /* Error Logs */
		Debug,		 /* Debug Logs */
		Quest,		 /* Quest Logs */
		Commands,	 /* Issued Comamnds */
		Crash,		 /* Crash Logs */
		Save,		 /* Client Saves */
		MaxLogID	 /* Max, used in functions to get the max log ID */
	};

	void StartZoneLogs(const std::string log_name);
	void WriteZoneLog(uint16 log_type, const std::string message);
	void CloseZoneLogs();
	void ConsoleMessage(uint16 log_type, const std::string message);

private:
	bool zone_general_init = false;

};

extern EQEmuLogSys log_sys;

#endif