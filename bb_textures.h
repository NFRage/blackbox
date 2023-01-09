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

/*
 * ddsCaps field is valid.
 */
#define DDSD_CAPS               0x00000001l
#define DDSD_HEIGHT             0x00000002l
#define DDSD_WIDTH              0x00000004l
#define DDSD_PITCH              0x00000008l
#define DDSD_BACKBUFFERCOUNT    0x00000020l
#define DDSD_ZBUFFERBITDEPTH    0x00000040l
#define DDSD_ALPHABITDEPTH      0x00000080l
#define DDSD_LPSURFACE          0x00000800l
#define DDSD_PIXELFORMAT        0x00001000l
#define DDSD_CKDESTOVERLAY      0x00002000l
#define DDSD_CKDESTBLT          0x00004000l
#define DDSD_CKSRCOVERLAY       0x00008000l
#define DDSD_CKSRCBLT           0x00010000l
#define DDSD_MIPMAPCOUNT        0x00020000l
#define DDSD_REFRESHRATE        0x00040000l
#define DDSD_LINEARSIZE         0x00080000l
#define DDSD_TEXTURESTAGE       0x00100000l
#define DDSD_FVF                0x00200000l
#define DDSD_SRCVBHANDLE        0x00400000l
#define DDSD_DEPTH              0x00800000l
#define DDSD_ALL                0x00fff9eel
#define DDSCAPS_TEXTURE                         0x00001000l
#define FOURCC_DXT1       0x31545844
#define FOURCC_DXT3       0x33545844
#define FOURCC_DXT5       0x35545844

struct DDS_PIXELFORMAT {
	std::uint32_t dwSize;
	std::uint32_t dwFlags;
	std::uint32_t dwFourCC;
	std::uint32_t dwRGBBitCount;
	std::uint32_t dwRBitMask;
	std::uint32_t dwGBitMask;
	std::uint32_t dwBBitMask;
	std::uint32_t dwABitMask;
};

typedef struct {
	std::uint32_t           dwSize;
	std::uint32_t           dwFlags;
	std::uint32_t           dwHeight;
	std::uint32_t           dwWidth;
	std::uint32_t           dwPitchOrLinearSize;
	std::uint32_t           dwDepth;
	std::uint32_t           dwMipMapCount;
	std::uint32_t           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	std::uint32_t           dwCaps;
	std::uint32_t           dwCaps2;
	std::uint32_t           dwCaps3;
	std::uint32_t           dwCaps4;
	std::uint32_t           dwReserved2;
} DDS_HEADER;

enum class ENFSTextureFileFormat : std::uint32_t
{
	Unknown,
	DDS,
	PNG,
	JPG
};

enum class ENFSTextureFormat : std::uint32_t
{
	Unknown,
	BC1,
	BC2,
	BC3,

	F16,
	RGBA8,
	RG8,
	R8,

	PNG
};

inline const char* GetNFSFormatString(ENFSTextureFormat format)
{
	switch (format) {
	case ENFSTextureFormat::BC1:
		return "BC1";
	case ENFSTextureFormat::BC2:
		return "BC2";
	case ENFSTextureFormat::BC3:
		return "BC3";
    default:
        break;
	}

	return "Unknown version";
}

inline ENFSTextureFormat GetNFSFormatFromCompressionType(char compressionType)
{
	switch (compressionType) {
	case 36:
		return ENFSTextureFormat::BC2;
	case 38:
		return ENFSTextureFormat::BC3;
	default:
		break;
	}

	return ENFSTextureFormat::Unknown;
}

inline ENFSTextureFormat GetNFSFormatFromFCC(std::uint32_t fourCC)
{
	switch (fourCC) {
	case 0x1A200152:
		return ENFSTextureFormat::BC1;
	case 0x1A200153:
		return ENFSTextureFormat::BC2;
	case 0x1A200154:
		return ENFSTextureFormat::BC3;
	default:
		break;
	}

	return ENFSTextureFormat::Unknown;
}

namespace bb
{

struct TextureInformation
{
	ENFSTextureFormat Format;
	std::int16_t Width;
	std::int16_t Height;
	std::int16_t MipLevels;
	std::int16_t Alignment;
};

class TextureConverter
{
public:
	static bool UntileXenonTexture(std::int32_t width, std::int32_t height, std::int32_t blockSize, const char* xenonData, char* pcData);

	// rawData - directly loaded texture from DDS file (without decoding)
	static bool LoadTextureFromDDSFile(nfr::api::IStream* file, std::vector<char>& rawData, TextureInformation& outInformation);

	// rawData - decoded image from file
	static bool LoadRawTextureFromFile(nfr::api::IStream* file, std::vector<char>& rawData, TextureInformation& outInformation);

	static bool DecodeTexture(std::int32_t width, std::int32_t height, std::int32_t blockSize, ENFSTextureFormat formatToDecode, const char* codedData, std::size_t codedDataSize, std::vector<char>& rawData, ENFSTextureFormat& outFormat);

	static bool EncodeTexture(std::int32_t width, std::int32_t height, std::int32_t mipLevels, ENFSTextureFormat srcFormat, ENFSTextureFormat dstFormat, const char* rawData, std::size_t rawDataSize, std::vector<char>& encodedData);
	static bool EncodeTextureToFile(std::int32_t width, std::int32_t height, std::int32_t mipLevels, ENFSTextureFormat srcFormat, ENFSTextureFormat dstFormat, const char* rawData, std::size_t rawDataSize, nfr::api::IStream* encodedFile);
};

}
