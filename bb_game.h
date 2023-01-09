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
#include <filesystem>

namespace bb
{
	enum class EGameVersion
	{
		Unknown,
		HotPursuit2,
		Underground,
		Underground2,
		MostWanted,
		Carbon,
		ProStreetXenon,
		ProStreetPC,
		Undercover,
		Nitro,
		World
	};

	inline const char* GetNFSString(EGameVersion version)
	{
		switch (version)
		{
		case EGameVersion::Unknown:
			return "Unknown";
		case EGameVersion::HotPursuit2:
			return "Need For Speed Hot Pursuit 2";
		case EGameVersion::Underground:
			return "Need For Speed Underground";
		case EGameVersion::Underground2:
			return "Need For Speed Underground 2";
		case EGameVersion::MostWanted:
			return "Need For Speed Most Wanted";
		case EGameVersion::Carbon:
			return "Need For Speed Carbon";
		case EGameVersion::ProStreetXenon:
			return "Need For Speed ProStreet (PC)";
		case EGameVersion::ProStreetPC:
			return "Need For Speed ProStreet (Xbox 360)";
		case EGameVersion::Undercover:
			return "Need For Speed Undercover";
		case EGameVersion::Nitro:
			return "Need For Speed Nitro";
		case EGameVersion::World:
			return "Need For Speed World";
		default:
			break;
		}

		return "Unknown version";
	}

	EGameVersion DetectNFSGameVersionFromEntries(std::vector<aFileDirectoryEntry>& entries);
	EGameVersion DetectNFSGameVersionFromFiles();
}