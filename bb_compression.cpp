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
#include "blackbox_pch.h"

#ifdef NFRAGE_TOOLS
// Thanks UEViewer for code
struct XboxPackHandle
{
	std::vector<std::uint8_t>& data;
	int	pos = 0;
	int rest = 0;

	XboxPackHandle(std::vector<std::uint8_t>& inData)
		: data(inData) {}
};

static int mspack_read(mspack_file* inFile, void* buffer, int bytes)
{
	XboxPackHandle* file = reinterpret_cast<XboxPackHandle*>(inFile);
	if (!file->rest)
	{
		// read block header
		if (file->data[file->pos] == 0xFF)
		{
			// [0]   = FF
			// [1,2] = uncompressed block size
			// [3,4] = compressed block size
			file->rest = (file->data[file->pos + 3] << 8) | file->data[file->pos + 4];
			file->pos += 5;
		}
		else
		{
			// [0,1] = compressed size
			file->rest = (file->data[file->pos + 0] << 8) | file->data[file->pos + 1];
			file->pos += 2;
		}
		if (file->rest > file->data.size() - file->pos)
			file->rest = file->data.size() - file->pos;
	}
	if (bytes > file->rest) bytes = file->rest;
	if (bytes <= 0) return 0;

	// copy block data
	memcpy(buffer, file->data.data() + file->pos, bytes);
	file->pos += bytes;
	file->rest -= bytes;

	return bytes;
}

static int mspack_write(mspack_file* inFile, void* buffer, int bytes)
{
	XboxPackHandle* file = reinterpret_cast<XboxPackHandle*>(inFile);
	assert(file->pos + bytes <= file->data.size());
	memcpy(file->data.data() + file->pos, buffer, bytes);
	file->pos += bytes;
	return bytes;
}

static void* mspack_alloc(mspack_system* self, size_t bytes)
{
	return malloc(bytes);
}

static void mspack_free(void* ptr)
{
	free(ptr);
}

static void mspack_copy(void* src, void* dst, size_t bytes)
{
	memcpy(dst, src, bytes);
}

static struct mspack_system lzxSys =
{
	NULL,				// open
	NULL,				// close
	mspack_read,
	mspack_write,
	NULL,				// seek
	NULL,				// tell
	NULL,				// message
	mspack_alloc,
	mspack_free,
	mspack_copy
};
#endif

namespace bb
{

bool Decompress(void* data, std::uint32_t uncompressedSize, std::uint32_t header, bool bLZ)
{
	return false;
}

bool DecompressXbox(std::vector<std::uint8_t>& inputData, std::vector<std::uint8_t>& outputData)
{
#ifdef NFRAGE_TOOLS
	// setup streams
	XboxPackHandle src(inputData);
	XboxPackHandle dst(outputData);

	uint32_t uncompressedSize = 0;

	// prepare decompressor
	lzxd_stream* lzxd = lzxd_init(
		&lzxSys, 
		reinterpret_cast<mspack_file*>(&src), 
		reinterpret_cast<mspack_file*>(&dst), 
		17, 
		0, 
		256 * 1024, 
		uncompressedSize,
		1
	);

	if (lzxd == nullptr) {
		dbg::Warning("Can't open decompressor for xbox file.");
		return false;
	}

	// decompress
	int r = lzxd_decompress(lzxd, uncompressedSize);
	if (r != MSPACK_ERR_OK) {
		dbg::Warning("Can't unpack file with lzxd_decompress (result is {})", r);
		return false;
	}

	// free resources
	lzxd_free(lzxd);
	return true;
#else
	return false;
#endif 
}

}
