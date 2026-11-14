#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <commctrl.h>
#include "../controller/ClinicManager.h"
#include "PatientPanel.h"
#include "AppointmentPanel.h"
#include "SearchPanel.h"
#include "AdminPanel.h"
#include <memory>

namespace scapms {

class MainWindow {
private:
    HWND hwndMain;
    HWND tabControl;
    HWND statusBar;
    ClinicManager manager;

    std::unique_ptr<PatientPanel> patientPanel;
    std::unique_ptr<AppointmentPanel> appointmentPanel;
    std::unique_ptr<SearchPanel> searchPanel;
    std::unique_ptr<AdminPanel> adminPanel;

    int currentTab;

    void createMenu();
    void createTabs();
    void createStatusBar();
    void createShortcuts();
    void showCurrentTab();
    void updateStatusBar();

public:
    MainWindow();
    ~MainWindow() = default;

    bool initialize(HINSTANCE hInstance, int nCmdShow);
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND getHwnd() const { return hwndMain; }

    void onFileNew();
    void onFileSave();
    void onFileLoad();
    void onFileExit();
    void onEditUndo();
    void onHelpAbout();
    void onTimerTick();
    void switchToTab(int tabIndex);
};

}

#endif
