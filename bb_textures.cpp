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
#define STB_DXT_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define BCDEC_IMPLEMENTATION
#include "stb/stb_dxt.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "bcdec/bcdec.h"

namespace bb
{ 

bool
TextureConverter::UntileXenonTexture(
	std::int32_t width,
	std::int32_t height,
	std::int32_t blockSize,
	const char* xenonData,
	char* pcData
)
{
	auto GetXenonTileOffset = [](std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t logBpb) {
		int alignedWidth = ALIGN_VALUE(width, 32);
		int macro = ((x >> 5) + (y >> 5) * (alignedWidth >> 5)) << (logBpb + 7);
		int micro = ((x & 7) + ((y & 0xE) << 2)) << logBpb;
		int offset = macro + ((micro & ~0xF) << 1) + (micro & 0xF) + ((y & 1) << 4);
		return (((offset & ~0x1FF) << 3) +
			((y & 16) << 7) +
			((offset & 0x1C0) << 2) +
			(((((y & 8) >> 2) + (x >> 3)) & 3) << 6) +
			(offset & 0x3F)
		) >> logBpb;
	};

	auto intLog2 = [](int n) {
		int r;
		for (r = -1; n; n >>= 1, r++) {

		}

		return r;
	};

	const std::int32_t textureAlign = 128;
	const std::int32_t alignedWidth = ALIGN_VALUE(width, textureAlign);
	const std::int32_t alignedHeight = ALIGN_VALUE(height, textureAlign);
	const std::int32_t tiledBlockWidth = alignedWidth / 4;
	const std::int32_t originalBlockWidth = width / 4;
	const std::int32_t tiledBlockHeight = alignedHeight / 4;
	const std::int32_t originalBlockHeight = height / 4;
	const std::int32_t logBpp = intLog2(blockSize);

	std::int32_t sxOffset = 0;
	std::int32_t syOffset = 0;
	if ((tiledBlockWidth >= originalBlockWidth * 2) && (width == 16)) {
		sxOffset = originalBlockWidth;
	}

	if ((tiledBlockHeight >= originalBlockHeight * 2) && (height == 16)) {
		syOffset = originalBlockHeight;
	}

	std::int32_t numImageBlocks = tiledBlockWidth * tiledBlockHeight;
	int bytesPerBlock = blockSize;
	for (int dy = 0; dy < originalBlockHeight; dy++) {
		for (int dx = 0; dx < originalBlockWidth; dx++) {
			std::uint32_t swzAddr = GetXenonTileOffset(dx + sxOffset, dy + syOffset, tiledBlockWidth, logBpp);
			assert(swzAddr < numImageBlocks);
			std::int32_t sy = swzAddr / tiledBlockWidth;
			std::int32_t sx = swzAddr % tiledBlockWidth;

			char* pDst = pcData + (dy * originalBlockWidth + dx) * bytesPerBlock;
			const char* pSrc = xenonData + (sy * tiledBlockWidth + sx) * bytesPerBlock;
			std::memcpy(pDst, pSrc, bytesPerBlock);
		}
	}

	return true;
}

bool
TextureConverter::DecodeTexture(
	std::int32_t width,
	std::int32_t height,
	std::int32_t blockSize,
	ENFSTextureFormat formatToDecode,
	const char* codedData,
	std::size_t codedDataSize,
	std::vector<char>& rawData,
	ENFSTextureFormat& outFormat
)
{
	switch (formatToDecode) {
	case ENFSTextureFormat::BC1:
	case ENFSTextureFormat::BC2:
	case ENFSTextureFormat::BC3: {
		const std::int32_t blockSize = formatToDecode == ENFSTextureFormat::BC1 ? BCDEC_BC1_BLOCK_SIZE : BCDEC_BC3_BLOCK_SIZE;
		std::uint64_t blockOffset = 0;
		std::uint64_t textureOffset = 0;

		// BC block contains 16 pixels in 16 bytes, so in unpacked way
		// we're getting 64 bytes on raw RGBA data
		rawData.resize(codedDataSize * 4);

		for (std::uint32_t y = 0; y < height / 4; y++) {
			for (std::uint32_t x = 0; x < width / 4; x++) {
				const char* shiftedSrcPtr = codedData + textureOffset;
				char* shiftedDstPtr = rawData.data() + (y * width + x) * 4 * 4;
				switch (formatToDecode) {
				case ENFSTextureFormat::BC1:
					bcdec_bc1(shiftedSrcPtr, shiftedDstPtr, width * 4 /* RGBA */);
					break;
				case ENFSTextureFormat::BC2:
					bcdec_bc2(shiftedSrcPtr, shiftedDstPtr, width * 4 /* RGBA */);
					break;
				case ENFSTextureFormat::BC3:
					bcdec_bc3(shiftedSrcPtr, shiftedDstPtr, width * 4 /* RGBA */);
					break;
				default:
					break;
				}

				textureOffset += blockSize;
			}
		}

		outFormat = ENFSTextureFormat::RGBA8;
	}
							   break;

	default:
		break;
	}

	return true;
}

std::uint32_t GetTextureBlockSize(ENFSTextureFormat textureFormat)
{
	switch (textureFormat)
	{
	case ENFSTextureFormat::Unknown:
		break;
	case ENFSTextureFormat::BC1:
		return BCDEC_BC1_BLOCK_SIZE;
	case ENFSTextureFormat::BC2:
		return BCDEC_BC2_BLOCK_SIZE;
	case ENFSTextureFormat::BC3:
		return BCDEC_BC3_BLOCK_SIZE;
    case ENFSTextureFormat::F16:
        return 2;
    case ENFSTextureFormat::RGBA8:
        return 4;
    case ENFSTextureFormat::R8:
        return 1;
    case ENFSTextureFormat::RG8:
        return 2;
    default:
        break;
	}

	return 0;
}

TextureInformation GetTextureInfoFromDDS(const DDS_HEADER& ddsHeader)
{
	TextureInformation outInformation = {};

	switch (ddsHeader.ddspf.dwFourCC) {
	case FOURCC_DXT1:
		outInformation.Format = ENFSTextureFormat::BC1;
		break;
	case FOURCC_DXT3:
		outInformation.Format = ENFSTextureFormat::BC2;
		break;
	case FOURCC_DXT5:
		outInformation.Format = ENFSTextureFormat::BC3;
		break;
	default:
		break;
	}

	outInformation.Width = ddsHeader.dwWidth;
	outInformation.Height = ddsHeader.dwHeight;
	outInformation.MipLevels = ddsHeader.dwMipMapCount;
	return outInformation;
}

std::uint64_t CalculateTextureSize(ENFSTextureFormat format, std::uint32_t width, std::uint32_t height, std::uint32_t mipLevels)
{
	std::uint64_t outTextureSize = 0;
	std::uint64_t baseTextureSize = 0;

	switch (format) {
	case ENFSTextureFormat::BC1:
		baseTextureSize = BCDEC_BC1_COMPRESSED_SIZE(width, height);
		break;
	case ENFSTextureFormat::BC2:
		baseTextureSize = BCDEC_BC2_COMPRESSED_SIZE(width, height);
		break;
	case ENFSTextureFormat::BC3:
		baseTextureSize = BCDEC_BC3_COMPRESSED_SIZE(width, height);
		break;
	case ENFSTextureFormat::F16:
		baseTextureSize = width * height * sizeof(short);
		break;
	case ENFSTextureFormat::RGBA8:
		baseTextureSize = width * height * sizeof(std::uint32_t);
		break;
	case ENFSTextureFormat::RG8:
		baseTextureSize = width * height * sizeof(std::uint16_t);
		break;
	case ENFSTextureFormat::R8:
		baseTextureSize = width * height * sizeof(std::uint8_t);
		break;
	default:
		break;
	}

	for (std::uint32_t i = 0; i < mipLevels; i++) {
		outTextureSize += baseTextureSize / (i + 1);
	}

	return outTextureSize;
}

bool
TextureConverter::LoadTextureFromDDSFile(
	nfr::api::IStream* file,
	std::vector<char>& rawData,
	TextureInformation& outInformation
)
{
	if (file->getSize() < 128) {
		dbg::Warning("File can't contain DDS header because it's smaller than 128 bytes.");
		return false;
	}

	DDS_HEADER ddsHeader = {};
	file->read(&ddsHeader, sizeof(DDS_HEADER));
	outInformation = GetTextureInfoFromDDS(ddsHeader);

	rawData.resize(CalculateTextureSize(outInformation.Format, outInformation.Width, outInformation.Height, outInformation.MipLevels));
	file->read(rawData.data(), rawData.size());

	return true;
}

bool
TextureConverter::LoadRawTextureFromFile(
	nfr::api::IStream* file,
	std::vector<char>& rawData,
	TextureInformation& outInformation
)
{
	ENFSTextureFormat inFormat = ENFSTextureFormat::Unknown;
	ENFSTextureFileFormat fileFormat = ENFSTextureFileFormat::Unknown;
	char magicWord[4] = {};

	file->read(magicWord, 4);
	if (!memcmp(magicWord, "DDS ", 4)) {
		fileFormat = ENFSTextureFileFormat::DDS;
	}

	if (fileFormat == ENFSTextureFileFormat::Unknown) {
		dbg::Warning("Can't detected file format.");
		return false;
	}

	switch (fileFormat) {
	case ENFSTextureFileFormat::DDS: {
		DDS_HEADER ddsHeader = {};
		char readBuffer[4096] = {};
		file->read(&ddsHeader, sizeof(DDS_HEADER));
		outInformation = GetTextureInfoFromDDS(ddsHeader);

		std::uint32_t blockSize = GetTextureBlockSize(outInformation.Format);
		std::uint64_t readedSize = 0;

		rawData.resize(CalculateTextureSize(ENFSTextureFormat::RGBA8, outInformation.Width, outInformation.Height, outInformation.MipLevels));

		std::uint32_t texturePosition = 0;
		while (!file->isEndOfFile()) {
			readedSize = file->read(readBuffer, 4096);
			if (readedSize == 0) {
				break;
			}

			for (std::uint32_t i = 0; i < readedSize; i += blockSize) {
				switch (inFormat) {
				case ENFSTextureFormat::BC1:
					bcdec_bc1(&readBuffer[i], &rawData[texturePosition], outInformation.Width * 4);
					break;
				case ENFSTextureFormat::BC2:
					bcdec_bc2(&readBuffer[i], &rawData[texturePosition], outInformation.Width * 4);
					break;
				case ENFSTextureFormat::BC3:
					bcdec_bc3(&readBuffer[i], &rawData[texturePosition], outInformation.Width * 4);
					break;
				default:
					break;
				}

				texturePosition += blockSize;
			}
		}
	}
								   break;

	case ENFSTextureFileFormat::PNG:
		break;
	case ENFSTextureFileFormat::JPG:
		break;
	default:
		break;
	}

	return true;
}

bool
TextureConverter::EncodeTexture(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipLevels,
	ENFSTextureFormat srcFormat,
	ENFSTextureFormat dstFormat,
	const char* rawData,
	std::size_t rawDataSize,
	std::vector<char>& encodedData
)
{
    if (srcFormat == dstFormat) {
        return false;
    }
    
    
    switch (dstFormat) {
        case ENFSTextureFormat::PNG: {
            if (srcFormat != ENFSTextureFormat::RGBA8) {
                dbg::Warning("Can't convert texture to png because it's not in RGBA8 format...");
                return false;
            }
            
            int outLength = 0;
            std::uint8_t* pngData = stbi_write_png_to_mem((std::uint8_t*)rawData, width * 4, width, height, 4, &outLength);
            if (pngData == nullptr) {
                return false;
            }
            
            encodedData.resize(outLength);
            std::memcpy(encodedData.data(), pngData, outLength);
        }
        break;
            
        default:
            break;
    }
    
    return true;
}


DDS_HEADER MakeDDSHeader(ENFSTextureFormat format, std::int32_t width, std::int32_t height, std::int32_t mipMapLevels)
{
    DDS_HEADER ddsHeader = {};
    ddsHeader.dwSize = 124;
    ddsHeader.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE;
    ddsHeader.dwHeight = height;
    ddsHeader.dwWidth = width;
    ddsHeader.dwPitchOrLinearSize = std::max((std::uint32_t)1, (ddsHeader.dwWidth + 3) / 4) * std::max((std::uint32_t)1, (ddsHeader.dwHeight + 3) / 4) * 8;
    ddsHeader.dwMipMapCount = mipMapLevels;
    ddsHeader.dwCaps = DDSCAPS_TEXTURE;

    ddsHeader.ddspf.dwSize = 32;
    ddsHeader.ddspf.dwFlags = 0x4;

    switch (format) {
    case ENFSTextureFormat::BC1:
        ddsHeader.ddspf.dwFourCC = FOURCC_DXT1;
        break;
    case ENFSTextureFormat::BC2:
        ddsHeader.ddspf.dwFourCC = FOURCC_DXT3;
        break;
    case ENFSTextureFormat::BC3:
        ddsHeader.ddspf.dwFourCC = FOURCC_DXT5;
        break;
    default:
        break;
    }

    return ddsHeader;
}

bool TextureConverter::EncodeTextureToFile(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipLevels,
	ENFSTextureFormat srcFormat, 
	ENFSTextureFormat dstFormat, 
	const char* rawData,
	std::size_t rawDataSize, 
	nfr::api::IStream* encodedFile
)
{
    if (srcFormat == dstFormat) {
        switch (srcFormat) {
            case ENFSTextureFormat::BC1:
            case ENFSTextureFormat::BC2:
            case ENFSTextureFormat::BC3: {
                encodedFile->write((void*)"DDS ", 4);
                DDS_HEADER ddsHeader = MakeDDSHeader(dstFormat, width, height, mipLevels);
                encodedFile->write(&ddsHeader, sizeof(DDS_HEADER));
                encodedFile->write((void*)rawData, rawDataSize);
            }
            break;
                
            default:
                break;
        }
    } else {
        const char* dataPtr = rawData;
        std::size_t dataPtrSize = rawDataSize;
        std::vector<char> decodedData;
        
        if (srcFormat != ENFSTextureFormat::RGBA8) {
            if (!DecodeTexture(width,
                          height,
                          GetTextureBlockSize(srcFormat),
                          srcFormat,
                          rawData,
                          rawDataSize,
                          decodedData,
                          srcFormat
            )) {
                return false;
            }
            
            dataPtr = decodedData.data();
            dataPtrSize = decodedData.size();
        }
        
        std::vector<char> encodedData;
        if (!EncodeTexture(width, height, mipLevels, srcFormat, dstFormat, dataPtr, dataPtrSize, encodedData)) {
            dbg::Error("Can't encode from {} to {} format.", (std::uint32_t)srcFormat, (std::uint32_t)dstFormat);
            return false;
        }
    }
    
	return true;
}

}
