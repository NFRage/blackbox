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

const std::unordered_map<std::uint32_t, std::string_view> NfsChunkIdMap =
{
	{ 0x00000000, "Empty"					},
	{ 0x00030201, "FEFont"					},
	{ 0x00030203, "FEFiles"					},
	{ 0x00030210, "FNGCompress"				},
	{ 0x00030220, "PresetRides"				},
	{ 0x00030230, "MagazinesFrontend"		},
	{ 0x00030231, "MagazinesShowcase"		},
	{ 0x00030240, "WideDecals"				},
	{ 0x00030250, "PresetSkins"				},
	{ 0x00034026, "Smokeables"				},
	{ 0x00034027, "WorldBounds"				},
	{ 0x00034107, "SceneryOverride"			},
	{ 0x00034108, "SceneryGroup"			},
	{ 0x00034146, "TrackPosMarkers"			},
	{ 0x00034201, "Tracks"					},
	{ 0x00034202, "SunInfos"				},
	{ 0x00034250, "Weatherman"				},
	{ 0x00034600, "CarTypeInfos"			},
	{ 0x00034601, "CarSkins"				},
	{ 0x00034603, "DBCarParts_Header"		},
	{ 0x00034604, "DBCarParts_Array"		},
	{ 0x00034605, "DBCarParts_Attribs"		},
	{ 0x00034606, "DBCarParts_Strings"		},
	{ 0x00034607, "SlotTypes"				},
	{ 0x00034608, "CarInfoAnimHookup"		},
	{ 0x00034609, "CarInfoAnimHideup"		},
	{ 0x0003460A, "DBCarParts_Structs"		},
	{ 0x0003460B, "DBCarParts_Models"		},
	{ 0x0003460C, "DBCarParts_Offsets"		},
	{ 0x0003460D, "DBCarParts_Custom"		},
	{ 0x00034A01, "GCareer_Upgrade"			},
	{ 0x00034A02, "GCareer_Races_Old"		},
	{ 0x00034A07, "StyleMomentsInfo"		},
	{ 0x00034A08, "StylePartitions"			},
	{ 0x00034A09, "GCareer_Stars"			},
	{ 0x00034A0A, "GCareer_Styles"			},
	{ 0x00034A11, "GCareer_Races"			},
	{ 0x00034A12, "GCareer_Shops"			},
	{ 0x00034A14, "GCareer_Brands"			},
	{ 0x00034A15, "GCareer_PartPerf"		},
	{ 0x00034A16, "GCareer_Showcases"		},
	{ 0x00034A17, "GCareer_Messages"		},
	{ 0x00034A18, "GCareer_Stages"			},
	{ 0x00034A19, "GCareer_Sponsors"		},
	{ 0x00034A1A, "GCareer_PerfTun"			},
	{ 0x00034A1B, "GCareer_Challenges"		},
	{ 0x00034A1C, "GCareer_PartUnlock"		},
	{ 0x00034A1D, "GCareer_Strings"			},
	{ 0x00034A1E, "GCareer_BankTrigs"		},
	{ 0x00034A1F, "GCareer_CarUnlocks"		},
	{ 0x00034B00, "DifficultyInfo"			},
	{ 0x00035020, "AcidEffects"				},
	{ 0x00035021, "AcidEmitters"			},
	{ 0x00037220, "Stream37220"				},
	{ 0x00037240, "Stream37240"				},
	{ 0x00037250, "Stream37250"				},
	{ 0x00037260, "Stream37260"				},
	{ 0x00037270, "Stream37270"				},
	{ 0x00039000, "STRBlocks"				},
	{ 0x00039001, "LangFont"				},
	{ 0x00039010, "Subtitles"				},
	{ 0x00039020, "MovieCatalog"			},
	{ 0x0003A100, "CompTPKBlock"			},
	{ 0x0003B200, "ICECatalog"				},
	{ 0x0003B800, "WWorld"					},
	{ 0x0003B801, "WCollisionPack"			},
	{ 0x0003B802, "WCollisionRaww"			},
	{ 0x0003B811, "NISScript"				},
	{ 0x0003B901, "Collision"				},
	{ 0x0003BC00, "EmitterLibrary"			},
	{ 0x0003BD00, "TPKSettings"				},
	{ 0x0003CE01, "Vinyl_Header"			},
	{ 0x0003CE02, "Vinyl_PointerTable"		},
	{ 0x0003CE04, "Vinyl_PathEntry"			},
	{ 0x0003CE05, "Vinyl_PathData"			},
	{ 0x0003CE06, "Vinyl_PathPoint"			},
	{ 0x0003CE07, "Vinyl_FillEffect"		},
	{ 0x0003CE08, "Vinyl_StrokeEffect"		},
	{ 0x0003CE09, "Vinyl_DropShadow"		},
	{ 0x0003CE0A, "Vinyl_InnerGlow"			},
	{ 0x0003CE0B, "Vinyl_ShadowEffect"		},
	{ 0x0003CE0C, "Vinyl_Gradient"			},
	{ 0x0003CE0E, "VinylDataHeader"			},
	{ 0x0003CE0F, "VinylCarEntries"			},
	{ 0x0003CE10, "VinylFloatMatrix"		},
	{ 0x0003CE11, "VinylVectorEntries"		},
	{ 0x0003CE12, "SkinRegionDB"			}, 
	{ 0x0003CE13, "VinylMetaData"			}, 
	{ 0x000B5846, "FX"						}, 
	{ 0x00135200, "Materials"				}, 
	{ 0x00E34009, "EAGLSkeleton"			}, 
	{ 0x00E34010, "EAGLAnimations"			}, 
	{ 0x30300200, "DDSTexture"				}, 
	{ 0x30300201, "ColorCube"				}, 
	{ 0x30300300, "PCAWater0"				}, 
	{ 0x33310001, "TPK_InfoPart1"			}, 
	{ 0x33310002, "TPK_InfoPart2"			}, 
	{ 0x33310003, "TPK_InfoPart3"			}, 
	{ 0x33310004, "TPK_InfoPart4"			}, 
	{ 0x33310005, "TPK_InfoPart5"			}, 
	{ 0x33312001, "TPK_AnimPart1"			}, 
	{ 0x33312002, "TPK_AnimPart2"			}, 
	{ 0x33320001, "TPK_DataPart1"			}, 
	{ 0x33320002, "TPK_DataPart2"			}, 
	{ 0x33320003, "TPK_DataPart3"			}, 
	{ 0x42704D67, "Nikki"					}, 
	{ 0x434B4241, "ABKC"					}, 
	{ 0x48434F4C, "LOCH"					}, 
	{ 0x4B415056, "VPAK"					}, 
	{ 0x52494F4D, "MOIR"					}, 
	{ 0x53219999, "MEMO"					}, 
	{ 0x55441122, "LZCompressed"			}, 
	{ 0x6468564D, "MVhd"					}, 
	{ 0x75736E47, "Gnsu"					}, 
	{ 0x80034100, "SpeedScenery"			}, 
	{ 0x80034602, "DBCarParts"				}, 
	{ 0x80034A00, "GCareer_Old"				}, 
	{ 0x80034A10, "GCareer"					}, 
	{ 0x80034A30, "GLimitations"			}, 
	{ 0x80036000, "EmitterTriggers"			}, 
	{ 0x80037020, "NISDescription"			}, 
	{ 0x80037050, "AnimDirectory"			}, 
	{ 0x8003B000, "QuickSpline"				}, 
	{ 0x8003B200, "IceCameraPart0"			}, 
	{ 0x8003B201, "IceCameraPart1"			}, 
	{ 0x8003B202, "IceCameraPart2"			}, 
	{ 0x8003B203, "IceCameraPart3"			}, 
	{ 0x8003B204, "IceCameraPart4"			}, 
	{ 0x8003B209, "IceSettings"				}, 
	{ 0x8003B500, "SoundStichs"				}, 
	{ 0x8003B810, "EventSequence"			}, 
	{ 0x8003B900, "DBCarBounds"				}, 
	{ 0x8003CE00, "VinylSystem"				}, 
	{ 0x8003CE03, "Vinyl_PathSet"			}, 
	{ 0x8003CE0D, "VinylDataTable"			}, 
	{ 0x80134000, "Geometry"				}, 
	{ 0x80135000, "ELights"					}, 
	{ 0xB0300100, "SpecialEffects"			}, 
	{ 0xB0300300, "PCAWeights"				}, 
	{ 0xB3300000, "TPKBlocks"				}, 
	{ 0xB3310000, "TPK_InfoBlock"			}, 
	{ 0xB3312000, "TPK_BinData"				}, 
	{ 0xB3312004, "TPK_AnimBlock"			}, 
	{ 0xB3320000, "TPK_DataBlock"			}
};

const std::unordered_map<std::uint32_t, std::string_view> TexturesFormatMap =
{
	{ 0x1A200152, "D3DFMT_DXT1"},
	{ 0x1A200153, "D3DFMT_DXT3"},
	{ 0x1A200154, "D3DFMT_DXT5"},
	{ 0x1A200171, "D3DFMT_DXN" },
	{ 0x4900102,  "D3DFMT_A8" },
	{ 0x28000102, "D3DFMT_L8" },
	{ 0x28280144, "D3DFMT_R5G6B5" },
	{ 0x28280145, "D3DFMT_R6G5B5" },
	{ 0x2A200B45, "D3DFMT_L6V5U5" },
	{ 0x28280143, "D3DFMT_X1R5G5B5" },
	{ 0x18280143, "D3DFMT_A1R5G5B5" },
	{ 0x1828014F, "D3DFMT_A4R4G4B4" },
	{ 0x2828014F, "D3DFMT_X4R4G4B4" },
	{ 0x1A20AB4F, "D3DFMT_Q4W4V4U4" },
	{ 0x800014A,  "D3DFMT_A8L8" },
	{ 0x2D20014A, "D3DFMT_G8R8" },
	{ 0x2D20AB4A, "D3DFMT_V8U8" },
	{ 0x1A220158, "D3DFMT_D16" },
	{ 0x28000158, "D3DFMT_L16" },
	{ 0x2DA2AB5E, "D3DFMT_R16F" },
	{ 0x2DA2AB5B, "D3DFMT_R16F_EXPAND" },
	{ 0x1A20014C, "D3DFMT_UYVY" },
	{ 0x1A20010C, "D3DFMT_LE_UYVY" },
	{ 0x1828014C, "D3DFMT_G8R8_G8B8" },
	{ 0x1828014B, "D3DFMT_R8G8_B8G8" },
	{ 0x1A20014B, "D3DFMT_YUY2" },
	{ 0x1A20010B, "D3DFMT_LE_YUY2" },
	{ 0x18280186, "D3DFMT_A8R8G8B8" },
	{ 0x18280086, "D3DFMT_LIN_A8R8G8B8" },
	{ 0x28280186, "D3DFMT_X8R8G8B8" },
	{ 0x1A200186, "D3DFMT_A8B8G8R8" },
	{ 0x2A200186, "D3DFMT_X8B8G8R8" },
	{ 0x2A200B86, "D3DFMT_X8L8V8U8" },
	{ 0x1A20AB86, "D3DFMT_Q8W8V8U8" },
	{ 0x182801B6, "D3DFMT_A2R10G10B10" },
	{ 0x282801B6, "D3DFMT_X2R10G10B10" },
	{ 0x1A2001B6, "D3DFMT_A2B10G10R10" },
	{ 0x1A202BB6, "D3DFMT_A2W10V10U10" },
	{ 0x8000199,  "D3DFMT_A16L16" },
	{ 0x2D200199, "D3DFMT_G16R16" },
	{ 0x2D20AB99, "D3DFMT_V16U16" },
	{ 0x282801B7, "D3DFMT_R10G11B11" },
	{ 0x282801B8, "D3DFMT_R11G11B10" },
	{ 0x2A20ABB7, "D3DFMT_W10V11U11" },
	{ 0x2A20ABB8, "D3DFMT_W11V11U10" },
	{ 0x2D22AB9F, "D3DFMT_G16R16F" },
	{ 0x2D22AB9C, "D3DFMT_G16R16F_EXPAND" },
	{ 0x280001A1, "D3DFMT_L32" },
	{ 0x2DA2ABA4, "D3DFMT_R32F"	},
	{ 0x1A20015A, "D3DFMT_A16B16G16R16" },
	{ 0x1A20AB5A, "D3DFMT_Q16W16V16U16" },
	{ 0x1A22AB60, "D3DFMT_A16B16G16R16F" },
	{ 0x1A22AB5D, "D3DFMT_A16B16G16R16F_EXPAND" },
	{ 0x80001A2,  "D3DFMT_A32L32" },
	{ 0x2D2001A2, "D3DFMT_G32R32" },
	{ 0x2D20ABA2, "D3DFMT_V32U32" },
	{ 0x2D22ABA5, "D3DFMT_G32R32F" },
	{ 0x1A2001A3, "D3DFMT_A32B32G32R32" },
	{ 0x1A20ABA3, "D3DFMT_Q32W32V32U32" },
	{ 0x1A22ABA6, "D3DFMT_A32B32G32R32F" },
	{ 0x28280106, "D3DFMT_LE_X8R8G8B8" },
	{ 0x18280106, "D3DFMT_LE_A8R8G8B8" },
	{ 0x28280136, "D3DFMT_LE_X2R10G10B10" },
	{ 0x18280136, "D3DFMT_LE_A2R10G10B10" },
	{ 0x1A20017A, "D3DFMT_DXT3A" },
	{ 0x1A20017D, "D3DFMT_DXT3A_1111" },
	{ 0x1A20017B, "D3DFMT_DXT5A" },
	{ 0x1A20017C, "D3DFMT_CTX1" },
	{ 0x2D200196, "D3DFMT_D24S8" },
	{ 0x2DA00196, "D3DFMT_D24X8" },
	{ 0x1A220197, "D3DFMT_D24FS8" },
	{ 0x1A2201A1, "D3DFMT_D32" }
};

namespace bb
{
	EGameVersion DetectNFSGameVersionFromEntries(std::vector<aFileDirectoryEntry>& entries)
	{
		std::uint32_t ProStreetCheck = nfr::api::getBinaryHash("TRACKS\\L6R_FE.BUN");
		auto find_result = std::find_if(entries.begin(), entries.end(), [ProStreetCheck](const aFileDirectoryEntry& entry) {
			return (entry.Hash == ProStreetCheck);
			});

		if (find_result != entries.end()) {
			return EGameVersion::ProStreetXenon;
		}

		return EGameVersion::Unknown;
	}

	EGameVersion DetectNFSGameVersionFromFiles()
	{
		nfr::api::path basePath = EngineFactory->getGameDirectory();
		if (EngineFactory->exists(basePath.append(std::to_string(nfr::api::getBinaryHash("TRACKS/L6R_FE.BUN"))))) {
			return EGameVersion::ProStreetXenon;
		}

		basePath = EngineFactory->getGameDirectory();
		if (EngineFactory->exists(basePath.append("TRACKS/L6R_FE.BUN"))) {
			return EGameVersion::ProStreetXenon;
		}

		return EGameVersion::Unknown;
	}
}