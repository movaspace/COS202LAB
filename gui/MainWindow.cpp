#include "MainWindow.h"
#include <commdlg.h>
#include <sstream>

#define IDM_FILE_NEW 5001
#define IDM_FILE_SAVE 5002
#define IDM_FILE_LOAD 5003
#define IDM_FILE_EXIT 5004
#define IDM_EDIT_UNDO 5005
#define IDM_HELP_ABOUT 5006
#define IDM_JUMP_SEARCH 5007
#define IDT_TIMER 1

namespace scapms {

static MainWindow* g_pMainWindow = nullptr;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_pMainWindow) {
        return g_pMainWindow->handleMessage(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_COMMAND || msg == WM_NOTIFY)
        SendMessage(GetParent(hwnd), msg, wp, lp);
    return DefWindowProc(hwnd, msg, wp, lp);
}

MainWindow::MainWindow() : hwndMain(NULL), tabControl(NULL), statusBar(NULL), currentTab(0) {
    g_pMainWindow = this;
}

bool MainWindow::initialize(HINSTANCE hInstance, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES | ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    // Panel class relays WM_COMMAND/WM_NOTIFY to main window
    WNDCLASS wcp = {};
    wcp.lpfnWndProc = PanelProc;
    wcp.hInstance = hInstance;
    wcp.lpszClassName = "SCAPMS_Panel";
    wcp.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClass(&wcp);

    const char CLASS_NAME[] = "SCAPMS_MainWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    RegisterClass(&wc);

    hwndMain = CreateWindowEx(
        0,
        CLASS_NAME,
        "Smart Clinic Appointment & Patient Management System (SCAPMS)",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 960, 700,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwndMain) {
        return false;
    }

    createMenu();
    createStatusBar();
    createTabs();

    patientPanel = std::make_unique<PatientPanel>(hwndMain, manager);
    appointmentPanel = std::make_unique<AppointmentPanel>(hwndMain, manager);
    searchPanel = std::make_unique<SearchPanel>(hwndMain, manager);
    adminPanel = std::make_unique<AdminPanel>(hwndMain, manager);

    // Trigger initial layout before first paint
    RECT rc; GetClientRect(hwndMain, &rc);
    SendMessage(hwndMain, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));

    showCurrentTab();
    updateStatusBar();

    SetTimer(hwndMain, IDT_TIMER, 5000, NULL);

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    return true;
}

void MainWindow::createMenu() {
    HMENU hMenuBar = CreateMenu();

    HMENU hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, "&New Database\tCtrl+N");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "&Save Database\tCtrl+S");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_LOAD, "&Load Database\tCtrl+L");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, "E&xit\tCtrl+Q");

    HMENU hEditMenu = CreateMenu();
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_UNDO, "&Undo Operation\tCtrl+Z");

    HMENU hHelpMenu = CreateMenu();
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, "&Help && Tooltips Guide\tF1");

    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "&File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, "&Edit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, "&Help");

    SetMenu(hwndMain, hMenuBar);
}

void MainWindow::createStatusBar() {
    statusBar = CreateWindowEx(
        0, STATUSCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        hwndMain, NULL, GetModuleHandle(NULL), NULL
    );

    int statwidths[] = {300, 500, 700, -1};
    SendMessage(statusBar, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM)statwidths);
}

void MainWindow::createTabs() {
    tabControl = CreateWindowEx(
        0, WC_TABCONTROL, "",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        10, 10, 920, 620,
        hwndMain, NULL, GetModuleHandle(NULL), NULL
    );

    TCITEM tie;
    tie.mask = TCIF_TEXT;

    tie.pszText = (LPSTR)"Patient Records";
    TabCtrl_InsertItem(tabControl, 0, &tie);

    tie.pszText = (LPSTR)"Appointments";
    TabCtrl_InsertItem(tabControl, 1, &tie);

    tie.pszText = (LPSTR)"Search&&Sort";
    TabCtrl_InsertItem(tabControl, 2, &tie);

    tie.pszText = (LPSTR)"Admin Panel";
    TabCtrl_InsertItem(tabControl, 3, &tie);

    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(tabControl, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void MainWindow::showCurrentTab() {
    if (!patientPanel || !appointmentPanel || !searchPanel || !adminPanel) return;

    HWND activeHwnd = NULL;
    if (currentTab == 0) activeHwnd = patientPanel->getContainer();
    else if (currentTab == 1) activeHwnd = appointmentPanel->getContainer();
    else if (currentTab == 2) activeHwnd = searchPanel->getContainer();
    else if (currentTab == 3) activeHwnd = adminPanel->getContainer();

    ShowWindow(patientPanel->getContainer(), currentTab == 0 ? SW_SHOW : SW_HIDE);
    ShowWindow(appointmentPanel->getContainer(), currentTab == 1 ? SW_SHOW : SW_HIDE);
    ShowWindow(searchPanel->getContainer(), currentTab == 2 ? SW_SHOW : SW_HIDE);
    ShowWindow(adminPanel->getContainer(), currentTab == 3 ? SW_SHOW : SW_HIDE);

    if (activeHwnd) {
        SetWindowPos(activeHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }

    if (currentTab == 0) patientPanel->refreshList();
    else if (currentTab == 1) appointmentPanel->refreshList();
    else if (currentTab == 2) searchPanel->refreshListWithRecords(manager.getDatabase().getAllPatients());
    else if (currentTab == 3) adminPanel->refreshAll();
}

void MainWindow::updateStatusBar() {
    if (!statusBar) return;
    int patCount = static_cast<int>(manager.getDatabase().getAllPatients().size());
    int qCount = manager.getDatabase().getQueueSize();
    auto summary = manager.getAppointmentController().getDailySummary();

    std::string s1 = "System: Active | Registered Patients: " + std::to_string(patCount);
    std::string s2 = "Waiting Queue: " + std::to_string(qCount) + " patients";
    std::string s3 = "Pending Appointments: " + std::to_string(summary.pending);
    std::string s4 = "Shortcuts: Ctrl+S (Save), Ctrl+L (Load), Ctrl+F (Search), Ctrl+Q (Exit), F1 (Help)";

    SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)s1.c_str());
    SendMessage(statusBar, SB_SETTEXT, 1, (LPARAM)s2.c_str());
    SendMessage(statusBar, SB_SETTEXT, 2, (LPARAM)s3.c_str());
    SendMessage(statusBar, SB_SETTEXT, 3, (LPARAM)s4.c_str());
}

void MainWindow::switchToTab(int tabIndex) {
    if (tabIndex >= 0 && tabIndex < 4) {
        currentTab = tabIndex;
        TabCtrl_SetCurSel(tabControl, tabIndex);
        showCurrentTab();
    }
}

void MainWindow::onFileNew() {
    if (MessageBox(hwndMain, "Reset database and clear all records?", "Confirm New Database", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        manager.reset();
        showCurrentTab();
        updateStatusBar();
        MessageBox(hwndMain, "Database reset complete.", "Information", MB_OK | MB_ICONINFORMATION);
    }
}

void MainWindow::onFileSave() {
    char szFile[MAX_PATH] = "patients.csv";
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwndMain;
    ofn.lpstrFilter = "CSV Files (*.csv)\0*.csv\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) {
        std::string patPath = szFile;
        std::string apptPath = patPath + ".appts.csv";
        std::string err;
        if (manager.saveToFiles(patPath, apptPath, err)) {
            MessageBox(hwndMain, "Clinic records saved successfully.", "Save Success", MB_OK | MB_ICONINFORMATION);
        } else {
            MessageBox(hwndMain, err.c_str(), "Save Error", MB_ICONERROR);
        }
    }
}

void MainWindow::onFileLoad() {
    char szFile[MAX_PATH] = {0};
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwndMain;
    ofn.lpstrFilter = "CSV Files (*.csv)\0*.csv\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        std::string patPath = szFile;
        std::string apptPath = patPath + ".appts.csv";
        std::string err;
        if (manager.loadFromFiles(patPath, apptPath, err)) {
            showCurrentTab();
            updateStatusBar();
            MessageBox(hwndMain, "Clinic records loaded successfully.", "Load Success", MB_OK | MB_ICONINFORMATION);
        } else {
            MessageBox(hwndMain, err.c_str(), "Load Error", MB_ICONERROR);
        }
    }
}

void MainWindow::onFileExit() {
    DestroyWindow(hwndMain);
}

void MainWindow::onEditUndo() {
    std::string msg;
    manager.undoLastAction(msg);
    showCurrentTab();
    updateStatusBar();
    MessageBox(hwndMain, msg.c_str(), "Undo Operation", MB_OK | MB_ICONINFORMATION);
}

void MainWindow::onHelpAbout() {
    static bool s_helpActive = false;
    if (s_helpActive) return;
    s_helpActive = true;

    std::string helpMsg =
        "Smart Clinic Appointment & Patient Management System (SCAPMS)\n"
        "User Guide & Tooltips\n\n"
        "Hover your cursor over buttons and controls to view built-in tooltips!\n\n"
        "Feature Guide:\n"
        "- Patient Records:\n"
        "  * Fill out the form on the right to register a new patient.\n"
        "  * Click any row in the list to load patient details into the form.\n"
        "  * Modify fields and click 'Update Selected' to save changes.\n"
        "  * Click 'Send to Waiting Queue' to queue a patient for consultation.\n\n"
        "- Appointments:\n"
        "  * Book appointments by entering Patient ID, Date, and Time.\n"
        "  * Select an appointment to cancel it or mark it completed.\n\n"
        "- Search & Sort:\n"
        "  * Search patients by ID or Name using the top search bar.\n"
        "  * Sort records by Patient Name, ID, or Age.\n\n"
        "- Admin Panel:\n"
        "  * Call next waiting patient from the consultation queue.\n"
        "  * Use 'Undo Last Action' to revert recent modifications.\n"
        "  * View live clinic attendance and category reports.\n\n"
        "Shortcuts: Ctrl+S (Save), Ctrl+L (Load), Ctrl+F (Search), Ctrl+Z (Undo), Ctrl+Q (Exit), F1 (Help)";

    MessageBox(hwndMain, helpMsg.c_str(), "SCAPMS - User Guide & Tooltips", MB_OK | MB_ICONINFORMATION);
    s_helpActive = false;
}

void MainWindow::onTimerTick() {
    updateStatusBar();
    if (currentTab == 3 && adminPanel) {
        adminPanel->refreshAll();
    }
}

LRESULT MainWindow::handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND: {
        WORD cmdId = LOWORD(wParam);
        switch (cmdId) {
        case IDM_FILE_NEW: onFileNew(); return 0;
        case IDM_FILE_SAVE: onFileSave(); return 0;
        case IDM_FILE_LOAD: onFileLoad(); return 0;
        case IDM_FILE_EXIT: onFileExit(); return 0;
        case IDM_EDIT_UNDO: onEditUndo(); return 0;
        case IDM_HELP_ABOUT: onHelpAbout(); return 0;
        case IDM_JUMP_SEARCH: switchToTab(2); return 0;
        }

        if (currentTab == 0 && patientPanel) patientPanel->handleCommand(wParam, lParam);
        else if (currentTab == 1 && appointmentPanel) appointmentPanel->handleCommand(wParam, lParam);
        else if (currentTab == 2 && searchPanel) searchPanel->handleCommand(wParam, lParam);
        else if (currentTab == 3 && adminPanel) adminPanel->handleCommand(wParam, lParam);

        updateStatusBar();
        return 0;
    }
    case WM_NOTIFY: {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        if (lpnmhdr->hwndFrom == tabControl && lpnmhdr->code == TCN_SELCHANGE) {
            currentTab = TabCtrl_GetCurSel(tabControl);
            showCurrentTab();
            updateStatusBar();
            return 0;
        }
        if (currentTab == 0 && patientPanel) patientPanel->handleNotify(wParam, lParam);
        else if (currentTab == 1 && appointmentPanel) appointmentPanel->handleNotify(wParam, lParam);
        break;
    }
    case WM_TIMER: {
        if (wParam == IDT_TIMER) {
            onTimerTick();
        }
        return 0;
    }
    case WM_HELP: {
        return TRUE;
    }
    case WM_KEYDOWN: {
        bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        if (ctrlPressed) {
            if (wParam == 'S' || wParam == 's') { onFileSave(); return 0; }
            if (wParam == 'L' || wParam == 'l') { onFileLoad(); return 0; }
            if (wParam == 'N' || wParam == 'n') { onFileNew(); return 0; }
            if (wParam == 'F' || wParam == 'f') { switchToTab(2); return 0; }
            if (wParam == 'Q' || wParam == 'q') { onFileExit(); return 0; }
            if (wParam == 'Z' || wParam == 'z') { onEditUndo(); return 0; }
        }
        break;
    }
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        SendMessage(statusBar, WM_SIZE, wParam, lParam);

        RECT rcStatus;
        GetWindowRect(statusBar, &rcStatus);
        int statusHeight = rcStatus.bottom - rcStatus.top;

        int tabWidth = width - 20;
        int tabHeight = height - statusHeight - 20;
        if (tabWidth > 0 && tabHeight > 0) {
            SetWindowPos(tabControl, HWND_BOTTOM, 10, 10, tabWidth, tabHeight, SWP_NOACTIVATE);

            RECT rcContent;
            SetRect(&rcContent, 10, 10, 10 + tabWidth, 10 + tabHeight);
            TabCtrl_AdjustRect(tabControl, FALSE, &rcContent);

            int contentX = rcContent.left + 2;
            int contentY = rcContent.top + 4;
            int contentW = (rcContent.right - rcContent.left) - 4;
            int contentH = (rcContent.bottom - rcContent.top) - 8;

            if (patientPanel) {
                SetWindowPos(patientPanel->getContainer(), HWND_TOP, contentX, contentY, contentW, contentH, SWP_NOACTIVATE);
                patientPanel->handleResize(contentW, contentH);
            }
            if (appointmentPanel) {
                SetWindowPos(appointmentPanel->getContainer(), HWND_TOP, contentX, contentY, contentW, contentH, SWP_NOACTIVATE);
                appointmentPanel->handleResize(contentW, contentH);
            }
            if (searchPanel) {
                SetWindowPos(searchPanel->getContainer(), HWND_TOP, contentX, contentY, contentW, contentH, SWP_NOACTIVATE);
                searchPanel->handleResize(contentW, contentH);
            }
            if (adminPanel) {
                SetWindowPos(adminPanel->getContainer(), HWND_TOP, contentX, contentY, contentW, contentH, SWP_NOACTIVATE);
                adminPanel->handleResize(contentW, contentH);
            }
        }
        return 0;
    }
    case WM_DESTROY: {
        KillTimer(hwnd, IDT_TIMER);
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

}
