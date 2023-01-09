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

struct XCompressDecodeHeader
{
	std::uint32_t Identifier;
	std::uint16_t Version;
	std::uint16_t Reserved;
	std::uint32_t CRC_Hash;
	std::uint32_t Flags;
};

struct XCompressNativeHeader
{
	std::uint32_t Identifier;
	std::uint16_t Version;
	std::uint16_t Reserved;
	std::uint32_t ContextFlags;
	std::uint32_t Flags;
	std::uint32_t WindowSize;
	std::uint32_t CompressionPartitionSize;
	std::uint32_t UncompressedSizeHigh;
	std::uint32_t UncompressedSizeLow;
	std::uint32_t CompressedSizeHigh;
	std::uint32_t CompressedSizeLow;
	std::uint32_t UncompressedBlockSize;
	std::uint32_t CompressedBlockSizeMax;
};

enum class EBigFilesCompression
{
	None,
	XbCompressDecode,
	XbCompressNative
};

namespace bb
{

bool Decompress(void* data, std::uint32_t uncompressedSize, std::uint32_t header, bool bLZ);
#ifdef SPEED_TOOLS
bool DecompressXbox(std::vector<std::uint8_t>& inputData, std::vector<std::uint8_t>& outputData);
#endif

}

