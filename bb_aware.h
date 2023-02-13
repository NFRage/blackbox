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

struct aFileDirectoryEntry
{
	std::uint32_t Hash;
	std::int32_t FileNumber;
	std::int32_t LocalSectorOffset;
	std::int32_t TotalSectorOffset;
	std::int32_t Size;
	std::uint32_t Checksum;
};

struct aChunk
{
	std::uint32_t Id;
	std::int32_t Size;

	char* getDataPtr() const
	{
		return (char*)this + sizeof(aChunk);
	}

	template<typename T>
	T* getDataPtr() const
	{
		return (T*)getDataPtr();
	}

	template<typename T>
	T* getAlignedPtr(std::ptrdiff_t align) const
	{
		return (T*)(((std::ptrdiff_t)getDataPtr() + (align - 1)) & (~(align - 1)));
	}

	std::size_t getSize() const
	{
		return Size;
	}

	std::size_t getAlignedSize(std::size_t align) const
	{
		return (getDataPtr() - getAlignedPtr<char>(align)) + Size;
	}
};