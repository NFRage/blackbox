#include "blackbox_pch.h"

namespace bb
{ 

bool BBGamePluginInstance::isCompatible() const
{
	return true;
}

bool BBGamePluginInstance::isRunnable() const
{
	return true;
}

bool BBGamePluginInstance::initialize()
{
	if (GameLogger == nullptr) {
		GameLogger = reinterpret_cast<spdlog::logger*>(EngineFactory->getGameLogger());
	}

	auto createDirectories = [](const char* pathToAppend) {
		nfr::api::path resourcesDirectory = EngineFactory->getResourcesDirectory();
		resourcesDirectory.append(pathToAppend);
		if (!std::filesystem::exists(resourcesDirectory)) {
			std::filesystem::create_directories(resourcesDirectory);
		}
	};

	createDirectories("textures");
	createDirectories("materials");
	createDirectories("sounds");
	createDirectories("scenes");
	createDirectories("meshes");
	createDirectories("prefabs");
	createDirectories("textures");
	createDirectories("movies");
	createDirectories("ui");

	if (EngineFactory->exists("NFS/ZDIR.BIN")) {
		dbg::Verbose("");
		dbg::Verbose("#################################################");
		dbg::Verbose("#");
		dbg::Verbose("# Beginning resource initalizing...");
		dbg::Verbose("#");
		dbg::Verbose("#################################################");
		dbg::Verbose("");
		dbg::Log("The \"NFS/ZDIR.BIN\" file was found. Trying to work with packed database...");
		if (!UnpackXboxData()) {
			return false;
		}

		if (GameVersion == EGameVersion::Unknown) {
			GameVersion = DetectNFSGameVersionFromFiles();
		}

		dbg::Log("Detected \"{}\" version of the game.", GetNFSString(GameVersion));
		if (!PostInitializePackedDatabase()) {
			return false;
		}

		dbg::Verbose("");
		dbg::Verbose("#################################################");
		dbg::Verbose("#");
		dbg::Verbose("# Resources initializing completed.");
		dbg::Verbose("#");
		dbg::Verbose("#################################################");
		dbg::Verbose("");

		if (!LoadChunkedFile("GLOBAL/GLOBALA.BUN")) {
			return false;
		}

		if (!LoadChunkedFile("GLOBAL/UniqueBootTextures.bin")) {
			return false;
		}

		if (!LoadCompressedFile("GLOBAL/GlobalB.lzc")) {
			return false;
		}
	}

	return true;
}

void BBGamePluginInstance::destroy()
{
}

bool BBGamePluginInstance::tick(float dt)
{
	return false;
}

long BBGamePluginInstance::addRef()
{
	return refCount.fetch_add(1);
}

long BBGamePluginInstance::release()
{
	long returnRefCount = refCount.fetch_sub(1);
	if (returnRefCount == 0) {
		delete this;
	}

	return returnRefCount;
}

}