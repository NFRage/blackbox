#ifdef SPEED_TOOLS
#include <mspack/mspack.h>
#include <mspack/lzx.h>
#endif

#include <spdlog/spdlog.h>

#include "blackbox.h"
#include "aware.h"

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