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

#ifdef _WIN32
#define BYTESWAP_SHORT(x) _byteswap_ushort(x)
#define BYTESWAP_LONG(x) _byteswap_ulong(x)
#else
#define BYTESWAP_SHORT(x) __builtin_bswap16(x)
#define BYTESWAP_LONG(x) __builtin_bswap32(x)
#endif

namespace bb
{

void ProcessTextureLoadAnimationChunk(aChunk* anumChunk)
{
	aChunk* nextChunk = anumChunk + 1;
	aChunk* lastChunk = (aChunk*)((char*)anumChunk + anumChunk->Size + 8);
	while (nextChunk != lastChunk) {
		dbg::Log("Processing anim chunk...");
		if (nextChunk->Id != static_cast<std::uint32_t>(ENFSChunkId::TPK_AnimBlock)) {
			dbg::Warning("Unexpected chunkId {:#06x} in texture animation chunk. Skipping this one...", nextChunk->Id);
			nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + 8);
			continue;
		}

		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + 8);
	}
}

bool ProcessTexturePackHeaderChunk(aChunk* chunkData, TexturePackHeader& outHeader, TexturePlatInfo& outPlatInfo, std::vector<TextureInfo>& texturesInfo)
{
	static const char verifyBuffer[12] = {};
	nfr::api::binary_hash_set hashesStorage;
	aChunk* nextChunk = chunkData + 1;
	aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + 8);

	TextureInfo* textureInfo = nullptr;
	aChunk* animChunk = nullptr;

	std::uint32_t textureIndexesCount = 0;
	std::uint32_t texturesInfoCount = 0;
	bool bEndianSwapped = false;

	while (nextChunk != lastChunk) {
		const std::string_view& chunkName = (NfsChunkIdMap.find(nextChunk->Id) != NfsChunkIdMap.end() ? NfsChunkIdMap.at(nextChunk->Id) : "");
		dbg::Log("    Processing chunk {}...", chunkName);
		ENFSChunkId currentChunk = static_cast<ENFSChunkId>(nextChunk->Id);
		if (currentChunk == ENFSChunkId::TPK_InfoPart1) {
			TexturePackHeader* texturePackHeader = (TexturePackHeader*)(nextChunk + 1);
			std::memcpy(&outHeader, texturePackHeader, sizeof(TexturePackHeader));
			dbg::Log("        Found package name {:#06x} ({})", texturePackHeader->FilenameHash, texturePackHeader->Name);
		} else {
			switch (currentChunk) {
			case ENFSChunkId::TPK_InfoPart2: {
				TextureIndexEntry* textureIndexEntry = (TextureIndexEntry*)(nextChunk + 1);
				textureIndexesCount = (nextChunk->Size / sizeof(TextureIndexEntry));
				for (std::uint32_t i = 0; i < textureIndexesCount; i++) {
					dbg::Log("        Found entry hash {:#06x} (padding: {})", textureIndexEntry->NameHash, textureIndexEntry->Padding);
					hashesStorage.insert(textureIndexEntry->NameHash);
					textureIndexEntry++;
				}

			}
			break;

			case ENFSChunkId::TPK_InfoPart3: {
				StreamingEntry* streamingEntry = (StreamingEntry*)(nextChunk + 1);
				for (std::uint32_t i = 0; i < (nextChunk->Size / sizeof(StreamingEntry)); i++) {
					dbg::Log("        Found streaming entry {:#06x}", streamingEntry->NameHash);
					streamingEntry++;
				}
			}
			break;

			case ENFSChunkId::TPK_InfoPart4: {
				textureInfo = (TextureInfo*)(nextChunk + 1);;
				texturesInfo.reserve(textureIndexesCount);
				for (std::uint32_t i = 0; i < textureIndexesCount; i++) {
					if (std::memcmp(textureInfo->BigPadding, verifyBuffer, sizeof(verifyBuffer)) != 0) {
						dbg::Warning("Something wrong with BigPadding!!! Can't process this chunk anymore (i - {})", i);
						break;
					}

					dbg::Log("        Found texture info {:#06x} ({})", textureInfo->NameHash, textureInfo->DebugName);
					texturesInfo.emplace_back(*textureInfo);
					textureInfo = (TextureInfo*)((char*)textureInfo + textureInfo->DebugNameSize + 89);
				}
			}
			break;

			case ENFSChunkId::TPK_InfoPart5: {
				TexturePlatInfo* texturePlatInfoEntry = (TexturePlatInfo*)(nextChunk + 1);
				std::memcpy(&outPlatInfo, texturePlatInfoEntry, sizeof(TexturePlatInfo));

				const std::string_view& formatName = (TexturesFormatMap.find(texturePlatInfoEntry->format) != TexturesFormatMap.end() ? TexturesFormatMap.at(texturePlatInfoEntry->format) : "");
				dbg::Log("        Found texture plat info (format: {})", formatName);
				texturesInfoCount = nextChunk->Size / 32 /* #TODO: check this one */;
			}
			break;

			case ENFSChunkId::TPK_BinData: {
				animChunk = nextChunk;
			}
			break;

			default:
				break;
			}
		}

		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + 8);
	}

	if (textureIndexesCount != texturesInfoCount) {
		return false;
	}

	if (animChunk != nullptr) {
		ProcessTextureLoadAnimationChunk(animChunk);
	}

	return true;
}

aChunk* ProcessTexturePackDataChunk(aChunk* chunkData)
{
	aChunk* nextChunk = chunkData + 1;
	aChunk* nextNextChunk = chunkData + 2;

	TextureVRAMDataHeader* vramHeader = (TextureVRAMDataHeader*)(nextChunk + 1);
	int32_t* someBlockSize = (int32_t*)((char*)&nextNextChunk->Size + nextChunk->Size);
	aChunk* dataChunk = (aChunk*)((((char*)nextChunk + nextChunk->Size) + *someBlockSize) + 16);
	if (vramHeader->EndianSwapped) {
		vramHeader->Version = BYTESWAP_LONG(vramHeader->Version);
		vramHeader->FilenameHash = BYTESWAP_LONG(vramHeader->FilenameHash);
		vramHeader->EndianSwapped = 0;
	}

	dbg::Log("        Found data chunk {:#06x}", vramHeader->FilenameHash);
	return dataChunk;
}

bool ProcessTexturePackChunk(aChunk* chunkData)
{	
	const bool isXenonPlatform = true;

	ENFSChunkId chunkId = static_cast<ENFSChunkId>(chunkData->Id);
	aChunk* dataChunk = nullptr; 
	std::vector<TextureInfo> texturesInfo;
	TexturePackHeader texturePackHeader = {};
	TexturePlatInfo texturePlatInfo = {};

	if (chunkId == ENFSChunkId::TPK_Blocks) {
		aChunk* nextChunk = chunkData + 1;
		aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + 8);
		while (nextChunk != lastChunk) {
			ENFSChunkId nextChunkId = static_cast<ENFSChunkId>(nextChunk->Id);
			if (nextChunkId == ENFSChunkId::TPK_InfoBlock) {
				ProcessTexturePackHeaderChunk(nextChunk, texturePackHeader, texturePlatInfo, texturesInfo);
			} else if (nextChunkId == ENFSChunkId::TPK_DataBlock) {
				dataChunk = ProcessTexturePackDataChunk(nextChunk);
			}

			nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + sizeof(aChunk));
		}

		if (false /* not enough space*/) {
			return false;
		}
	} else if (chunkId == ENFSChunkId::TPK_DataBlock) {
		dataChunk = ProcessTexturePackDataChunk(chunkData);
	}

	if (texturePackHeader.FilenameHash == 0) {
		dbg::Log("Procedeed to the empty chunk. Skipping the chunk");
		return true;
	}

	if (texturesInfo.empty()) {
		dbg::Warning("No textures info was found in chunk {}. Skipping the chunk", texturePackHeader.Filename);
		return false;
	}

	char* dataPtr = dataChunk->getDataPtr();	// Textures chunk is aligned by 4096 bytes
	std::size_t alignedSize = dataChunk->getSize();

	// Skipping align padding
	while ((*reinterpret_cast<std::uint32_t*>(dataPtr)) == '\x11\x11\x11\x11') {
		dataPtr += 4;
		alignedSize -= 4;
	}

	dbg::Log("    Processing textures in TPK block...");
	for (int32_t i = 0; i < texturesInfo.size(); i++) {
		const TextureInfo& textureInfo = texturesInfo[i];
		ENFSTextureFormat texFormat = GetNFSFormatFromCompressionType(textureInfo.ImageCompressionType);
        
		const char* texturePackedData = (dataPtr + textureInfo.ImagePlacement);
		const std::size_t blockSize = texFormat == ENFSTextureFormat::BC1 ? 8 : 16;

		const char* textureName = textureInfo.DebugName;
		if (EntriesMap.find(textureInfo.NameHash) != EntriesMap.end()) {
			textureName = EntriesMap.at(textureInfo.NameHash).c_str();
		}
        
        if (texFormat == ENFSTextureFormat::Unknown) {
            dbg::Warning("Unknown texture format {} in texture {}. Skipping texture...", textureInfo.ImageCompressionType, textureName);
            continue;
        }

		dbg::Log("        Loading {} ({}x{}, {}KB, {} mips, {} format)... ",
			textureName,
			textureInfo.Width,
			textureInfo.Height,
			textureInfo.ImageSize / 1024,
			textureInfo.NumMipMapLevels,
			GetNFSFormatString(texFormat)
		);
		
		std::vector<char> pcData;
		const char* pcDataPtr = texturePackedData;

		if (isXenonPlatform) {
			pcData.resize(textureInfo.ImageSize);
			pcDataPtr = pcData.data();

			for (std::uint32_t i = 0; i < textureInfo.ImageSize; i += 2) {
				std::uint16_t* ptrToSwap = (std::uint16_t*)&texturePackedData[i];
				*ptrToSwap = BYTESWAP_SHORT(*ptrToSwap);
			}

			const char* mipDataPtr = texturePackedData;
			char* mipPCDataPtr = pcData.data();
			if (!TextureConverter::UntileXenonTexture(textureInfo.Width, textureInfo.Height, blockSize, mipDataPtr, mipPCDataPtr)) {
				dbg::Warning("Can't convert {} texture from Xenon to PC format.", textureInfo.DebugName);
				return false;
			}
			/*
			for (std::uint32_t mip = 0; mip < textureInfo.NumMipMapLevels; mip++) {
				const std::int32_t textureAlign = 128;
				const std::uint32_t mipWidth = textureInfo.Width / (mip + 1);
				const std::uint32_t mipHeight = textureInfo.Height / (mip + 1);
				const std::uint32_t mipAlignedSize = ALIGN_VALUE(mipWidth, textureAlign) * ALIGN_VALUE(mipHeight, 4) / 16 * blockSize;
				const std::uint32_t mipSize = mipWidth * mipHeight / 16 * blockSize;


				mipDataPtr += mipSize;
				mipPCDataPtr += mipSize;
			}
			*/
		}
        
		nfr::api::path outFileDDSPath = EngineFactory->getResourcesDirectory();
		outFileDDSPath.append("textures");

		std::string ddsFileName = textureName;
		ddsFileName += ".dds";
		outFileDDSPath.append(ddsFileName);

		if (EngineFactory->exists(outFileDDSPath)) {
			std::filesystem::remove(outFileDDSPath);
		}

		nfr::api::SafeInterface<nfr::api::IStream> ddsStream = EngineFactory->openFile(nfr::api::EStreamFlags::WriteFlag, outFileDDSPath);
		if (!ddsStream->isOpen()) {
			dbg::Warning("Can't create raw file {}. Skipping the file...", ddsFileName);
			continue;
		}
        
        if (!TextureConverter::EncodeTextureToFile(textureInfo.Width, textureInfo.Height, 1, texFormat, texFormat, pcDataPtr, textureInfo.ImageSize, ddsStream.get())) {
            return false;
        }
	}

	return true;
}

void NotifyLoadSolidList(SolidListHeader* solidHeader)
{

}

SolidListHeader* ProcessSolidListHeaderChunk(aChunk* chunkData)
{
	SolidListHeader* foundHeader = nullptr;
	return foundHeader;
}

void ProcessSolidListDataChunk(aChunk* chunkData, SolidListHeader* solidHeader)
{

}

bool ProcessSolidListChunk(aChunk* chunkData)
{
	ENFSChunkId chunkEnumId = static_cast<ENFSChunkId>(chunkData->Id);
	if (chunkEnumId != ENFSChunkId::Geometry) {
		return false;
	}

	aChunk* nextChunk = chunkData + 1;
	aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + 8);
	SolidListHeader* solidHeader = nullptr;
	while (nextChunk != lastChunk) {
		if (nextChunk->Id == static_cast<std::uint32_t>(ENFSChunkId::GeometryHeader)) {
			if (solidHeader != nullptr) {
				NotifyLoadSolidList(solidHeader);
			}

			solidHeader = ProcessSolidListHeaderChunk(nextChunk);
		}
		else if (nextChunk->Id == static_cast<std::uint32_t>(ENFSChunkId::GeometryData)) {
			ProcessSolidListDataChunk(nextChunk, solidHeader);
		}

		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + sizeof(aChunk));
	}

	return true;
}

#define MAX_FONT_STATES 24

struct OldEngineFont
{
	char Signature[4];
	std::uint32_t Size;
	std::uint16_t Version;
	std::uint16_t Num;
	std::int32_t Flags;
	std::int8_t CenterX;
	std::int8_t CenterY;
	std::uint8_t Ascent;
	std::uint8_t Descent;
	std::int32_t GlyphTbl;
	std::int32_t KernTbl;
	std::int32_t Shape;
	std::int32_t States[MAX_FONT_STATES];

	void EndianSwap()
	{
		Size = BYTESWAP_LONG(Size);
		Version = BYTESWAP_SHORT(Version);
		Num = BYTESWAP_SHORT(Num);
		Flags = BYTESWAP_SHORT(Flags);
		GlyphTbl = BYTESWAP_LONG(GlyphTbl);
		KernTbl = BYTESWAP_LONG(KernTbl);
		Shape = BYTESWAP_LONG(Shape);
		for (std::int32_t& state : States) {
			state = BYTESWAP_LONG(state);
		}
	}
};

struct EngineFont
{
	char FontName[256];
	char TextureName[256];
	OldEngineFont Font;
};

struct FontDescription
{
	std::string Name;
	std::string TextureName;
	std::int32_t Height;
};

bool ProcessEngineFontChunk(aChunk* chunkData)
{
	const bool isXenonPlatform = true;

	FontDescription fontDescription = {};
	EngineFont* fontData = reinterpret_cast<EngineFont*>(chunkData->getDataPtr());
	if (isXenonPlatform) {
		fontData->Font.EndianSwap();
	}

	fontDescription.Name = fontData->FontName;
	fontDescription.TextureName = fontData->TextureName;
	fontDescription.Height = static_cast<float>(fontData->Font.States[1]);

	dbg::Log("    Found \"{}\" font with {} height", fontDescription.Name, fontDescription.Height);
	return true;
}

void* GetDataFrontendPackageChunk(aChunk* chunkData)
{
	ENFSChunkId chunkId = static_cast<ENFSChunkId>(chunkData->Id);
	if (chunkId == ENFSChunkId::FEPackage) {
		return (chunkData + 1);
	}
	
	return nullptr;
}

std::string GetFrontendPackageName(aChunk* chunkData)
{
	char* dataChunk = chunkData->getDataPtr();
	ENFSChunkId chunkId = static_cast<ENFSChunkId>(chunkData->Id);

	// "DiscError.fng"
	// "FeMainMenu.fng"
	// "Loading.fng"

	// PkHd
	if (chunkId == ENFSChunkId::FEPackage) {
		std::uint32_t magicWord = *(std::uint32_t*)dataChunk;
		if (magicWord == 0xE76E4546 || magicWord == 0x64486B50 || magicWord >= 0x20000) {
			const char* packageName = dataChunk + 40;
			return packageName;
		}
	} else if (chunkId == ENFSChunkId::FNGCompress) {
		//aChunk* nextChunk = chunkData + 1;
		//return nextChunk->Id;
	}

	return "";
}

struct FrontendPackage
{
	std::string Name;
	std::vector<char> Data;
};

bool ProcessFrontendPackageChunk(aChunk* chunkData)
{
    return true;
	aChunk* dataChunk = chunkData + 1;
	ENFSChunkId chunkId = static_cast<ENFSChunkId>(chunkData->Id);

	if (chunkId == ENFSChunkId::FEPackage) {
		while (static_cast<ENFSChunkId>(dataChunk->Id) == ENFSChunkId::FNGCompress) {
			dataChunk = dataChunk + 1;
		}
	}

	FrontendPackage frontendPackage = {};
	frontendPackage.Name = GetFrontendPackageName(chunkData);
	frontendPackage.Data.resize(dataChunk->Size);
	std::memcpy(frontendPackage.Data.data(), dataChunk->getDataPtr(), dataChunk->Size);

	dbg::Log("    Found \"{}\" frontend package with hash {:#06x} and {} size.",
		frontendPackage.Name, 
		nfr::api::getBinaryUpperHash(frontendPackage.Name.c_str()),
		frontendPackage.Data.size()
	);

	return true;
}
enum class QuickSplineEndPointType : std::int32_t
{
	Loop,
	Line,
	Extrapolated
};

enum class QuickSplineBasisType : std::int32_t
{
	Overhauser
};

struct QuickSpline
{
	std::uint32_t Hash;
	QuickSplineEndPointType EndPointType;
	QuickSplineBasisType BasisType;
	float MaxParam;
	float MinParam;
	float Length;
	std::int8_t ControlPointsDirty;
	std::int8_t BufferWasAllocated;
	std::int8_t MinControlPoints;
	std::uint16_t MaxControlPoints;
	std::uint16_t NumControlPoints;
};

std::uint32_t GetDecompressedJDLZSize(bool isLittleEndian, char* input)
{
	std::uint32_t outputLength = *(std::uint32_t*)(input + 8);
	if (!isLittleEndian) {
		outputLength = BYTESWAP_LONG(outputLength);
	}

	return outputLength;
}

struct JLZPackHeader
{
    char MagicWord[4];
    char FirstFlag;
    char SecondFlag;
    char Padding[2];
    std::uint32_t UncompressedSize;
    std::uint32_t CompressedSize;
};

void JLDZ_Decompress(std::uint8_t* input, std::uint8_t* output)
{
    int flags1 = 1, flags2 = 1;
    int t = 0, length = 0;
    int inPos = 16, outPos = 0;
    
    JLZPackHeader* dataHeader = reinterpret_cast<JLZPackHeader*>(input);
    int inputLength = dataHeader->CompressedSize;
    int outputLength = dataHeader->UncompressedSize;

    while ((inPos < inputLength) && (outPos < outputLength))
    {
        if (flags1 == 1) {
            flags1 = input[inPos++] | 0x100;
        }
        
        if (flags2 == 1) {
            flags2 = input[inPos++] | 0x100;
        }

        if ((flags1 & 1) == 1) {
            if((flags2 & 1) == 1 ) {
                length = ( input[inPos + 1] | ( ( input[inPos] & 0xF0 ) << 4 ) ) + 3;
                t = ( input[inPos] & 0x0F ) + 1;
            } else {
                t = ( input[inPos + 1] | ( ( input[inPos] & 0xE0 ) << 3 ) ) + 17;
                length = ( input[inPos] & 0x1F ) + 3;
            }

            inPos += 2;
            for (int i = 0; i < length; ++i) {
                output[outPos + i] = output[outPos + i - t];
            }

            outPos += length;
            flags2 >>= 1;
        } else {
            if (outPos < outputLength) {
                output[outPos++] = input[inPos++];
            }
        }
        
        flags1 >>= 1;
    }
}

bool ProcessQuickSplineChunk(aChunk* chunkData)
{
	aChunk* nextChunk = chunkData + 1;
	aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + sizeof(aChunk));
	QuickSpline* spline = reinterpret_cast<QuickSpline*>(nextChunk->getDataPtr() + 16);

	// #TODO: rework this shit
	dbg::Log("    Found \"{}\" spline with {} size ({} -> {})",
		spline->Hash,
		spline->Length,
		spline->MinParam,
		spline->MaxParam
	);

	while (nextChunk != lastChunk) {
		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + sizeof(aChunk));
	}


	return true;
}

bool ProcessChunk(aChunk* chunkData)
{
    bool result = false;
	const std::string_view& chunkName = (NfsChunkIdMap.find(chunkData->Id) != NfsChunkIdMap.end() ? NfsChunkIdMap.at(chunkData->Id) : "Unknown chunk");
	
	ENFSChunkId chunkEnumId = static_cast<ENFSChunkId>(chunkData->Id);
	switch (chunkEnumId) {
	case ENFSChunkId::TPK_Blocks:
	case ENFSChunkId::TPK_DataBlock:
		result = ProcessTexturePackChunk(chunkData);
        break;
	case ENFSChunkId::FEFont:
		result = ProcessEngineFontChunk(chunkData);
        break;
	case ENFSChunkId::QuickSpline:
		result = ProcessQuickSplineChunk(chunkData);
        break;
	case ENFSChunkId::Geometry:
		result = ProcessSolidListChunk(chunkData);
        break;
	default:
		break;
	}
    
    if (!result) {
        dbg::Warning("Can't process chunk \"{}\" ({:#06x}). Skipping chunk...", chunkName, chunkData->Id);
    } else {
        dbg::Log("Processed chunk \"{}\" ({:#06x})", chunkName, chunkData->Id);
    }
    
	return result;
}


nfr::api::IStream* OpenFile(const char* filePath)
{
	nfr::api::path globalFile = EngineFactory->getGameDirectory();
    globalFile.append(filePath);
    if (!EngineFactory->exists(globalFile)) {
        dbg::Error("Can't open \"{}\" file. Aborting...", filePath);
        return {};
    }

    return EngineFactory->openFile(nfr::api::EStreamFlags::ReadFlag, globalFile);
}

struct CIPHeader
{
    int Magic;
    int USize;
    int CSize;
    int UPos;
    int CPos;
    char UnusedPadding[4];
};

bool ProcessChunkedFile(nfr::api::IStream* globalStream)
{
    std::vector<char> unpackedDataBuffer;

    bool isPackedData = false;
    std::uint32_t magicWord = 0;
    globalStream->read(&magicWord, sizeof(std::uint32_t));
    globalStream->seek(nfr::api::EStreamMode::Set, 0);

    while (!globalStream->isEndOfFile()) {
        aChunk chunk = {};
        globalStream->read(&chunk, sizeof(chunk));
        if (chunk.Id == 0) {
            continue;
        }
        
        if (globalStream->getSize() < chunk.Size) {
            dbg::Warning("Invalid chunk size (chunkSize: {}; fileSize: {}). Skipping chunk...", chunk.Size, globalStream->getSize());
            continue;
        }

        unpackedDataBuffer.resize(std::max(unpackedDataBuffer.size(), std::size_t(chunk.Size + sizeof(aChunk))));
        std::fill(unpackedDataBuffer.begin(), unpackedDataBuffer.end(), 0);

        globalStream->read(unpackedDataBuffer.data() + sizeof(aChunk), chunk.Size);
        std::memcpy(unpackedDataBuffer.data(), &chunk, sizeof(aChunk));

        aChunk* chunkPtr = reinterpret_cast<aChunk*>(unpackedDataBuffer.data());
        if (!ProcessChunk(chunkPtr)) {
            continue;
        }
    }

    dbg::Log("All chunks are processed.");
    dbg::Log("");
    return true;
}

bool ProcessCompressedFile(nfr::api::IStream* compressedFile, std::vector<std::uint8_t>& decompressedData)
{
    std::vector<std::uint8_t> compressedData;
    
    std::uint32_t magicWord = 0;
    compressedFile->read(&magicWord, 4);
    if (magicWord != 0x5A4C444A || compressedFile->getSize() < 16) {
        return false;
    }
    
    compressedFile->seek(nfr::api::EStreamMode::Set, 0);
    compressedData.resize(compressedFile->getSize());
    compressedFile->read(compressedData.data(), compressedData.size());
    
    JLZPackHeader* packHeader = reinterpret_cast<JLZPackHeader*>(compressedData.data());
    if (packHeader->CompressedSize > packHeader->UncompressedSize) {
        dbg::Error("Compressed size can't be bigger than uncompressed size. Aborting...");
        return false;
    }
    
    if (packHeader->FirstFlag != 2 || packHeader->SecondFlag != 16) {
        dbg::Error("Invalid flags are passed to JLZ file ({:#06x} - expected 0x2, {:#06x} - expected 0x10). Aborting...",
            packHeader->FirstFlag,
            packHeader->SecondFlag
        );
        
        return false;
    }
    
    dbg::Log("Found JLZ header (u - {}KB, c - {}KB)", packHeader->UncompressedSize / 1024, packHeader->CompressedSize / 1024);
    dbg::Log("Processing file decompression...");
    
    decompressedData.resize(packHeader->UncompressedSize);
    //JLZDecompress(compressedData.data(), decompressedData.data());
    JLDZ_Decompress(compressedData.data(), decompressedData.data());
    dbg::Log("File decompressed successfully. Trying to parse chunks inside...");
    
    std::uint32_t offset = 0;
    while (offset < decompressedData.size()) {
        aChunk* chunk = (aChunk*)&decompressedData[offset];
        offset += chunk->Size + sizeof(aChunk);
        if (chunk->Id == 0) {
            continue;
        }
        
        if (!ProcessChunk(chunk)) {
            continue;
        }
    }
    
    return true;
}

bool LoadCompressedFile(const char* filePath, bool saveUncompressedFile)
{
    std::vector<std::uint8_t> uncompressedData;
	nfr::api::SafeInterface<nfr::api::IStream> compressedFile = OpenFile(filePath);
    if (!compressedFile->isOpen()) {
        dbg::Error("Can't open \"{}\" file. Aborting...", filePath);
        return false;
    }
    
    dbg::Log("Processing \"{}\" file...", filePath);
    if (!ProcessCompressedFile(compressedFile.get(), uncompressedData)) {
        dbg::Error("Can't decompress \"{}\" file. Aborting...", filePath);
        return false;
    }
    
    if (saveUncompressedFile) {
		nfr::api::path pathToFile = EngineFactory->getTempDirectory();
        pathToFile.append(filePath);
        
		nfr::api::path directoriesPath = pathToFile;
        directoriesPath.remove_filename();
        std::filesystem::create_directories(directoriesPath);
        
        pathToFile.replace_extension(".ULZC");
		nfr::api::SafeInterface<nfr::api::IStream> uncompressedFile = EngineFactory->openFile(nfr::api::EStreamFlags::WriteFlag, pathToFile);
        if (!uncompressedFile->isOpen()) {
            return false;
        }
        
        uncompressedFile->write(uncompressedData.data(), uncompressedData.size());
    }
    
    return true;
}

bool LoadChunkedFile(const char* filePath)
{
	nfr::api::SafeInterface<nfr::api::IStream> globalStream = OpenFile(filePath);
	if (!globalStream->isOpen()) {
		dbg::Error("Can't open \"{}\" file. Aborting...", filePath);
		return false;
	}

    dbg::Log("Processing \"{}\" file...", filePath);
    return ProcessChunkedFile(globalStream.get());
}

}
