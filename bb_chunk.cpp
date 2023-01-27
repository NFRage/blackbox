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

namespace bb
{

nfr::api::binary_hash_map<FrontendPackage> FEPackages;
nfr::api::binary_hash_map<MaterialInfo> MaterialsMap;
nfr::api::binary_hash_map<GameLight> LightsMap;
std::vector<EngineLightPack> EngineLightsMap;

void 
JLZDecompress(std::uint8_t* input, std::uint8_t* output, std::int32_t inputLength, std::int32_t outputLength)
{
	// Appapted to C++ code from https://github.com/MWisBest/OpenNFSTools/blob/master/LibNFS/Compression/JDLZ.cs
    int flags1 = 1, flags2 = 1;
    int t = 0, length = 0;
    int inPos = 16, outPos = 0;

    while ((inPos < inputLength) && (outPos < outputLength)) {
        if (flags1 == 1) {
            flags1 = input[inPos++] | 0x100;
        }
        
        if (flags2 == 1) {
            flags2 = input[inPos++] | 0x100;
        }

        if ((flags1 & 1) == 1) {
            if ((flags2 & 1) == 1) {
                length = (input[inPos + 1] | ((input[inPos] & 0xF0) << 4)) + 3;
                t = (input[inPos] & 0x0F) + 1;
            } else {
                t = (input[inPos + 1] | ((input[inPos] & 0xE0) << 3)) + 17;
                length = (input[inPos] & 0x1F) + 3;
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

void 
ProcessTextureLoadAnimationChunk(aChunk* anumChunk)
{
	aChunk* nextChunk = anumChunk + 1;
	aChunk* lastChunk = (aChunk*)((char*)anumChunk + anumChunk->Size + 8);
	while (nextChunk != lastChunk) {
		dbg::Verbose("Processing anim chunk...");
		if (nextChunk->Id != static_cast<std::uint32_t>(ENFSChunkId::TPK_AnimBlock)) {
			dbg::Warning("Unexpected chunkId {:#06x} in texture animation chunk. Skipping this one...", nextChunk->Id);
			nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + 8);
			continue;
		}

		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + 8);
	}
}

bool
ProcessTexturePackHeaderChunk(
	aChunk* chunkData,
	TexturePackHeader& outHeader, 
	TexturePlatInfo& outPlatInfo,
	std::vector<TextureInfo>& texturesInfo
)
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
		const char* chunkName = (NfsChunkIdMap.find(nextChunk->Id) != NfsChunkIdMap.end() ? NfsChunkIdMap.at(nextChunk->Id).data() : "");
		dbg::Verbose("    Processing chunk {}...", chunkName);

		switch (static_cast<ENFSChunkId>(nextChunk->Id)) {
		case ENFSChunkId::TPK_InfoPart1: {
			TexturePackHeader* texturePackHeader = (TexturePackHeader*)(nextChunk + 1);
			std::memcpy(&outHeader, texturePackHeader, sizeof(TexturePackHeader));
			dbg::Verbose("        Found package name {:#06x} ({})", texturePackHeader->FilenameHash, texturePackHeader->Name);
		}
		break;

		case ENFSChunkId::TPK_InfoPart2: {
			TextureIndexEntry* textureIndexEntry = (TextureIndexEntry*)(nextChunk + 1);
			textureIndexesCount = (nextChunk->Size / sizeof(TextureIndexEntry));
			for (std::uint32_t i = 0; i < textureIndexesCount; i++) {
				dbg::Verbose("        Found entry hash {:#06x} (padding: {})", textureIndexEntry->NameHash, textureIndexEntry->Padding);
				hashesStorage.insert(textureIndexEntry->NameHash);
				textureIndexEntry++;
			}

		}
		break;

		case ENFSChunkId::TPK_InfoPart3: {
			StreamingEntry* streamingEntry = (StreamingEntry*)(nextChunk + 1);
			for (std::uint32_t i = 0; i < (nextChunk->Size / sizeof(StreamingEntry)); i++) {
				dbg::Verbose("        Found streaming entry {:#06x}", streamingEntry->NameHash);
				streamingEntry++;
			}
		}
		break;

		case ENFSChunkId::TPK_InfoPart4: {
			textureInfo = (TextureInfo*)(nextChunk + 1);;
			texturesInfo.reserve(textureIndexesCount);
			for (std::uint32_t i = 0; i < textureIndexesCount; i++) {
				if (std::memcmp(textureInfo->BigPadding, verifyBuffer, sizeof(verifyBuffer)) != 0) {
					dbg::Warning("Something wrong with BigPadding!!! Couldn't process this chunk anymore (i - {})", i);
					break;
				}

				dbg::Verbose("        Found texture info {:#06x} ({})", textureInfo->NameHash, textureInfo->DebugName);
				texturesInfo.emplace_back(*textureInfo);
				textureInfo = (TextureInfo*)((char*)textureInfo + textureInfo->DebugNameSize + 89);
			}
		}
		break;

		case ENFSChunkId::TPK_InfoPart5: {
			TexturePlatInfo* texturePlatInfoEntry = (TexturePlatInfo*)(nextChunk + 1);
			std::memcpy(&outPlatInfo, texturePlatInfoEntry, sizeof(TexturePlatInfo));

			const std::string_view& formatName = (TexturesFormatMap.find(texturePlatInfoEntry->format) != TexturesFormatMap.end() ? TexturesFormatMap.at(texturePlatInfoEntry->format) : "");
			dbg::Verbose("        Found texture plat info (format: {})", formatName);
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

aChunk*
ProcessTexturePackDataChunk(aChunk* chunkData)
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

	dbg::Verbose("        Found data chunk {:#06x}", vramHeader->FilenameHash);
	return dataChunk;
}

bool
ProcessTexturePackChunk(aChunk* chunkData)
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
	} else if (chunkId == ENFSChunkId::TPK_DataBlock) {
		dataChunk = ProcessTexturePackDataChunk(chunkData);
	}

	if (texturePackHeader.FilenameHash == 0) {
		dbg::Verbose("Procedeed to the empty chunk. Skipping the chunk");
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

	dbg::Verbose("    Processing textures in TPK block...");
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

		dbg::Verbose("        Loading {} ({}x{}, {}KB, {} mips, {} format)... ",
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
			if (!TextureConverter::UntileXenonTexture(textureInfo.Width, textureInfo.Height, textureInfo.NumMipMapLevels, blockSize, mipDataPtr, mipPCDataPtr)) {
				dbg::Warning("Couldn't convert {} texture from Xenon to PC format.", textureInfo.DebugName);
				return false;
			}
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
			dbg::Warning("Couldn't create raw file {}. Skipping the file...", ddsFileName);
			continue;
		}
        
        if (!TextureConverter::EncodeTextureToFile(textureInfo.Width, textureInfo.Height, 1, texFormat, texFormat, pcDataPtr, textureInfo.ImageSize, ddsStream.get())) {
            return false;
        }
	}

	return true;
}

void 
NotifyLoadSolidList(SolidListHeader* solidHeader)
{

}

SolidListHeader* 
ProcessSolidListHeaderChunk(aChunk* chunkData)
{
	SolidListHeader* foundHeader = nullptr;
	return foundHeader;
}

void 
ProcessSolidListDataChunk(aChunk* chunkData, SolidListHeader* solidHeader)
{

}

bool 
ProcessSolidListChunk(aChunk* chunkData)
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
		} else if (nextChunk->Id == static_cast<std::uint32_t>(ENFSChunkId::GeometryData)) {
			ProcessSolidListDataChunk(nextChunk, solidHeader);
		}

		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + sizeof(aChunk));
	}

	return true;
}

bool
ProcessEngineFontChunk(aChunk* chunkData)
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

	dbg::Verbose("    Found \"{}\" font with {} height", fontDescription.Name, fontDescription.Height);
	return true;
}

void* 
GetDataFrontendPackageChunk(aChunk* chunkData)
{
	ENFSChunkId chunkId = static_cast<ENFSChunkId>(chunkData->Id);
	if (chunkId == ENFSChunkId::FEPackage) {
		return (chunkData + 1);
	}
	
	return nullptr;
}

std::pair<std::uint32_t, std::string>
GetFrontendPackageName(aChunk* chunkData)
{
	char* dataChunk = chunkData->getDataPtr();
	ENFSChunkId chunkId = static_cast<ENFSChunkId>(chunkData->Id);
	if (chunkId == ENFSChunkId::FEPackage) {
		std::uint32_t magicWord = *(std::uint32_t*)dataChunk;
		if (magicWord == 0xE76E4546 || magicWord == 0x64486B50 || magicWord >= 0x20000) {
			const char* packageName = dataChunk + 40;
			return { nfr::api::getBinaryUpperHash(packageName), packageName};
		}
	} else if (chunkId == ENFSChunkId::FNGCompress) {
		aChunk* nextChunk = chunkData + 1;
		if (EntriesMap.find(nextChunk->Id) != EntriesMap.end()) {
			return { nextChunk->Id, EntriesMap.at(nextChunk->Id) };
		}

		dbg::Warning("Couldn't find string for hash {:#06x}. Skipping name of this one...", nextChunk->Id);
		return { nextChunk->Id, ""};
	}

	dbg::Warning("Invalid chunk has passed to this function...");
	return { 0xFFFFFFFF, "" };
}

bool
ProcessFrontendPackageChunk(aChunk* chunkData)
{
	aChunk* dataChunk = chunkData;
	aChunk* nextChunk = dataChunk + 1;
	ENFSChunkId chunkId = static_cast<ENFSChunkId>(chunkData->Id);

	if (chunkId == ENFSChunkId::FEPackage && nextChunk->Size <= dataChunk->Size) {
		dataChunk = dataChunk + 1;
		nextChunk = dataChunk + 1;
	}

	if (chunkId == ENFSChunkId::FNGCompress && nextChunk->Size <= dataChunk->Size) {
		dataChunk = dataChunk + 1;
	}

	FrontendPackage frontendPackage = {};
	auto packageNamePair = GetFrontendPackageName(chunkData);
	frontendPackage.Hash = packageNamePair.first;
	frontendPackage.Name = packageNamePair.second;
	frontendPackage.Data.resize(dataChunk->Size);
	std::memcpy(frontendPackage.Data.data(), dataChunk->getDataPtr(), dataChunk->Size);

	if (frontendPackage.Name.empty()) {
		dbg::Verbose("    Found frontend package with hash {:#06x} and {} size.",
			frontendPackage.Hash,
			frontendPackage.Data.size()
		);
	} else {
		dbg::Verbose("    Found \"{}\" frontend package with hash {:#06x} and {} size.",
			frontendPackage.Name, 
			frontendPackage.Hash,
			frontendPackage.Data.size()
		);
	}

	FEPackages.emplace(std::make_pair(frontendPackage.Hash, std::move(frontendPackage)));
	return true;
}

bool 
ProcessQuickSplineChunk(aChunk* chunkData)
{
	aChunk* nextChunk = chunkData + 1;
	aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + sizeof(aChunk));
	QuickSpline* spline = reinterpret_cast<QuickSpline*>(nextChunk->getDataPtr() + 16);

	// #TODO: rework this shit
	dbg::Verbose("    Found \"{}\" spline with {} size ({} -> {})",
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

bool 
ProcessMaterialsChunk(aChunk* chunkData)
{
	MaterialStruct* material = reinterpret_cast<MaterialStruct*>(chunkData->getDataPtr());
	dbg::Verbose("    Found {}\"{}\" material with hash {:#06x} and version {}",
		material->NameHash == 0x2C420D64 ? "default " : "",
		material->Name,
		material->NameHash,
		material->Version
	);

	MaterialsMap.emplace(std::make_pair(material->NameHash, material->Data));
	return true;
}

bool
ProcessLightsChunk(aChunk* chunkData)
{
	EngineLightPack* engineLight = nullptr;
	aChunk* nextChunk = chunkData + 1;
	aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + sizeof(aChunk));
	while (nextChunk != lastChunk) {
		switch (static_cast<ENFSChunkId>(nextChunk->Id)) {
			case ENFSChunkId::LightPack: {
				LightPack* lightPack = nextChunk->getDataPtr<LightPack>();
				if (GameVersion == EGameVersion::ProStreetXenon || GameVersion == EGameVersion::ProStreetPC) {
					if (lightPack->Version != 4) {
						dbg::Error("Invalid version of the light pack (in {}, required {})", lightPack->Version, 4);
						return false;
					}
				}

				dbg::Verbose("    Found light pack ({} lights, {} tree nodes)", lightPack->NumLights, lightPack->NumTreeNodes);
				engineLight = &EngineLightsMap.emplace_back(std::move(
					EngineLightPack(lightPack->ScenerySectionNumber, lightPack->NumTreeNodes, lightPack->NumLights)
				));
			}
			break;
			case ENFSChunkId::AABBTree: {
				AABBTree* aabbTree = nextChunk->getDataPtr<AABBTree>();
				dbg::Verbose("    Found AABB tree ({} leaf nodes, {} parent nodes)", aabbTree->NumLeafNodes, aabbTree->NumParentNodes);
				engineLight->aabb.Depth = aabbTree->Depth;
				engineLight->aabb.TotalNodes = aabbTree->TotalNodes;
				engineLight->aabb.NumParentNodes = aabbTree->NumParentNodes;
				engineLight->aabb.NumLeafNodes = aabbTree->NumLeafNodes;
			}
			break;
			case ENFSChunkId::LightArray: {
				GameLight* gameLight = nextChunk->getDataPtr<GameLight>();
				dbg::Verbose("    Found \"{}\" game light with hash {:#06x}", gameLight->Name, gameLight->NameHash);
				LightsMap.emplace(std::move(std::make_pair(gameLight->NameHash, *gameLight)));
			}
			break;
		default:
			break;
		}
		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + sizeof(aChunk));
	}

	return true;
}

bool
ProcessPCAWeightsChunk(aChunk* chunkData)
{
	ePcaWeights* weights = nullptr;
	aChunk* nextChunk = chunkData + 1;
	aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + sizeof(aChunk));
	while (nextChunk != lastChunk) {
		switch (static_cast<ENFSChunkId>(nextChunk->Id)) {
		case ENFSChunkId::PCAWeightsData: {
			weights = nextChunk->getDataPtr<ePcaWeights>();
			if (EntriesMap.find(weights->NameHash) != EntriesMap.end()) {
				dbg::Verbose("    Found PCA weights data \"{}\" with hash {:#06x}", EntriesMap.at(weights->NameHash), weights->NameHash);
			} else {
				dbg::Verbose("    Found PCA weights data with hash {:#06x}", weights->NameHash);
			}
		}
		break;
		case ENFSChunkId::PCAMeanData:

			break;
		case ENFSChunkId::PCAFramesData:

			break;
		default:
			break;
		}

		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + sizeof(aChunk));
	}

	return true;
}

bool
ProcessEventSysData(char* data, std::uint32_t dataSize)
{
	if (std::memcmp(data, "CARP", 4) != 0) {
		return false;
	}

	//EventSysPipeGroup* eventSys = reinterpret_cast<EventSysPipeGroup*>(data);
	data += 16;
	if (std::memcmp(data, "Strs", 4) != 0) {
		return false;
	}

	return true;
}

bool 
ProcessEventSequenceChunk(aChunk* chunkData)
{
	aChunk* nextChunk = chunkData + 1;
	aChunk* lastChunk = (aChunk*)((char*)chunkData + chunkData->Size + sizeof(aChunk));
	while (nextChunk != lastChunk) {
		if (!ProcessEventSysData(nextChunk->getDataPtr() + 24, nextChunk->Size)) {
			return false;
		}

		dbg::Verbose("    Found event sequence.");
		nextChunk = (aChunk*)((char*)nextChunk + nextChunk->Size + sizeof(aChunk));
	}

	return true;
}

bool
ProcessCarTypesInfosChunk(aChunk* chunkData)
{
	CarTypeInfo* carTypeInfo = chunkData->getAlignedPtr<CarTypeInfo>(0x10);
	dbg::Verbose("    Found car type info \"{}\" for \"{}\" model.", carTypeInfo->CarTypeName, carTypeInfo->BaseModelName);
	return true;
}

bool
ProcessChunk(aChunk* chunkData)
{
    bool result = false;
	const char* chunkName = (NfsChunkIdMap.find(chunkData->Id) != NfsChunkIdMap.end() ? NfsChunkIdMap.at(chunkData->Id).data() : "Unknown chunk");
	
	ENFSChunkId chunkEnumId = static_cast<ENFSChunkId>(chunkData->Id);
	dbg::Verbose("");
	dbg::Verbose("[\"{}\"]:", chunkName);
	dbg::Verbose("--------------------------------------------------");
	switch (chunkEnumId) {
	case ENFSChunkId::CarTypeInfos:
		result = ProcessCarTypesInfosChunk(chunkData);
		break;
	case ENFSChunkId::EventSequence:
		result = ProcessEventSequenceChunk(chunkData);
		break;
	case ENFSChunkId::PCAWeights:
		result = ProcessPCAWeightsChunk(chunkData);
		break;
	case ENFSChunkId::FEPackage:
	case ENFSChunkId::FNGCompress:
		result = ProcessFrontendPackageChunk(chunkData);
		break;
	case ENFSChunkId::ELights:
		result = ProcessLightsChunk(chunkData);
		break;
	case ENFSChunkId::Materials:
		result = ProcessMaterialsChunk(chunkData);
		break;
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
	default:
		dbg::Error("Can't process unknown chunk {:#06x} (\"{}\"). Skipping chunk...", chunkData->Id,chunkName);
		dbg::Verbose("--------------------------------------------------");
		return false;
	}

    if (!result) {
        dbg::Error("Can't process chunk {:#06x} (\"{}\"). Skipping chunk...", chunkData->Id, chunkName);
    }

	dbg::Verbose("--------------------------------------------------");
    
	return result;
}

nfr::api::IStream* 
OpenFile(const char* filePath)
{
	nfr::api::path globalFile = EngineFactory->getGameDirectory();
    globalFile.append(filePath);
    if (!EngineFactory->exists(globalFile)) {
        dbg::Error("Can't open \"{}\" file. Aborting...", filePath);
        return {};
    }

    return EngineFactory->openFile(nfr::api::EStreamFlags::ReadFlag, globalFile);
}

bool 
ProcessChunkedFile(nfr::api::IStream* globalStream)
{
    std::vector<char> unpackedDataBuffer;

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
		std::memset(unpackedDataBuffer.data(), 0, unpackedDataBuffer.size());

        globalStream->read(unpackedDataBuffer.data() + sizeof(aChunk), chunk.Size);
        std::memcpy(unpackedDataBuffer.data(), &chunk, sizeof(aChunk));

        if (!ProcessChunk(reinterpret_cast<aChunk*>(unpackedDataBuffer.data()))) {
            continue;
        }
    }

    dbg::Log("All chunks are processed.");
    dbg::Verbose("");
    return true;
}

bool
ProcessCompressedFile(nfr::api::IStream* compressedFile, std::vector<std::uint8_t>& decompressedData)
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
    
    dbg::Verbose("Found JLZ header (u - {}KB, c - {}KB)", packHeader->UncompressedSize / 1024, packHeader->CompressedSize / 1024);
    dbg::Verbose("Processing file decompression...");
    
    decompressedData.resize(packHeader->UncompressedSize);
	JLZDecompress(compressedData.data(), decompressedData.data(), packHeader->CompressedSize, packHeader->UncompressedSize);
    dbg::Verbose("File decompressed successfully. Trying to parse chunks inside...");
    
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

bool
LoadCompressedFile(const char* filePath, bool saveUncompressedFile)
{
    std::vector<std::uint8_t> uncompressedData;
	nfr::api::SafeInterface<nfr::api::IStream> compressedFile = OpenFile(filePath);
    if (!compressedFile->isOpen()) {
        dbg::Error("Can't open \"{}\" file. Aborting...", filePath);
        return false;
    }
    
    dbg::Log("Processing \"{}\" file...", filePath);
    if (!ProcessCompressedFile(compressedFile.get(), uncompressedData)) {
        dbg::Error("Couldn't decompress \"{}\" file. Aborting...", filePath);
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

bool 
LoadChunkedFile(const char* filePath)
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
