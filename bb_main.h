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
#include <algorithm>
#include <string>

namespace bb
{

struct DirectoryHandle
{
	DirectoryHandle() = default;
	DirectoryHandle(DirectoryHandle&&) = default;
	DirectoryHandle(std::vector<std::int32_t>&& inFileIndexes)
		: fileIndexes(std::move(inFileIndexes)) {}

	std::vector<std::int32_t> fileIndexes;
};

struct FileHandle
{
	FileHandle(FileHandle&&) = default;
	FileHandle(nfr::api::SafeInterface<nfr::api::IStream> inStream, std::string&& inFileName)
		: stream(std::move(inStream)), fileName(std::move(inFileName)) {}

	nfr::api::SafeInterface<nfr::api::IStream> stream;
	std::string fileName;
	std::vector<aFileDirectoryEntry> entries;
};

bool PostInitializePackedDatabase();
bool UnpackXboxData();

}

