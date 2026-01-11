#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // Aqu� no hay conflicto porque NO incluimos Raylib

// Variables est�ticas ocultas (solo este archivo las ve)
static WNDPROC OriginalWndProc = NULL;
static bool* externalPauseFlag = nullptr;

// La funci�n del Hook (sin namespaces raros)
LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_ENTERSIZEMOVE:
        if (externalPauseFlag) *externalPauseFlag = true;
        break;
    case WM_EXITSIZEMOVE:
        if (externalPauseFlag) *externalPauseFlag = false;
        break;
    }
    // IMPORTANTE: CallWindowProc debe recibir el puntero original
    return CallWindowProc(OriginalWndProc, hwnd, uMsg, wParam, lParam);
}

// Funci�n que llamar�s desde tu motor
extern "C" void AttachHook(void* windowHandle, bool* pauseFlag) {
    if (windowHandle == nullptr) return;

    externalPauseFlag = pauseFlag;
    HWND hwnd = (HWND)windowHandle;

    // Cambiamos la funci�n de la ventana y guardamos la original
    OriginalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MyWindowProc);
}
#endif