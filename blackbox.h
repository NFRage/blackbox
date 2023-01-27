/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2022-2023. All rights reserved.
* nfrage - engine code for NFRage project
**********************************************************************
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301 USA
*****************************************************************/
#pragma once

#ifdef _WIN32
#ifdef DLL_PLATFORM
#ifdef BLACKBOX_EXPORTS
#define BLACKBOX_PLUGIN_API __declspec(dllexport)
#else
#define BLACKBOX_PLUGIN_API __declspec(dllimport)
#endif
#else
#define BLACKBOX_PLUGIN_API
#endif
#else
#ifdef LIB_EXPORTS
#define BLACKBOX_PLUGIN_API __attribute__((__visibility__("default")))
#else
#define BLACKBOX_PLUGIN_API 
#endif
#endif

#include <nfrage_api.h>

extern "C"
{
	BLACKBOX_PLUGIN_API nfr::api::IPluginFactory* CreatePluginFactory(nfr::api::IEngineFactory* engineFactory);
};

extern BLACKBOX_PLUGIN_API spdlog::logger* GameLogger;
extern BLACKBOX_PLUGIN_API nfr::api::IEngineFactory* EngineFactory;

namespace bb::dbg
{
	template<typename... Args>
	inline void Verbose(const nfr::api::string_view& fmt, Args&&... args)
	{
		GameLogger->trace(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	inline void Log(const nfr::api::string_view& fmt, Args&&... args)
	{
		GameLogger->info(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	inline void Error(const nfr::api::string_view& fmt, Args&&... args)
	{
		GameLogger->error(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	inline void Warning(const nfr::api::string_view& fmt, Args&&... args)
	{
		GameLogger->warn(fmt, std::forward<Args>(args)...);
	}
}