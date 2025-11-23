#include "memory_mapped_file.h"

#if defined(_WIN32)
#   include <cstdio>
#   include <cstring>
#else
#   include <cerrno>
#   include <cstring>
#   include <cstdio>
#   include <fcntl.h>
#   include <unistd.h>
#endif

MemoryMappedFile::MemoryMappedFile()
{
    // Default constructor.
}

MemoryMappedFile::MemoryMappedFile(const std::string &path)
{
    open(path);
}

#if !defined(__SWITCH__)
MemoryMappedFile::MemoryMappedFile(const std::filesystem::path &path)
{
    open(path);
}
#endif

MemoryMappedFile::~MemoryMappedFile()
{
    close();
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile &&other)
{
#if defined(_WIN32)
    fileHandle = other.fileHandle;
    fileMappingHandle = other.fileMappingHandle;
    fileView = other.fileView;
    fileSize = other.fileSize;

    other.fileHandle = nullptr;
    other.fileMappingHandle = nullptr;
    other.fileView = nullptr;
    other.fileSize.QuadPart = 0;
#elif defined(__SWITCH__)
    file = other.file;
    buffer = other.buffer;
    bufferSize = other.bufferSize;

    other.file = nullptr;
    other.buffer = nullptr;
    other.bufferSize = 0;
#else
    fileHandle = other.fileHandle;
    fileView = other.fileView;
    fileSize = other.fileSize;

    other.fileHandle = -1;
    other.fileView = MAP_FAILED;
    other.fileSize = 0;
#endif
}

#if defined(_WIN32)

bool MemoryMappedFile::open(const std::string &path)
{
    // Convert std::string to wide for CreateFileW
    std::wstring wpath;
    wpath.assign(path.begin(), path.end());
    fileHandle = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "CreateFileW failed with error %lu.\n", GetLastError());
        fileHandle = nullptr;
        return false;
    }

    if (!GetFileSizeEx(fileHandle, &fileSize))
    {
        fprintf(stderr, "GetFileSizeEx failed with error %lu.\n", GetLastError());
        CloseHandle(fileHandle);
        fileHandle = nullptr;
        return false;
    }

    fileMappingHandle = CreateFileMappingW(fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (fileMappingHandle == nullptr)
    {
        fprintf(stderr, "CreateFileMappingW failed with error %lu.\n", GetLastError());
        CloseHandle(fileHandle);
        fileHandle = nullptr;
        return false;
    }

    fileView = MapViewOfFile(fileMappingHandle, FILE_MAP_READ, 0, 0, 0);
    if (fileView == nullptr)
    {
        fprintf(stderr, "MapViewOfFile failed with error %lu.\n", GetLastError());
        CloseHandle(fileMappingHandle);
        CloseHandle(fileHandle);
        fileMappingHandle = nullptr;
        fileHandle = nullptr;
        return false;
    }

    return true;
}

#elif defined(__SWITCH__)

/*
 * Switch (libnx) implementation:
 * - No mmap. Read entire file into heap buffer and expose pointer.
 * - Caller must ensure filesystem is mounted (fsdevMountSdmc/romfsInit) before opening.
 */

bool MemoryMappedFile::open(const std::string &path)
{
    // Close anything already open
    close();

    file = std::fopen(path.c_str(), "rb");
    if (!file)
    {
        fprintf(stderr, "fopen for %s failed with error %s.\n", path.c_str(), std::strerror(errno));
        return false;
    }

    if (std::fseek(file, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "fseek failed with error %s.\n", std::strerror(errno));
        std::fclose(file);
        file = nullptr;
        return false;
    }

    long sz = std::ftell(file);
    if (sz < 0)
    {
        fprintf(stderr, "ftell failed with error %s.\n", std::strerror(errno));
        std::fclose(file);
        file = nullptr;
        return false;
    }

    // Return to beginning and allocate buffer
    if (std::fseek(file, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "fseek (rewind) failed with error %s.\n", std::strerror(errno));
        std::fclose(file);
        file = nullptr;
        return false;
    }

    bufferSize = static_cast<size_t>(sz);
    if (bufferSize == 0)
    {
        // empty file -> allocate 1 byte to ensure data() is non-null (optional)
        buffer = nullptr;
        // keep file open? not necessary; close and treat as open with zero size
        std::fclose(file);
        file = nullptr;
        return true;
    }

    buffer = static_cast<uint8_t*>(std::malloc(bufferSize));
    if (!buffer)
    {
        fprintf(stderr, "malloc of %zu bytes failed.\n", bufferSize);
        std::fclose(file);
        file = nullptr;
        bufferSize = 0;
        return false;
    }

    size_t read = std::fread(buffer, 1, bufferSize, file);
    if (read != bufferSize)
    {
        fprintf(stderr, "fread read %zu of %zu bytes for %s (error %s).\n", read, bufferSize, path.c_str(), std::strerror(errno));
        std::free(buffer);
        buffer = nullptr;
        bufferSize = 0;
        std::fclose(file);
        file = nullptr;
        return false;
    }

    // We can close the FILE handle after reading (we keep buffer in memory)
    std::fclose(file);
    file = nullptr;

    return true;
}

#else // POSIX (non-switch)

bool MemoryMappedFile::open(const std::string &path)
{
    // allow calling with std::string; same as original POSIX implementation
    fileHandle = ::open(path.c_str(), O_RDONLY);
    if (fileHandle == -1)
    {
        fprintf(stderr, "open for %s failed with error %s.\n", path.c_str(), strerror(errno));
        return false;
    }

    fileSize = lseek(fileHandle, 0, SEEK_END);
    if (fileSize == (off_t)(-1))
    {
        fprintf(stderr, "lseek failed with error %s.\n", strerror(errno));
        ::close(fileHandle);
        fileHandle = -1;
        return false;
    }

    fileView = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fileHandle, 0);
    if (fileView == MAP_FAILED)
    {
        fprintf(stderr, "mmap failed with error %s.\n", strerror(errno));
        ::close(fileHandle);
        fileHandle = -1;
        return false;
    }

    return true;
}

#endif // end platform-specific open implementations

#if !defined(__SWITCH__) && !defined(_WIN32)
bool MemoryMappedFile::open(const std::filesystem::path &path)
{
    return open(path.string());
}
#endif

void MemoryMappedFile::close()
{
#if defined(_WIN32)
    if (fileView != nullptr)
    {
        UnmapViewOfFile(fileView);
        fileView = nullptr;
    }

    if (fileMappingHandle != nullptr)
    {
        CloseHandle(fileMappingHandle);
        fileMappingHandle = nullptr;
    }

    if (fileHandle != nullptr)
    {
        CloseHandle(fileHandle);
        fileHandle = nullptr;
    }
#elif defined(__SWITCH__)
    if (buffer)
    {
        std::free(buffer);
        buffer = nullptr;
    }
    bufferSize = 0;

    if (file)
    {
        std::fclose(file);
        file = nullptr;
    }
#else
    if (fileView != MAP_FAILED)
    {
        munmap(fileView, fileSize);
        fileView = MAP_FAILED;
    }

    if (fileHandle != -1)
    {
        ::close(fileHandle);
        fileHandle = -1;
    }
#endif
}

bool MemoryMappedFile::isOpen() const
{
#if defined(_WIN32)
    return (fileView != nullptr);
#elif defined(__SWITCH__)
    return (buffer != nullptr) || (bufferSize == 0); // empty files considered open
#else
    return (fileView != MAP_FAILED);
#endif
}

uint8_t *MemoryMappedFile::data() const
{
#if defined(_WIN32)
    return reinterpret_cast<uint8_t *>(fileView);
#elif defined(__SWITCH__)
    return buffer;
#else
    return reinterpret_cast<uint8_t *>(fileView);
#endif
}

size_t MemoryMappedFile::size() const
{
#if defined(_WIN32)
    return fileSize.QuadPart;
#elif defined(__SWITCH__)
    return bufferSize;
#else
    return static_cast<size_t>(fileSize);
#endif
}
