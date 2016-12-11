// (C) Copyright Gert-Jan de Vos and Jan Wilmans 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// Repository at: https://github.com/djeedjay/DebugViewPP/

#include "stdafx.h"
#include "CobaltFusion/stringbuilder.h"
#include "DebugView++Lib/LogSource.h"
#include "DebugView++Lib/ProcessInfo.h"
#include "DebugView++Lib/NewlineFilter.h"

namespace fusion {
namespace debugviewpp {

Lines NewlineFilter::Process(const Line& line)
{
	Lines lines;
	auto& message = m_lineBuffers[line.pid];
	message.reserve(4000);

	Line outputLine = line;
	for (auto c : line.message)
	{
		if (c == '\r')
			continue;

		if (c == '\n')
		{
			outputLine.message = message;
			message.clear();
			lines.push_back(outputLine);
		}
		else
		{
			message.push_back(c);
		}
	}

	// this appears now needed to for OutputDebugString messages, but causes empty lines to disappear from tailed-files
	// indicating this we have a bug somewhere else, removing this will cause DebugView++Test to fail.
	if (message.empty())
	{
		m_lineBuffers.erase(line.pid);
	}
	else if (outputLine.pLogSource->GetAutoNewLine() || message.size() > 8192)	// 8k line limit prevents stack overflow in handling code 
	{
		outputLine.message = message;
		message.clear();
		lines.push_back(outputLine);
	}
	return lines;
}

Lines NewlineFilter::FlushLinesFromTerminatedProcess(DWORD pid, HANDLE handle)
{
	Lines lines;
	if (m_lineBuffers.find(pid) != m_lineBuffers.end())
	{
		if (!m_lineBuffers[pid].empty())
		{
			// timestamp not filled, this will be done by the loopback source
			lines.push_back(Line(0, FILETIME(), pid, "<flush>", m_lineBuffers[pid], nullptr));
		}
		m_lineBuffers.erase(pid);
	}
	auto processName = Str(ProcessInfo::GetProcessName(handle)).str();
	auto info = ProcessInfo::GetProcessInfo(handle);
	std::string infoStr = stringbuilder() << "<process started at " << info << " has now terminated>";
	lines.push_back(Line(0, FILETIME(), pid, processName, infoStr, nullptr));
	return lines;
}

} // namespace debugviewpp 
} // namespace fusion
