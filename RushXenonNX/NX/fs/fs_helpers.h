#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace FSHelpers {

    // Asegura que sdmc esté montado y crea la carpeta base si no existe.
    // baseDir por defecto: "sdmc:/RushXenonNX"
    bool EnsureMounted();

    // Crea baseDir y las subcarpetas pasadas en `subdirs`.
    // Ejemplo de subdirs: {"game","update","patched"}
    bool CreateBaseAndSubdirs(const char* baseDir = "sdmc:/RushXenonNX",
                              const std::vector<std::string>& subdirs = std::vector<std::string>{ "game", "update", "patched", "dlc" });

    // Crea una subcarpeta dentro de baseDir (no recursiva para subdir simple)
    bool CreateSubdir(const char* baseDir, const char* subdir);

} // namespace FSHelpers


// Lector sencillo de archivos binarios (futuro uso)
class BinaryReader {
public:
    BinaryReader();
    ~BinaryReader();

    // Abre el archivo en modo binario (rb)
    bool Open(const char* filepath);

    // Lee hasta 'bytes' en buffer, devuelve cantidad leída.
    size_t Read(void* buffer, size_t bytes);

    // Lee todo el archivo en out vector (true si OK)
    bool ReadAll(std::vector<uint8_t>& out);

    // Lee desde offset (fseek) y devuelve leídos
    size_t ReadAt(size_t offset, void* buffer, size_t bytes);

    // Devuelve tamaño del archivo o 0 si no está abierto o error
    size_t GetSize() const;

    void Close();
    bool IsOpen() const;

private:
    FILE* m_file;
    size_t m_size;

    // helper interno
    bool compute_size();
};
