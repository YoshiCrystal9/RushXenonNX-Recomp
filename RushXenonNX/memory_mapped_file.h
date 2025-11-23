#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#if defined(_WIN32)
#   include <Windows.h>
#elif !defined(__SWITCH__)
#   include <filesystem>
#   include <sys/mman.h>
#endif

struct MemoryMappedFile {
#if defined(_WIN32)
    HANDLE fileHandle = nullptr;
    HANDLE fileMappingHandle = nullptr;
    LPVOID fileView = nullptr;
    LARGE_INTEGER fileSize = {};
#elif defined(__SWITCH__)
    // On Switch we don't have mmap; we store the file buffered in memory.
    FILE *file = nullptr;
    uint8_t *buffer = nullptr;
    size_t bufferSize = 0;
#else
    int fileHandle = -1;
    void *fileView = MAP_FAILED;
    off_t fileSize = 0;
#endif

    MemoryMappedFile();
    // Constructors: accept std::string (portable). If std::filesystem is available also provide the path overload.
    MemoryMappedFile(const std::string &path);
#if !defined(__SWITCH__)
    MemoryMappedFile(const std::filesystem::path &path);
#endif

    MemoryMappedFile(MemoryMappedFile &&other);
    ~MemoryMappedFile();

    // open by string path (portable)
    bool open(const std::string &path);
#if !defined(__SWITCH__)
    bool open(const std::filesystem::path &path);
#endif

    void close();
    bool isOpen() const;
    uint8_t *data() const;
    size_t size() const;
};
