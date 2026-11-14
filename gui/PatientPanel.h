#ifndef PATIENTPANEL_H
#define PATIENTPANEL_H

#include <windows.h>
#include <commctrl.h>
#include "../controller/ClinicManager.h"

namespace scapms {

class PatientPanel {
private:
    HWND parentHwnd;
    HWND containerHwnd;
    ClinicManager& manager;

    HWND listView;
    HWND lblType, lblName, lblAge, lblContact, lblExtra1, lblExtra2;
    HWND cbType;
    HWND editName, editAge, editContact, editExtra1, editExtra2;
    HWND btnAdd, btnEdit, btnDelete, btnQueue;

    void createControls();
    void updateDynamicFields();

public:
    PatientPanel(HWND parent, ClinicManager& mgr);
    ~PatientPanel() = default;

    HWND getContainer() const;
    void refreshList();
    void handleCommand(WPARAM wParam, LPARAM lParam);
    void handleNotify(WPARAM wParam, LPARAM lParam);
    void populateFormFromSelection();
    void handleResize(int width, int height);
    void onAddPatient();
    void onEditPatient();
    void onDeletePatient();
    void onAddToQueue();
};

}

#endif
