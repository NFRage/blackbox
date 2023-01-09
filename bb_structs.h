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
#include <cstdint>

#define ALIGN_VALUE(x, align)  ((x + (align-1)) & (~(align-1)))

enum class ENFSChunkId : std::uint32_t
{
	Empty = 0x00000000, // none
	FEFont = 0x00030201, // 0x10 Modular
	FEPackage = 0x00030203, // 0x10 Modular
	FNGCompress = 0x00030210, // 0x10 Modular
	PresetRides = 0x00030220, // 0x10 Modular
	MagazinesFrontend = 0x00030230, // 0x10 Modular
	MagazinesShowcase = 0x00030231, // 0x10 Modular
	WideDecals = 0x00030240, // 0x10 Modular
	PresetSkins = 0x00030250, // 0x08 Actual
	Smokeables = 0x00034026, // 0x10 Modular
	WorldBounds = 0x00034027, // varies
	SceneryOverride = 0x00034107, // 0x10 Modular
	SceneryGroup = 0x00034108, // 0x10 Modular
	TrackPosMarkers = 0x00034146, // varies
	Tracks = 0x00034201, // 0x10 Modular
	SunInfos = 0x00034202, // varies
	Weatherman = 0x00034250, // varies
	CarTypeInfos = 0x00034600, // 0x10 Modular
	CarSkins = 0x00034601, // 0x10 Modular
	DBCarParts_Header = 0x00034603, // varies
	DBCarParts_Array = 0x00034604, // varies
	DBCarParts_Attribs = 0x00034605, // varies
	DBCarParts_Strings = 0x00034606, // varies
	SlotTypes = 0x00034607, // varies
	CarInfoAnimHookup = 0x00034608, // 0x10 Modular
	CarInfoAnimHideup = 0x00034609, // varies
	DBCarParts_Structs = 0x0003460A, // varies
	DBCarParts_Models = 0x0003460B, // varies
	DBCarParts_Offsets = 0x0003460C, // varies
	DBCarParts_Custom = 0x0003460D, // varies
	GCareer_Upgrade = 0x00034A01, // varies
	GCareer_Races_Old = 0x00034A02, // 0x08 Actual
	StyleMomentsInfo = 0x00034A07, // 0x80 Modular
	StylePartitions = 0x00034A08, // 0x80 Modular
	GCareer_Stars = 0x00034A09, // varies
	GCareer_Styles = 0x00034A0A, // 0x10 Modular
	GCareer_Races = 0x00034A11, // varies
	GCareer_Shops = 0x00034A12, // varies
	GCareer_Brands = 0x00034A14, // varies
	GCareer_PartPerf = 0x00034A15, // varies
	GCareer_Showcases = 0x00034A16, // varies
	GCareer_Messages = 0x00034A17, // varies
	GCareer_Stages = 0x00034A18, // varies
	GCareer_Sponsors = 0x00034A19, // varies
	GCareer_PerfTun = 0x00034A1A, // varies
	GCareer_Challenges = 0x00034A1B, // varies
	GCareer_PartUnlock = 0x00034A1C, // varies
	GCareer_Strings = 0x00034A1D, // varies
	GCareer_BankTrigs = 0x00034A1E, // varies
	GCareer_CarUnlocks = 0x00034A1F, // varies
	DifficultyInfo = 0x00034B00, // 0x80 Modular
	AcidEffects = 0x00035020, // 0x80 Modular
	AcidEmitters = 0x00035021, // 0x80 Modular
	Stream37220 = 0x00037220, // varies
	Stream37240 = 0x00037240, // varies
	Stream37250 = 0x00037250, // varies
	Stream37260 = 0x00037260, // varies
	Stream37270 = 0x00037270, // varies
	STRBlocks = 0x00039000, // 0x10 Modular
	LangFont = 0x00039001, // 0x10 Modular
	Subtitles = 0x00039010, // 0x10 Modular
	MovieCatalog = 0x00039020, // 0x80 Modular
	CompTPKBlock = 0x0003A100, // 0x10 Modular
	ICECatalog = 0x0003B200, // 0x80 Modular
	WWorld = 0x0003B800, // varies
	WCollisionPack = 0x0003B801, // varies
	WCollisionRaww = 0x0003B802, // 0x800 Modular
	NISScript = 0x0003B811, // varies
	Collision = 0x0003B901, // 0x10 Modular
	EmitterLibrary = 0x0003BC00, // varies
	TPKSettings = 0x0003BD00, // 0x80 Modular
	Vinyl_Header = 0x0003CE01, // 0x08 Actual
	Vinyl_PointerTable = 0x0003CE02, // 0x0C Actual
	Vinyl_PathEntry = 0x0003CE04, // varies
	Vinyl_PathData = 0x0003CE05, // varies
	Vinyl_PathPoint = 0x0003CE06, // varies
	Vinyl_FillEffect = 0x0003CE07, // varies
	Vinyl_StrokeEffect = 0x0003CE08, // varies
	Vinyl_DropShadow = 0x0003CE09, // varies
	Vinyl_InnerGlow = 0x0003CE0A, // varies
	Vinyl_ShadowEffect = 0x0003CE0B, // varies
	Vinyl_Gradient = 0x0003CE0C, // varies
	VinylDataHeader = 0x0003CE0E, // varies
	VinylCarEntries = 0x0003CE0F, // varies
	VinylFloatMatrix = 0x0003CE10, // varies
	VinylVectorEntries = 0x0003CE11, // varies
	SkinRegionDB = 0x0003CE12, // 0x10 Modular
	VinylMetaData = 0x0003CE13, // 0x10 Modular
	FX = 0x000B5846, // not supported
	Materials = 0x00135200, // 0x10 Modular
	EAGLSkeleton = 0x00E34009, // varies
	EAGLAnimations = 0x00E34010, // varies
	DDSTexture = 0x30300200, // 0x80 Modular
	ColorCube = 0x30300201, // 0x10 Modular
	PCAWater0 = 0x30300300, // 0x80 Modular
	TPK_InfoPart1 = 0x33310001, // 0x08 Actual
	TPK_InfoPart2 = 0x33310002, // 0x0C Actual
	TPK_InfoPart3 = 0x33310003, // varies
	TPK_InfoPart4 = 0x33310004, // varies
	TPK_InfoPart5 = 0x33310005, // varies
	TPK_AnimPart1 = 0x33312001, // varies
	TPK_AnimPart2 = 0x33312002, // varies
	TPK_DataPart1 = 0x33320001, // 0x08 Actual
	TPK_DataPart2 = 0x33320002, // 0x80 Modular
	TPK_DataPart3 = 0x33320003, // ????
	Nikki = 0x42704D67, // varies
	ABKC = 0x434B4241, // not supported
	LOCH = 0x48434F4C, // not supported
	VPAK = 0x4B415056, // not supported
	MOIR = 0x52494F4D, // not supported
	MEMO = 0x53219999, // not supported
	LZCompressed = 0x55441122, // varies
	MVhd = 0x6468564D, // not supported
	Gnsu = 0x75736E47, // not supported
	SpeedScenery = 0x80034100, // varies
	DBCarParts = 0x80034602, // varies
	GCareer_Old = 0x80034A00, // 0x80 Modular
	GCareer = 0x80034A10, // 0x80 Modular
	GLimitations = 0x80034A30, // 0x80 Modular
	EmitterTriggers = 0x80036000, // varies
	NISDescription = 0x80037020, // varies
	AnimDirectory = 0x80037050, // 0x10 Modular
	QuickSpline = 0x8003B000, // varies
	IceCameraPart0 = 0x8003B200, // 0x10 Modular
	IceCameraPart1 = 0x8003B201, // 0x10 Modular
	IceCameraPart2 = 0x8003B202, // 0x10 Modular
	IceCameraPart3 = 0x8003B203, // 0x10 Modular
	IceCameraPart4 = 0x8003B204, // 0x10 Modular
	IceSettings = 0x8003B209, // 0x10 Modular
	SoundStichs = 0x8003B500, // 0x10 Modular
	EventSequence = 0x8003B810, // varies
	DBCarBounds = 0x8003B900, // 0x08 Actual
	VinylSystem = 0x8003CE00, // 0x800 Modular
	Vinyl_PathSet = 0x8003CE03, // varies
	VinylDataTable = 0x8003CE0D, // 0x10 Modular
	Geometry = 0x80134000, // varies
	GeometryHeader = 0x80134001, // varies
	GeometryData = 0x80134010, // varies
	ELights = 0x80135000, // varies
	SpecialEffects = 0xB0300100, // 0x80 Modular
	PCAWeights = 0xB0300300, // 0x80 Modular
	TPK_Blocks = 0xB3300000, // 0x80 Modular
	TPK_InfoBlock = 0xB3310000, // 0x40 Modular
	TPK_BinData = 0xB3312000, // varies
	TPK_AnimBlock = 0xB3312004, // varies
	TPK_DataBlock = 0xB3320000, // 0x80 Modular
};


class TexturePack;

struct RenderState
{
	std::uint32_t Pad1 : 4;
	std::uint32_t AlphaTestRef : 4;
	std::uint32_t SubSortKey : 1;
	std::uint32_t ColourWriteAlpha : 1;
	std::uint32_t MultiPassBlend : 1;
	std::uint32_t BiasLevel : 2;
	std::uint32_t WantsAuxiliaryTextures : 1;
	std::uint32_t IsAdditiveBlend : 1;
	std::uint32_t HasTextureAnimation : 1;
	std::uint32_t TextureAddressV : 2;
	std::uint32_t TextureAddressU : 2;
	std::uint32_t AlphaBlendDest : 4;
	std::uint32_t AlphaBlendSrc : 4;
	std::uint32_t AlphaBlendEnabled : 1;
	std::uint32_t AlphaTestEnabled : 1;
	std::uint32_t IsBackFaceCulled : 1;
	std::uint32_t ZWriteEnabled : 1;
};

struct SolidListHeader
{
	char BigPadding[8];

	std::int32_t Version;
	std::int32_t NumSolids;
	char Filename[56];
	char GroupName[32];
	std::uint32_t PermChunkByteOffset;
	std::uint32_t PermChunkByteSize;
	std::int16_t MaxSolidChunkByteAlignment;
	std::int16_t EndianSwapped;

	char AnotherBigPadding[8];
	std::int16_t NumTexturePacks;
	std::int16_t NumDefaultTextures;

	char EndBigPadding[16];
	//bPList<TexturePack> TexturePackList;
	//bPList<eTextureEntry> DefaultTextureList;
};



struct TexturePlatInfo
{
	char BigPadding[8];
	RenderState mRenderState;
	std::uint32_t type;
	std::uint16_t Pad0;
	std::uint16_t PunchThruValue;
	std::uint32_t format;

	char AnotherBigPadding[8];
	//void* pD3DTexture;
	//struct eTextureBucket* pActiveBucket;
};

struct TextureInfo
{
	char BigPadding[12];
	std::uint32_t NameHash;
	std::uint32_t ClassNameHash;
	std::int32_t ImagePlacement;
	std::int32_t PalettePlacement;
	std::int32_t ImageSize;
	std::int32_t PaletteSize;
	std::int32_t BaseImageSize;
	std::int16_t Width;
	std::int16_t Height;
	std::int8_t ShiftWidth;
	std::int8_t ShiftHeight;
	std::uint8_t ImageCompressionType;
	std::uint8_t PaletteCompressionType;
	std::int16_t NumPaletteEntries;
	std::int8_t NumMipMapLevels;
	std::int8_t TilableUV;
	std::int8_t BiasLevel;
	std::int8_t RenderingOrder;
	std::int8_t ScrollType;
	std::int8_t UsedFlag;
	std::int8_t ApplyAlphaSorting;
	std::int8_t AlphaUsageType;
	std::int8_t AlphaBlendType;
	std::int8_t Flags;
	std::int8_t MipmapBiasType;
	std::int8_t Padding;
	std::int16_t ScrollTimeStep;
	std::int16_t ScrollSpeedS;
	std::int16_t ScrollSpeedT;
	std::int16_t OffsetS;
	std::int16_t OffsetT;
	std::int16_t ScaleS;
	std::int16_t ScaleT;
	char UnusedOffset[8];
	std::uint32_t PaletteData;
	std::int8_t DebugNameSize;
	char DebugName[35];
};

struct StreamingEntry
{
	std::uint32_t NameHash;
	std::uint32_t ChunkByteOffset;
	std::int32_t ChunkByteSize;
	std::int32_t UncompressedSize;
	std::uint8_t UserFlags;
	std::uint8_t Flags;
	std::uint16_t RefCount;
	std::uint32_t Padding;
};

struct TextureIndexEntry
{
	std::uint32_t NameHash;
	std::uint32_t Padding;
};

struct TexturePackHeader
{
	std::int32_t Version;
	char Name[28];
	char Filename[64];
	std::uint32_t FilenameHash;
	std::uint32_t PermChunkByteOffset;
	std::uint32_t PermChunkByteSize;
	std::int32_t EndianSwapped;

	char BigPadding[12];
	//TexturePack* pTexturePack;
	//TextureIndexEntry* TextureIndexEntryTable;
	//StreamingEntry* TextureStreamEntryTable;
};

struct TextureAnim
{
	char BigPadding[8];
	char Name[16];
	std::uint32_t NameHash;
	std::int8_t NumFrames;
	std::int8_t FramesPerSecond;
	std::int8_t TimeBase;
	std::int8_t pad0;
	std::int8_t EndianSwapped;
	std::int8_t Valid;
	std::int8_t CurrentFrame;
	std::int8_t pad1;
	char AnotherBigPadding[8];
	//TexturePack* ParentTexturePack;
	//TextureAnimEntry* TextureAnimTable;
};


struct TextureVRAMDataHeader
{
	char BigPadding[8];
	std::int32_t Version;
	std::uint32_t FilenameHash;
	std::int32_t EndianSwapped;
	std::uint32_t Padding;
};

/*
	alignment:

	0x33320003: 4096;
	0x33320002: 128
	0x3B400: 128
	0x134802: 32
	0x134B04: 128
	0x134B01: 128
*/
