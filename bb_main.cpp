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

nfr::api::binary_hash_map<std::string> EntriesMap;
BLACKBOX_PLUGIN_API spdlog::logger* GameLogger;

bb::EGameVersion GameVersion = bb::EGameVersion::Unknown;

namespace bb
{


bool UnpackXboxData()
{
	std::vector<FileHandle> files;
	std::vector<aFileDirectoryEntry> directoryEntries;

	auto ProcessFile = [&files](std::string& inFileName) -> bool {
#ifdef NFRAGE_TOOLS
		auto DecompressXboxData = [](std::vector<std::uint8_t>& compressedBuffer, std::vector<std::uint8_t>& uncompressedBuffer) -> bool {
			if (!DecompressXbox(
				compressedBuffer,
				uncompressedBuffer
			)) {
				return false;
			}

			return true;
		};
#endif
		std::size_t beginOffset = 0;
		if (std::strncmp(inFileName.data(), "Outfile: ", 9) == 0) {
			beginOffset = 9;
		}

		std::string pathToFile = "NFS/";
		std::string fileName = &inFileName.at(beginOffset);	// "Outfile: "
		std::size_t foundNewLine = fileName.find_first_of('\r');
		if (foundNewLine != std::size_t(-1)) {
			fileName.erase(foundNewLine, 1);
		}

		if (std::strncmp(fileName.data(), "NFS", 3) != 0) {
			pathToFile += fileName;
		} else {
			pathToFile = fileName;
		}

		std::string directoryName = fileName;
		directoryName.erase(0, 2);
		directoryName.erase(directoryName.find_last_of('.'), 4);

		nfr::api::SafeInterface<nfr::api::IStream> assetStream = EngineFactory->openFile(nfr::api::EStreamFlags::ReadFlag, pathToFile.c_str());
		if (assetStream->isOpen() && assetStream->getSize() >= 4) {
			std::uint32_t headerMagic = 0;
			assetStream->read(&headerMagic, sizeof(std::uint32_t));
			assetStream->seek(nfr::api::EStreamMode::Set, 0);

			EBigFilesCompression compressionType = EBigFilesCompression::None;
			if (!std::memcmp(&headerMagic, "\x0F\xF5\x12\xEE", 4)) {
				compressionType = EBigFilesCompression::XbCompressNative;
			} else if (!std::memcmp(&headerMagic, "\xee\x12\xf5\x0f", 4)) {
				compressionType = EBigFilesCompression::XbCompressNative;
			} else if (!std::memcmp(&headerMagic, "\x0F\xF5\x12\xED", 4)) {
				compressionType = EBigFilesCompression::XbCompressDecode;
			} else if (!std::memcmp(&headerMagic, "\xed\x12\xf5\x0f", 4)) {
				compressionType = EBigFilesCompression::XbCompressDecode;
			}

			nfr::api::path newFilePath = EngineFactory->getTempDirectory();
			newFilePath.append(fileName);

			switch (compressionType) {

			case EBigFilesCompression::None: {
				std::error_code err;
				if (!EngineFactory->exists(newFilePath)) {
					dbg::Log("The file \"{}\" has no compression. Copying to temp directory..", fileName);
					if (!std::filesystem::copy_file(pathToFile, newFilePath, err)) {
						dbg::Warning("Can't copy file from \"{}\" to \"{}\". Skipping file...",
							pathToFile,
							newFilePath.generic_string()
						);

						return false;
					}
				}
			}
										   break;

#ifdef NFRAGE_TOOLS
			case EBigFilesCompression::XbCompressNative: {
				std::vector<char> compressedBuffer;
				std::vector<char> uncompressedBuffer;

				compressedBuffer.resize(assetStream->getSize());
				assetStream->read(compressedBuffer.data(), compressedBuffer.size());

				XCompressNativeHeader* nativeHeader = reinterpret_cast<XCompressNativeHeader*>(compressedBuffer.data());
				const std::uint64_t uncompressedSize = (std::uint64_t(nativeHeader->UncompressedSizeHigh) << 32) + std::uint64_t(nativeHeader->UncompressedSizeLow);
				const std::uint64_t compressedSize = (std::uint64_t(nativeHeader->CompressedSizeHigh) << 32) + std::uint64_t(nativeHeader->CompressedSizeLow);

				uncompressedBuffer.resize(uncompressedSize);
				//if (!DecompressXboxData(compressedBuffer, uncompressedBuffer)) {
				//	return false;
				//}

				api::SafeInterface<api::IStream> unpackedStream = fs::OpenFile(api::EStreamFlags::WriteFlag, newFilePath);
				if (!unpackedStream->isOpen()) {
					dbg::Warning("Can't create unpacked version of. Skipping file...", fileName);
					return false;
				}

				unpackedStream->write(uncompressedBuffer.data(), uncompressedBuffer.size());
			}
													   break;
#endif
			default:
				dbg::Warning("The unsupported file format in file \"{}\". Skipping file...", fileName);
				break;
			}

			uint32_t openAlreadyExistingFile = nfr::api::EStreamFlags::ReadFlag | nfr::api::EStreamFlags::AppendFlag;
			nfr::api::SafeInterface<nfr::api::IStream> readyToWorkFile = EngineFactory->openFile(openAlreadyExistingFile, newFilePath);
			if (!readyToWorkFile->isOpen()) {
				dbg::Warning("Can't open unpacked version of {}. Skipping file...", fileName);
				return false;
			}

			files.emplace_back(std::move(FileHandle(std::move(readyToWorkFile), std::move(fileName))));
		}

		return true;
	};


	if (EngineFactory->exists("ZDIRKeys.txt")) {
		dbg::Log("Loading keys from \"ZDIRKeys.txt\"...");
		nfr::api::SafeInterface<nfr::api::IStream> stream = EngineFactory->openFile(nfr::api::EStreamFlags::ReadFlag, "ZDIRKeys.txt");
		if (!stream->isOpen()) {
			dbg::Error("Can't open \"ZDIRKeys.txt\" file. Aborting...");
			return false;
		}

		std::string readedKey;
		while (stream->getLine(readedKey)) {
			std::size_t foundNewLine = readedKey.find_first_of('\r');
			if (foundNewLine != std::size_t(-1)) {
				readedKey.erase(foundNewLine, 1);
			}

			if (!readedKey.empty()) {
				std::uint32_t hash = nfr::api::getBinaryUpperHash(readedKey.c_str());
				EntriesMap[hash] = readedKey;
			}
		}
	} else {
		dbg::Warning("Can't find file \"ZDIRKeys.txt\". That means that you're unpacking data with hashes only...");
	}

	{
		nfr::api::path datBinFile = EngineFactory->getGameDirectory();
		datBinFile.append("DAT.BIN");
		if (EngineFactory->exists(datBinFile)) {
			dbg::Log("Skipping data unpacking (already unpacked).");
			return true;
		}
	}

	{
		nfr::api::SafeInterface<nfr::api::IStream> stream = EngineFactory->openFile(nfr::api::EStreamFlags::ReadFlag, "NFS/ZDIR.BIN");
		if (!stream->isOpen()) {
			dbg::Error("Can't open \"NFS/ZDIR.BIN\" file. Aborting...");
			return false;
		}

		std::uint64_t entriesCount = stream->getSize() / sizeof(aFileDirectoryEntry);
		if (entriesCount == 0) {
			dbg::Error("Invalid size of \"NFS/ZDIR.BIN\" file. Aborting...");
			return false;
		}

		directoryEntries.resize(entriesCount);
		stream->read(reinterpret_cast<char*>(directoryEntries.data()), entriesCount * sizeof(aFileDirectoryEntry));
	}

	{
		dbg::Log("Parsing \"NAMES.HOO\" file...");
		std::string readString;
		int32_t counter = 0;
		nfr::api::SafeInterface<nfr::api::IStream> stream = EngineFactory->openFile(nfr::api::EStreamFlags::ReadFlag, "NFS/NAMES.HOO");

		while (stream->getLine(readString)) {
			if (!ProcessFile(readString)) {
				continue;
			}

			counter++;
		}

		if (counter == 0) {
			dbg::Error("No files has been processed. Aborting...");
			return false;
		}
	}

	for (const auto& entry : directoryEntries) {
		if (entry.FileNumber >= files.size()) {
			continue;
		}

		files[entry.FileNumber].entries.emplace_back(entry);
	}

	for (auto& file : files) {
		std::sort(file.entries.begin(), file.entries.end(), [](aFileDirectoryEntry& left, aFileDirectoryEntry& right) {
			return left.TotalSectorOffset < right.TotalSectorOffset;
		});

		if (GameVersion == EGameVersion::Unknown) {
			GameVersion = DetectNFSGameVersionFromEntries(file.entries);
		}
	}

	for (auto& file : files) {
		dbg::Log("    {} with {} entries", file.fileName, file.entries.size());

		for (const auto& entry : file.entries) {
			auto entriesIt = EntriesMap.find(entry.Hash);
			bool bStringFound = entriesIt != EntriesMap.end();
			std::string fileName = (bStringFound ? (*entriesIt).second.c_str() : std::to_string(entry.Hash));
            for (char& sym : fileName) {
                if (sym == '\\') {
                    sym = '/';
                }
            }
            
			dbg::Log("        entry {}: crc32: {:#06x}; hash: {:#06x} ",
				fileName,
				entry.Checksum,
				entry.Hash
			);

			nfr::api::path newFilePath = EngineFactory->getGameDirectory();
			newFilePath.append(fileName);
			if (EngineFactory->exists(newFilePath)) {
				continue;
			}

			nfr::api::path folderPath = newFilePath;
			folderPath.remove_filename();
			if (!EngineFactory->exists(folderPath)) {
				std::filesystem::create_directories(folderPath);
			}

			nfr::api::SafeInterface<nfr::api::IStream> newFileStream = EngineFactory->openFile(nfr::api::EStreamFlags::WriteFlag, newFilePath);
			if (!newFileStream->isOpen()) {
				dbg::Warning("Can't create file {}. Skipping this one", fileName);
				continue;
			}

			if (entry.Size == 0) {
				dbg::Log("The file {} is empty. Skipping this one", fileName);
				continue;
			}

			const std::int64_t fileSize = file.stream->getSize();
			const std::uint32_t localOffset = entry.LocalSectorOffset << 11;
			if (fileSize <= localOffset) {
				dbg::Warning("THIS IS ILLEGAL!!!");
				dbg::Warning("File - {}", fileName);
				dbg::Warning("File size - {}; Sector offset - {}", fileSize, localOffset);
				continue;
			}

			std::int64_t endOffsetPosition = localOffset + entry.Size;
			if (fileSize < endOffsetPosition) {
				dbg::Warning("THIS IS ILLEGAL!!!");
				dbg::Warning("File - {}", fileName);
				dbg::Warning("Offset + Sector size ({}) is bigger than file size ({})", endOffsetPosition, fileSize);
				continue;
			}

			std::vector<char> readBuffer;
			readBuffer.resize(entry.Size);

			file.stream->seek(nfr::api::EStreamMode::Set, localOffset);
			file.stream->read(readBuffer.data(), readBuffer.size());

			newFileStream->write(readBuffer.data(), readBuffer.size());
		}
	}

	directoryEntries.clear();
	files.clear();

	std::error_code err;
	dbg::Log("Cleaning up after unpacking...");
	for (const auto& it : std::filesystem::recursive_directory_iterator(EngineFactory->getTempDirectory())) {
		std::filesystem::remove(it, err);
		if (err) {
			dbg::Warning("Can't remove directory {} ({})", it.path().generic_string(), err.message());
			return false;
		}
	}

	return true;
}

bool PostInitializePackedDatabase()
{
	std::error_code err;
	nfr::api::path moviesDir = EngineFactory->getWorkingDirectory();
	moviesDir.append("Movies");
	if (EngineFactory->exists(moviesDir)) {
		dbg::Log("Movies directory exists. Trying to copy it to main folder...");
		nfr::api::path newMoviesDir = EngineFactory->getGameDirectory();
		newMoviesDir.append("Movies");
		if (!EngineFactory->exists(newMoviesDir)) {
			std::filesystem::create_directory(newMoviesDir, err);
			if (err) {
				dbg::Error("Can't copy movies directory to game directory. Aborting...");
				return false;
			}

			dbg::Log("Copying to game folder...");
			for (const auto& it : std::filesystem::directory_iterator(moviesDir, err)) {
				nfr::api::path newFilePath = newMoviesDir;
				nfr::api::path fileName = it.path().filename();
				newFilePath /= fileName;

				std::string fileNameString = fileName.generic_string();
				if (!EngineFactory->exists(newFilePath)) {
					dbg::Log("Copying {}...", fileNameString);
					std::filesystem::copy(it.path(), newFilePath, err);
					if (err) {
						dbg::Warning("Failed to copy file {}. Skipping file...", fileNameString);
						continue;
					}
				}
			}

		} else {
			dbg::Log("Already copied! Skipping...");
		}
	}

	return true;
}


}
