#include "AdminPanel.h"
#include "LayoutManager.h"
#include <sstream>

#define IDC_ADMIN_CALL_NEXT  4001
#define IDC_ADMIN_UNDO       4002
#define IDC_ADMIN_REFRESH    4003
#define IDC_ADMIN_QUEUE_LIST 4004
#define IDC_ADMIN_CACHE_LIST 4005
#define IDC_ADMIN_REPORT_EDIT 4006

namespace scapms {

AdminPanel::AdminPanel(HWND parent, ClinicManager& mgr)
    : parentHwnd(parent), containerHwnd(NULL), manager(mgr),
      grpQueue(NULL), listQueue(NULL), btnCallNext(NULL), lblCurrentCalled(NULL),
      grpUndo(NULL), btnUndo(NULL), lblUndoStatus(NULL),
      grpCache(NULL), listRecentCache(NULL),
      grpReports(NULL), btnRefreshReports(NULL), editReportOutput(NULL) {
    createControls();
}

HWND AdminPanel::getContainer() const { return containerHwnd; }

void AdminPanel::createControls() {
    HINSTANCE hi = GetModuleHandle(NULL);
    containerHwnd = CreateWindowEx(0, "SCAPMS_Panel", "", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 800, 560, parentHwnd, NULL, hi, NULL);

    // Left column: Queue group
    grpQueue = CreateWindowEx(0, "BUTTON", "Consultation Waiting Queue",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 5, 370, 220, containerHwnd, NULL, hi, NULL);
    listQueue = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL,
        20, 25, 350, 130, containerHwnd, (HMENU)IDC_ADMIN_QUEUE_LIST, hi, NULL);
    btnCallNext = CreateWindowEx(0, "BUTTON", "Call Next Patient",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 165, 160, 28, containerHwnd, (HMENU)IDC_ADMIN_CALL_NEXT, hi, NULL);
    lblCurrentCalled = CreateWindowEx(0, "STATIC", "Current: None",
        WS_CHILD | WS_VISIBLE, 190, 169, 180, 20, containerHwnd, NULL, hi, NULL);

    // Left column: Undo group
    grpUndo = CreateWindowEx(0, "BUTTON", "Administrative Actions",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 232, 370, 85, containerHwnd, NULL, hi, NULL);
    btnUndo = CreateWindowEx(0, "BUTTON", "Undo Last Action",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 254, 170, 28, containerHwnd, (HMENU)IDC_ADMIN_UNDO, hi, NULL);
    lblUndoStatus = CreateWindowEx(0, "STATIC", "Ready",
        WS_CHILD | WS_VISIBLE, 20, 288, 350, 20, containerHwnd, NULL, hi, NULL);

    // Left column: Cache group
    grpCache = CreateWindowEx(0, "BUTTON", "Recently Accessed Patients",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 324, 370, 220, containerHwnd, NULL, hi, NULL);
    listRecentCache = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL,
        20, 345, 350, 190, containerHwnd, (HMENU)IDC_ADMIN_CACHE_LIST, hi, NULL);

    // Right column: Reports group
    grpReports = CreateWindowEx(0, "BUTTON", "Clinic Analytics & Reports",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 390, 5, 390, 540, containerHwnd, NULL, hi, NULL);
    btnRefreshReports = CreateWindowEx(0, "BUTTON", "Refresh Reports Dashboard",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 25, 200, 28, containerHwnd, (HMENU)IDC_ADMIN_REFRESH, hi, NULL);
    editReportOutput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL,
        400, 60, 370, 475, containerHwnd, (HMENU)IDC_ADMIN_REPORT_EDIT, hi, NULL);

    LayoutManager::addTooltip(containerHwnd, btnCallNext, "Call and serve the next waiting patient in line");
    LayoutManager::addTooltip(containerHwnd, btnUndo, "Revert the last database or patient operation");
    LayoutManager::addTooltip(containerHwnd, btnRefreshReports, "Refresh live clinic report analytics");

    refreshAll();
}

void AdminPanel::refreshAll() {
    SendMessage(listQueue, LB_RESETCONTENT, 0, 0);
    auto qList = manager.getQueueList();
    if (qList.empty()) {
        SendMessage(listQueue, LB_ADDSTRING, 0, (LPARAM)"Queue is currently empty");
    } else {
        int pos = 1;
        for (const auto& patId : qList) {
            auto pat = manager.getDatabase().findPatientById(patId);
            std::string item = std::to_string(pos++) + ". " + patId;
            if (pat) item += " [" + pat->getName() + " - " + pat->getPatientType() + "]";
            SendMessage(listQueue, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }

    SendMessage(listRecentCache, LB_RESETCONTENT, 0, 0);
    auto cache = manager.getRecentlyAccessedPatients();
    if (cache.empty()) {
        SendMessage(listRecentCache, LB_ADDSTRING, 0, (LPARAM)"No recently accessed records");
    } else {
        int idx = 1;
        for (auto* p : cache) {
            if (p) {
                std::string e = "[" + std::to_string(idx++) + "] " + p->getId() + ": " + p->getName() + " (" + p->getPatientType() + ")";
                SendMessage(listRecentCache, LB_ADDSTRING, 0, (LPARAM)e.c_str());
            }
        }
    }

    onRefreshReports();
}

void AdminPanel::onCallNextPatient() {
    std::string nextId = manager.callNextPatient();
    if (nextId.empty()) {
        SetWindowText(lblCurrentCalled, "Current: None (empty)");
        MessageBox(parentHwnd, "No patients waiting in queue.", "Queue Status", MB_ICONINFORMATION);
    } else {
        auto pat = manager.getDatabase().findPatientById(nextId);
        std::string name = pat ? pat->getName() : "Unknown";
        SetWindowText(lblCurrentCalled, ("Current: " + nextId + " (" + name + ")").c_str());
        MessageBox(parentHwnd, ("Now Serving: " + nextId + " - " + name).c_str(), "Patient Called", MB_OK | MB_ICONINFORMATION);
    }
    refreshAll();
}

void AdminPanel::onUndo() {
    std::string msg;
    if (manager.undoLastAction(msg)) {
        SetWindowText(lblUndoStatus, msg.c_str());
        MessageBox(parentHwnd, msg.c_str(), "Undo Successful", MB_OK | MB_ICONINFORMATION);
    } else {
        SetWindowText(lblUndoStatus, msg.c_str());
        MessageBox(parentHwnd, msg.c_str(), "Undo Info", MB_ICONINFORMATION);
    }
    refreshAll();
}

void AdminPanel::onRefreshReports() {
    std::ostringstream oss;
    oss << "==========================================\r\n";
    oss << "      SCAPMS CLINIC REPORT DASHBOARD      \r\n";
    oss << "==========================================\r\n\r\n";

    oss << "1. MOST FREQUENTLY ATTENDED PATIENTS\r\n";
    auto frequent = manager.getAppointmentController().getMostFrequentlyAttendedPatients();
    if (frequent.empty()) { oss << "No completed appointments yet.\r\n"; }
    else {
        for (const auto& pair : frequent) {
            auto pat = manager.getDatabase().findPatientById(pair.first);
            oss << "  " << pair.first << " (" << (pat ? pat->getName() : "?") << ") | Visits: " << pair.second << "\r\n";
        }
    }
    oss << "\r\n2. PATIENTS WITH PENDING APPOINTMENTS\r\n";
    auto pending = manager.getAppointmentController().getPendingPatientsReport();
    if (pending.empty()) { oss << "No pending appointments.\r\n"; }
    else {
        for (const auto& id : pending) {
            auto pat = manager.getDatabase().findPatientById(id);
            oss << "  Patient: " << id << " (" << (pat ? pat->getName() : "?") << ")\r\n";
        }
    }
    oss << "\r\n3. PATIENT CATEGORY DISTRIBUTION\r\n";
    auto catDist = manager.getCategoryDistribution();
    oss << "  General:   " << catDist.generalCount   << "\r\n";
    oss << "  Emergency: " << catDist.emergencyCount  << "\r\n";
    oss << "  Follow-Up: " << catDist.followUpCount   << "\r\n";
    oss << "  Total:     " << (catDist.generalCount + catDist.emergencyCount + catDist.followUpCount) << "\r\n";

    oss << "\r\n4. DAILY APPOINTMENT SUMMARY\r\n";
    auto summary = manager.getAppointmentController().getDailySummary();
    oss << "  Total:     " << summary.total     << "\r\n";
    oss << "  Completed: " << summary.completed << "\r\n";
    oss << "  Pending:   " << summary.pending   << "\r\n";
    oss << "  Cancelled: " << summary.cancelled << "\r\n";
    oss << "\r\n==========================================\r\n";

    SetWindowText(editReportOutput, oss.str().c_str());
}

void AdminPanel::handleCommand(WPARAM wParam, LPARAM lParam) {
    WORD id = LOWORD(wParam);
    if      (id == IDC_ADMIN_CALL_NEXT) onCallNextPatient();
    else if (id == IDC_ADMIN_UNDO)      onUndo();
    else if (id == IDC_ADMIN_REFRESH)   refreshAll();
}

void AdminPanel::handleResize(int width, int height) {
    if (!containerHwnd) return;

    int leftW  = width / 2 - 15;
    int rightX = width / 2 + 5;
    int rightW = width - rightX - 10;

    // Queue group: top ~40% of left column
    int qH = (int)(height * 0.40);
    SetWindowPos(grpQueue,        NULL, 10, 5,     leftW, qH,    SWP_NOZORDER);
    SetWindowPos(listQueue,       NULL, 20, 25,    leftW-20, qH-70, SWP_NOZORDER);
    SetWindowPos(btnCallNext,     NULL, 20, qH-37, 160, 28,     SWP_NOZORDER);
    SetWindowPos(lblCurrentCalled,NULL, 190, qH-33, leftW-200, 20, SWP_NOZORDER);

    // Undo group: fixed 85px height
    int undoY = qH + 10;
    SetWindowPos(grpUndo,     NULL, 10, undoY,    leftW, 85,  SWP_NOZORDER);
    SetWindowPos(btnUndo,     NULL, 20, undoY+22, 170, 28,    SWP_NOZORDER);
    SetWindowPos(lblUndoStatus,NULL, 20, undoY+56, leftW-20, 20, SWP_NOZORDER);

    // Cache group: fills remaining height
    int cacheY = undoY + 95;
    int cacheH = height - cacheY - 10;
    SetWindowPos(grpCache,       NULL, 10, cacheY, leftW, cacheH, SWP_NOZORDER);
    SetWindowPos(listRecentCache,NULL, 20, cacheY+22, leftW-20, cacheH-30, SWP_NOZORDER);

    // Reports group: full right column
    SetWindowPos(grpReports,       NULL, rightX, 5,  rightW, height-10, SWP_NOZORDER);
    SetWindowPos(btnRefreshReports,NULL, rightX+10, 24, 200, 28, SWP_NOZORDER);
    SetWindowPos(editReportOutput, NULL, rightX+10, 58, rightW-20, height-70, SWP_NOZORDER);
}

}
