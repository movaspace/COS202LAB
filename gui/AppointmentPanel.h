#ifndef APPOINTMENTPANEL_H
#define APPOINTMENTPANEL_H

#include <windows.h>
#include <commctrl.h>
#include "../controller/ClinicManager.h"

namespace scapms {

class AppointmentPanel {
private:
    HWND parentHwnd;
    HWND containerHwnd;
    ClinicManager& manager;

    HWND listView;
    HWND lblPatientId, lblDate, lblTime, lblReason;
    HWND editPatientId, editDate, editTime, editReason;
    HWND btnSchedule, btnCancel, btnComplete;

    void createControls();

public:
    AppointmentPanel(HWND parent, ClinicManager& mgr);
    ~AppointmentPanel() = default;

    HWND getContainer() const;
    void refreshList();
    void handleCommand(WPARAM wParam, LPARAM lParam);
    void handleNotify(WPARAM wParam, LPARAM lParam);
    void populateFormFromSelection();
    void handleResize(int width, int height);
    void onSchedule();
    void onCancel();
    void onComplete();
};

}

#endif
