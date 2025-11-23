#pragma once
#include <string>
#include <cstdio>

class SDLogger {
public:
    // Inicializa el logger: monta SD, crea carpeta, vacía contenido y abre archivo
    static bool Init(const char* baseDir = "sdmc:/RushXenonNX", const char* filename = "log.txt");

    // Log formateado estilo printf
    static void Log(const char* fmt, ...);

    // Cierra el archivo de log
    static void Close();

private:
    static FILE* s_file;
    static std::string s_baseDir;
    static std::string s_filePath;

    static bool mkdir_recursive(const char* path);
    static bool remove_all_in_dir(const char* dirpath);
};
