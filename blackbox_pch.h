#ifdef SPEED_TOOLS
#include <mspack/mspack.h>
#include <mspack/lzx.h>
#endif

#include <spdlog/spdlog.h>

#include "blackbox.h"

struct aFileDirectoryEntry
{
	std::uint32_t Hash;
	std::int32_t FileNumber;
	std::int32_t LocalSectorOffset;
	std::int32_t TotalSectorOffset;
	std::int32_t Size;
	std::uint32_t Checksum;
};

struct aChunk
{
	std::uint32_t Id;
	std::int32_t Size;

	char* getDataPtr() const
	{
		return (char*)this + sizeof(aChunk);
	}

	template<typename T>
	T* getDataPtr() const
	{
		return (T*)getDataPtr();
	}

	template<typename T>
	T* getAlignedPtr(std::ptrdiff_t align) const
	{
		return (T*)(((std::ptrdiff_t)getDataPtr() + (align - 1)) & (~(align - 1)));
	}

	std::size_t getSize() const
	{
		return Size;
	}

	std::size_t getAlignedSize(std::size_t align) const
	{
		return (getDataPtr() - getAlignedPtr<char>(align)) + Size;
	}
};


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