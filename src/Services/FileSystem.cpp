#include "FileSystem.h"
#include "LittleFS.h"

namespace Services {
  FileSystem *FileSystem::instance = nullptr;

  FileSystem::FileSystem() {
    mounted = LittleFS.begin();
    FSInfo fs_info;
    if (LittleFS.info(fs_info)) {
      Serial.println("LittleFS mounted:");
      Serial.println("  totalBytes=" + String(fs_info.totalBytes));
      Serial.println("  usedBytes=" + String(fs_info.usedBytes));
      Serial.println("  blockSize=" + String(fs_info.blockSize));
      Serial.println("  pageSize=" + String(fs_info.pageSize));
      Serial.println("  maxOpenFiles=" + String(fs_info.maxOpenFiles));
      Serial.println("  maxPathLength=" + String(fs_info.maxPathLength));
    } else {
      Serial.println("LittleFS not mounted.");
    }
  }

  void FileSystem::setup() {
  }

  void FileSystem::loop() {
  }
}