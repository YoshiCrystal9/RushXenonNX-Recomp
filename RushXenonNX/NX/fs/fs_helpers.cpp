#include "fs_helpers.h"
#include "../log/nxlogger.h"
#include <switch.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <vector>
#include <iostream> // opcional para debug si no usas SDLogger
#include <unistd.h>

// Si quieres loguear con tu SDLogger, descomenta la siguiente línea
// #include "logger.h"

using namespace FSHelpers;

static bool mkdir_recursive_internal(const std::string& path) {
    // normaliza quitando '/' final
    std::string p = path;
    while (!p.empty() && p.back() == '/') p.pop_back();
    if (p.empty()) return false;

    // si comienza por sdmc: dejarlo
    size_t pos = 0;
    if (p.find("sdmc:") == 0) {
        pos = 5;
        if (p.size() > 5 && p[5] == '/') pos = 6;
    }

    for (size_t i = pos; i <= p.size(); ++i) {
        if (i == p.size() || p[i] == '/') {
            std::string sub = p.substr(0, i);
            if (sub.empty()) continue;
            if (mkdir(sub.c_str(), 0777) != 0) {
                if (errno != EEXIST) {
                    // fallo real
                    return false;
                }
            }
        }
    }
    return true;
}

bool FSHelpers::EnsureMounted() {
    Result rc = fsdevMountSdmc();
    if (R_FAILED(rc)) {
        // si dispones de SDLogger, úsalo; si no, imprimir en consola
        SDLogger::Log("FSHelpers: fsdevMountSdmc failed: 0x%08X", rc);
        //std::cerr << "FSHelpers: fsdevMountSdmc failed: 0x" << std::hex << rc << std::dec << "\n";
        return false;
    }
    return true;
}

bool FSHelpers::CreateBaseAndSubdirs(const char* baseDir, const std::vector<std::string>& subdirs) {
    if (!EnsureMounted()) return false;

    // crear base (recursivamente)
    if (!mkdir_recursive_internal(baseDir)) {
        SDLogger::Log("FSHelpers: mkdir_recursive_internal failed for %s", baseDir);
        //std::cerr << "FSHelpers: mkdir_recursive_internal failed for " << baseDir << "\n";
        return false;
    }

    // crear subdirs
    for (const auto& sd : subdirs) {
        std::string full = std::string(baseDir) + "/" + sd;
        if (!mkdir_recursive_internal(full)) {
            SDLogger::Log("FSHelpers: failed to create subdir %s", full.c_str());
            //std::cerr << "FSHelpers: failed to create subdir " << full << "\n";
            return false;
        }
    }

    SDLogger::Log("FSHelpers: created base and subdirs in %s", baseDir);
    return true;
}

bool FSHelpers::CreateSubdir(const char* baseDir, const char* subdir) {
    if (!EnsureMounted()) return false;
    std::string full = std::string(baseDir) + "/" + subdir;
    return mkdir_recursive_internal(full);
}

/* ================= BinaryReader ================= */

BinaryReader::BinaryReader() : m_file(nullptr), m_size(0) {}

BinaryReader::~BinaryReader() {
    Close();
}

bool BinaryReader::Open(const char* filepath) {
    if (m_file) Close();

    m_file = fopen(filepath, "rb");
    if (!m_file) {
        SDLogger::Log("BinaryReader: failed to open %s: %s", filepath, strerror(errno));
        //std::cerr << "BinaryReader: failed to open " << filepath << ": " << strerror(errno) << "\n";
        m_size = 0;
        return false;
    }

    if (!compute_size()) {
        Close();
        return false;
    }

    return true;
}

bool BinaryReader::compute_size() {
    if (!m_file) return false;
    if (fseek(m_file, 0, SEEK_END) != 0) return false;
    long s = ftell(m_file);
    if (s < 0) return false;
    m_size = static_cast<size_t>(s);
    rewind(m_file);
    return true;
}

size_t BinaryReader::Read(void* buffer, size_t bytes) {
    if (!m_file || !buffer || bytes == 0) return 0;
    size_t r = fread(buffer, 1, bytes, m_file);
    return r;
}

size_t BinaryReader::ReadAt(size_t offset, void* buffer, size_t bytes) {
    if (!m_file || !buffer || bytes == 0) return 0;
    if (fseek(m_file, (long)offset, SEEK_SET) != 0) return 0;
    size_t r = fread(buffer, 1, bytes, m_file);
    return r;
}

bool BinaryReader::ReadAll(std::vector<uint8_t>& out) {
    if (!m_file) return false;
    if (!compute_size()) return false;
    out.clear();
    out.resize(m_size);
    if (m_size == 0) return true; // archivo vacío
    rewind(m_file);
    size_t read = fread(out.data(), 1, m_size, m_file);
    return read == m_size;
}

size_t BinaryReader::GetSize() const {
    return m_size;
}

void BinaryReader::Close() {
    if (m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
    m_size = 0;
}

bool BinaryReader::IsOpen() const {
    return m_file != nullptr;
}
