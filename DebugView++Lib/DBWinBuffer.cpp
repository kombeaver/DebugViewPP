// (C) Copyright Gert-Jan de Vos and Jan Wilmans 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// Repository at: https://github.com/djeedjay/DebugViewPP/

#include "stdafx.h"
#include "Win32Lib/Win32Lib.h"
#include "DebugView++Lib/DBWinBuffer.h"

namespace fusion {
namespace debugviewpp {

bool IsWindowsVistaOrGreater()
{
	OSVERSIONINFO osvi = {0};
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	return (osvi.dwMajorVersion > 5);
}

bool IsDBWinViewerActive()
{
	Win32::Handle hMap(::OpenFileMapping(FILE_MAP_READ, false, L"DBWIN_BUFFER"));
	return hMap != nullptr;
}

bool HasGlobalDBWinReaderRights()
{
	//if (IsWindowsVistaOrGreater())
	{
		Win32::Handle hMap(::CreateFileMapping(nullptr, nullptr, PAGE_READWRITE, 0, sizeof(DbWinBuffer), L"Global\\DBWIN_BUFFER"));
		return hMap != nullptr;
	}
	return false;
}

} // namespace debugviewpp 
} // namespace fusion
