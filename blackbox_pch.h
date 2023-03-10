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
#ifdef NFRAGE_TOOLS
#include <mspack/mspack.h>
#include <mspack/lzx.h>
#endif

#include <spdlog/spdlog.h>

#include "blackbox.h"
#include "bb_aware.h"
#include "bb_compression.h"
#include "bb_textures.h"
#include "bb_structs.h"
#include "bb_chunk.h"
#include "bb_game.h"
#include "bb_main.h"

#include "blackbox_instance.h"

extern const std::unordered_map<std::uint32_t, std::string_view> NfsChunkIdMap;
extern const std::unordered_map<std::uint32_t, std::string_view> TexturesFormatMap;
extern nfr::api::binary_hash_map<std::string> EntriesMap;
extern bb::EGameVersion GameVersion;