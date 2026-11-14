#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <windows.h>
#include <commctrl.h>
#include "../controller/ClinicManager.h"

namespace scapms {

class AdminPanel {
private:
    HWND parentHwnd;
    HWND containerHwnd;
    ClinicManager& manager;

    HWND grpQueue, listQueue, btnCallNext, lblCurrentCalled;
    HWND grpUndo, btnUndo, lblUndoStatus;
    HWND grpCache, listRecentCache;
    HWND grpReports, btnRefreshReports, editReportOutput;

    void createControls();

public:
    AdminPanel(HWND parent, ClinicManager& mgr);
    ~AdminPanel() = default;

    HWND getContainer() const;
    void refreshAll();
    void handleCommand(WPARAM wParam, LPARAM lParam);
    void handleResize(int width, int height);
    void onCallNextPatient();
    void onUndo();
    void onRefreshReports();
};

}

#endif
