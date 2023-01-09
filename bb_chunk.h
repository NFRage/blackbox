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

namespace bb
{
	bool LoadChunkedFile(const char* filePath);
    bool LoadCompressedFile(const char* filePath, bool saveUncompressedFile = false);

	bool ProcessChunk(aChunk* chunkData);
	bool ProcessTexturePackChunk(aChunk* chunkData);
	aChunk* ProcessTexturePackDataChunk(aChunk* chunkData);
	bool ProcessTexturePackHeaderChunk(aChunk* chunkData, TexturePackHeader& outHeader, TexturePlatInfo& outPlatInfo, std::vector<TextureInfo>& texturesInfo);
	void ProcessTextureLoadAnimationChunk(aChunk* anumChunk);
}
