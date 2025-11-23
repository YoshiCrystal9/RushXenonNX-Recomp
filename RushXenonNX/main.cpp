#include <switch.h>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <vector>
#include "nx/log/nxlogger.h"
#include "nx/fs/fs_helpers.h"
#include "memory.h"
#include "guest_thread.h"
#include "heap.h"
#include "xdbf_wrapper.h"
#include "image.h"

Memory g_memory;
Heap g_userHeap;
BinaryReader br;
XDBFWrapper g_xdbfWrapper;

uint32_t LdrLoadModule() {
    SDLogger::Log("=== LdrLoadModule START ===");

    std::vector<uint8_t> data;
    SDLogger::Log("Vector data inicializado (size=%zu)", data.size());

    BinaryReader br;
    SDLogger::Log("Creado BinaryReader");
    SDLogger::Log("Intentando abrir archivo default_patched.xex...");

    if (!br.Open("sdmc:/RushXenonNX/game/default_patched.xex")) {
        SDLogger::Log("ERROR: No se pudo abrir default_patched.xex");
        return 0;
    }
    SDLogger::Log("Archivo default_patched.xex abierto correctamente");

    SDLogger::Log("Leyendo todos los datos del archivo...");
    if (!br.ReadAll(data)) {
        SDLogger::Log("ERROR leyendo el archivo completo");
        br.Close();
        return 0;
    }
    SDLogger::Log("Leído %zu bytes desde default_patched.xex", data.size());

    // Cerrar archivo ya que no se usará más
    br.Close();

    // Verificar que hay datos
    if (data.empty()) {
        SDLogger::Log("ERROR: data está vacío, no se puede cargar módulo");
        return 0;
    }

    SDLogger::Log("Parseando imagen...");
    const auto image = Image::ParseImage(data.data(), data.size());

    SDLogger::Log("Copiando imagen a memoria traducida...");
    memcpy(g_memory.Translate(image.base), image.data.get(), image.size);

    SDLogger::Log("Inicializando XDBFWrapper...");
    g_xdbfWrapper = XDBFWrapper(
        static_cast<uint8_t*>(g_memory.Translate(image.resource_offset)),
        image.resource_size
    );

    SDLogger::Log("=== LdrLoadModule END (entry=0x%08X) ===", image.entry_point);
    return image.entry_point;
}

int main()
{
    consoleInit(NULL);
    SDLogger::Init();
    SDLogger::Log("RushXenonNX started.");

    if (g_memory.base == nullptr)
    {
        SDLogger::Log("Memory not initialized.");
        consoleExit(NULL);
    }

    char buffer[128];
    sprintf(buffer, "Memory base: %p", g_memory.base);
    SDLogger::Log(buffer);

    g_userHeap.Init();

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    uint32_t entry = LdrLoadModule();

    GuestThread::Start({ entry, 0, 0 });
    SDLogger::Log("Started game!");


    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            break;

        consoleUpdate(NULL);
    }
    return 0;
}
