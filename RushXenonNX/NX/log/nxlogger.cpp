#include "nxlogger.h"
#include <cstring>
#include <switch.h>
#include <iostream>
#include <cstdarg>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

FILE* SDLogger::s_file = nullptr;
std::string SDLogger::s_baseDir = "";
std::string SDLogger::s_filePath = "";

bool SDLogger::Init(const char* baseDir, const char* filename) {
    Result rc = fsdevMountSdmc();
    if (R_FAILED(rc)) {
        std::cout << "Logger: fsdevMountSdmc failed at: 0x" << std::hex << rc << std::dec << "\n";
    }

    s_baseDir = baseDir;
    s_filePath = std::string(baseDir) + "/" + filename;

    mkdir_recursive(baseDir);
    remove_all_in_dir(baseDir);

    // === NUEVO: CREAR SUBDIRECTORIOS ===
    std::string subdirs[] = {
        std::string(baseDir) + "/game",
        std::string(baseDir) + "/update",
        std::string(baseDir) + "/dlc",
        std::string(baseDir) + "/patched"
    };

    for (const auto& dir : subdirs) {
        mkdir_recursive(dir.c_str());
    }
    // ===================================

    s_file = fopen(s_filePath.c_str(), "a");
    if (!s_file) {
        std::cout << "Logger: ERROR opening " << s_filePath << ": " << strerror(errno) << "\n";
        return false;
    }

    setvbuf(s_file, nullptr, _IOLBF, 0);
    Log("=== Logger STARTED ===");
    return true;
}


void SDLogger::Log(const char* fmt, ...) {
    if (!s_file) return;

    char buffer[1024];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    fprintf(s_file, "%s\n", buffer);
    fflush(s_file);

    std::cout << buffer << std::endl;
}

void SDLogger::Close() {
    if (s_file) {
        Log("=== Logger End ===");
        fclose(s_file);
        s_file = nullptr;
    }
}

bool SDLogger::mkdir_recursive(const char* path) {
    std::string p = path;

    while (!p.empty() && p.back() == '/') p.pop_back();
    if (p.empty()) return false;

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
                    // Ignorar errores excepto si no es "ya existe"
                }
            }
        }
    }
    return true;
}

bool SDLogger::remove_all_in_dir(const char* dirpath) {
    DIR* dir = opendir(dirpath);
    if (!dir) return false;

    struct dirent* ent;
    bool ok = true;

    while ((ent = readdir(dir)) != nullptr) {
        const char* name = ent->d_name;
        if (!strcmp(name, ".") || !strcmp(name, "..")) continue;

        // solo borrar el archivo de log
        std::string full = std::string(dirpath) + "/" + name;

        if (full == s_filePath) {
            if (unlink(full.c_str()) != 0) {
                ok = false;
            }
        }
    }

    closedir(dir);
    return ok;
}

