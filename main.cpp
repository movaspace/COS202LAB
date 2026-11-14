#include <windows.h>
#include "gui/MainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    scapms::MainWindow app;

    if (!app.initialize(hInstance, nCmdShow)) {
        MessageBox(NULL, "Failed to initialize SCAPMS Application", "Error", MB_ICONERROR);
        return 1;
    }

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        // Intercept global keyboard shortcuts
        if (msg.message == WM_KEYDOWN) {
            if (msg.wParam == VK_F1) {
                app.onHelpAbout();
                continue;
            }
            bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            if (ctrlPressed) {
                if (msg.wParam == 'S' || msg.wParam == 's') {
                    app.onFileSave();
                    continue;
                }
                if (msg.wParam == 'L' || msg.wParam == 'l') {
                    app.onFileLoad();
                    continue;
                }
                if (msg.wParam == 'N' || msg.wParam == 'n') {
                    app.onFileNew();
                    continue;
                }
                if (msg.wParam == 'F' || msg.wParam == 'f') {
                    app.switchToTab(2);
                    continue;
                }
                if (msg.wParam == 'Q' || msg.wParam == 'q') {
                    app.onFileExit();
                    continue;
                }
                if (msg.wParam == 'Z' || msg.wParam == 'z') {
                    app.onEditUndo();
                    continue;
                }
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
