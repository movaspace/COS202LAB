#include "AppointmentPanel.h"
#include "LayoutManager.h"
#include <sstream>

#define IDC_APPT_LIST       2001
#define IDC_APPT_PATID      2002
#define IDC_APPT_DATE       2003
#define IDC_APPT_TIME       2004
#define IDC_APPT_REASON     2005
#define IDC_APPT_BTN_SCHED  2006
#define IDC_APPT_BTN_CANCEL 2007
#define IDC_APPT_BTN_COMP   2008

static const int A_LBL_W  = 110;
static const int A_ROW_H  = 35;

namespace scapms {

AppointmentPanel::AppointmentPanel(HWND parent, ClinicManager& mgr)
    : parentHwnd(parent), containerHwnd(NULL), manager(mgr),
      listView(NULL),
      lblPatientId(NULL), lblDate(NULL), lblTime(NULL), lblReason(NULL),
      editPatientId(NULL), editDate(NULL), editTime(NULL), editReason(NULL),
      btnSchedule(NULL), btnCancel(NULL), btnComplete(NULL) {
    createControls();
}

HWND AppointmentPanel::getContainer() const { return containerHwnd; }

void AppointmentPanel::createControls() {
    HINSTANCE hi = GetModuleHandle(NULL);
    containerHwnd = CreateWindowEx(0, "SCAPMS_Panel", "", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 800, 560, parentHwnd, NULL, hi, NULL);

    listView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        10, 10, 540, 530, containerHwnd, (HMENU)IDC_APPT_LIST, hi, NULL);
    ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMN lvc; lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvc.iSubItem = 0; lvc.cx = 75;  lvc.pszText = (LPSTR)"Appt ID";    ListView_InsertColumn(listView, 0, &lvc);
    lvc.iSubItem = 1; lvc.cx = 75;  lvc.pszText = (LPSTR)"Patient ID"; ListView_InsertColumn(listView, 1, &lvc);
    lvc.iSubItem = 2; lvc.cx = 90;  lvc.pszText = (LPSTR)"Date";       ListView_InsertColumn(listView, 2, &lvc);
    lvc.iSubItem = 3; lvc.cx = 60;  lvc.pszText = (LPSTR)"Time";       ListView_InsertColumn(listView, 3, &lvc);
    lvc.iSubItem = 4; lvc.cx = 140; lvc.pszText = (LPSTR)"Reason";     ListView_InsertColumn(listView, 4, &lvc);
    lvc.iSubItem = 5; lvc.cx = 80;  lvc.pszText = (LPSTR)"Status";     ListView_InsertColumn(listView, 5, &lvc);

    int fx = 560, row = 0;
    int ew = 145;

    lblPatientId  = CreateWindowEx(0, "STATIC", "Patient ID:", WS_CHILD | WS_VISIBLE, fx, row*A_ROW_H+15, A_LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editPatientId = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                   fx+A_LBL_W+5, row*A_ROW_H+12, ew, 24, containerHwnd, (HMENU)IDC_APPT_PATID, hi, NULL); row++;

    lblDate  = CreateWindowEx(0, "STATIC", "Date:", WS_CHILD | WS_VISIBLE, fx, row*A_ROW_H+15, A_LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editDate = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "2026-11-13", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                              fx+A_LBL_W+5, row*A_ROW_H+12, ew, 24, containerHwnd, (HMENU)IDC_APPT_DATE, hi, NULL); row++;

    lblTime  = CreateWindowEx(0, "STATIC", "Time:", WS_CHILD | WS_VISIBLE, fx, row*A_ROW_H+15, A_LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editTime = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "10:00", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                              fx+A_LBL_W+5, row*A_ROW_H+12, ew, 24, containerHwnd, (HMENU)IDC_APPT_TIME, hi, NULL); row++;

    lblReason  = CreateWindowEx(0, "STATIC", "Reason:", WS_CHILD | WS_VISIBLE, fx, row*A_ROW_H+15, A_LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editReason = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "General Checkup", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                fx+A_LBL_W+5, row*A_ROW_H+12, ew, 24, containerHwnd, (HMENU)IDC_APPT_REASON, hi, NULL); row++;

    int btnY = row*A_ROW_H+15;
    btnSchedule = CreateWindowEx(0, "BUTTON", "Schedule Appointment", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, fx, btnY,     ew+A_LBL_W+5, 30, containerHwnd, (HMENU)IDC_APPT_BTN_SCHED,  hi, NULL);
    btnCancel   = CreateWindowEx(0, "BUTTON", "Cancel Appointment",   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, fx, btnY+38,  ew+A_LBL_W+5, 30, containerHwnd, (HMENU)IDC_APPT_BTN_CANCEL, hi, NULL);
    btnComplete = CreateWindowEx(0, "BUTTON", "Mark as Completed",    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, fx, btnY+76,  ew+A_LBL_W+5, 30, containerHwnd, (HMENU)IDC_APPT_BTN_COMP,   hi, NULL);

    LayoutManager::addTooltip(containerHwnd, btnSchedule, "Schedule appointment for the specified patient ID");
    LayoutManager::addTooltip(containerHwnd, btnCancel, "Cancel the selected appointment");
    LayoutManager::addTooltip(containerHwnd, btnComplete, "Mark the selected appointment as completed");

    refreshList();
}

void AppointmentPanel::refreshList() {
    ListView_DeleteAllItems(listView);
    const auto& appts = manager.getDatabase().getAllAppointments();
    int i = 0;
    for (const auto& a : appts) {
        LVITEM lvi; lvi.mask = LVIF_TEXT; lvi.iItem = i; lvi.iSubItem = 0;
        std::string id = a.getAppointmentId(); lvi.pszText = (LPSTR)id.c_str();
        ListView_InsertItem(listView, &lvi);
        std::string patId = a.getPatientId(); ListView_SetItemText(listView, i, 1, (LPSTR)patId.c_str());
        std::string d = a.getDate();           ListView_SetItemText(listView, i, 2, (LPSTR)d.c_str());
        std::string t = a.getTime();           ListView_SetItemText(listView, i, 3, (LPSTR)t.c_str());
        std::string r = a.getReason();         ListView_SetItemText(listView, i, 4, (LPSTR)r.c_str());
        std::string s = a.getStatus();         ListView_SetItemText(listView, i, 5, (LPSTR)s.c_str());
        i++;
    }
}

void AppointmentPanel::handleCommand(WPARAM wParam, LPARAM lParam) {
    WORD id = LOWORD(wParam);
    if      (id == IDC_APPT_BTN_SCHED)  onSchedule();
    else if (id == IDC_APPT_BTN_CANCEL) onCancel();
    else if (id == IDC_APPT_BTN_COMP)   onComplete();
}

void AppointmentPanel::handleNotify(WPARAM wParam, LPARAM lParam) {
    LPNMHDR lpnmhdr = (LPNMHDR)lParam;
    if (lpnmhdr->idFrom == IDC_APPT_LIST) {
        if (lpnmhdr->code == NM_CLICK || lpnmhdr->code == LVN_ITEMCHANGED) {
            populateFormFromSelection();
        }
    }
}

void AppointmentPanel::populateFormFromSelection() {
    int sel = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (sel < 0) return;

    char patBuf[64] = {0}, dateBuf[64] = {0}, timeBuf[64] = {0}, reasonBuf[256] = {0};
    ListView_GetItemText(listView, sel, 1, patBuf, sizeof(patBuf));
    ListView_GetItemText(listView, sel, 2, dateBuf, sizeof(dateBuf));
    ListView_GetItemText(listView, sel, 3, timeBuf, sizeof(timeBuf));
    ListView_GetItemText(listView, sel, 4, reasonBuf, sizeof(reasonBuf));

    SetWindowText(editPatientId, patBuf);
    SetWindowText(editDate, dateBuf);
    SetWindowText(editTime, timeBuf);
    SetWindowText(editReason, reasonBuf);
}

void AppointmentPanel::onSchedule() {
    char patBuf[64]={}, dateBuf[64]={}, timeBuf[64]={}, reasonBuf[256]={};
    GetWindowText(editPatientId, patBuf, sizeof(patBuf));
    GetWindowText(editDate, dateBuf, sizeof(dateBuf));
    GetWindowText(editTime, timeBuf, sizeof(timeBuf));
    GetWindowText(editReason, reasonBuf, sizeof(reasonBuf));
    if (strlen(patBuf) == 0) { MessageBox(parentHwnd, "Enter a valid Patient ID.", "Missing Input", MB_ICONWARNING); return; }
    std::string outId, err;
    if (manager.getAppointmentController().scheduleAppointment(patBuf, dateBuf, timeBuf, reasonBuf, outId, err)) {
        refreshList();
        MessageBox(parentHwnd, ("Appointment " + outId + " scheduled.").c_str(), "Scheduled", MB_OK | MB_ICONINFORMATION);
    } else { MessageBox(parentHwnd, err.c_str(), "Scheduling Error", MB_ICONERROR); }
}

void AppointmentPanel::onCancel() {
    int sel = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (sel < 0) { MessageBox(parentHwnd, "Select an appointment to cancel.", "Selection Required", MB_ICONWARNING); return; }
    char idBuf[64]={};
    ListView_GetItemText(listView, sel, 0, idBuf, sizeof(idBuf));
    std::string err;
    if (manager.getAppointmentController().cancelAppointment(idBuf, err)) { refreshList(); MessageBox(parentHwnd, "Appointment cancelled.", "Updated", MB_OK | MB_ICONINFORMATION); }
    else MessageBox(parentHwnd, err.c_str(), "Cancellation Failed", MB_ICONERROR);
}

void AppointmentPanel::onComplete() {
    int sel = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (sel < 0) { MessageBox(parentHwnd, "Select an appointment to complete.", "Selection Required", MB_ICONWARNING); return; }
    char idBuf[64]={};
    ListView_GetItemText(listView, sel, 0, idBuf, sizeof(idBuf));
    std::string err;
    if (manager.getAppointmentController().completeAppointment(idBuf, err)) { refreshList(); MessageBox(parentHwnd, "Marked as completed.", "Updated", MB_OK | MB_ICONINFORMATION); }
    else MessageBox(parentHwnd, err.c_str(), "Update Failed", MB_ICONERROR);
}

void AppointmentPanel::handleResize(int width, int height) {
    if (!containerHwnd) return;

    int formW = 265;
    int listW = width - formW - 25;
    if (listW < 280) listW = 280;
    SetWindowPos(listView, NULL, 10, 10, listW, height - 20, SWP_NOZORDER);

    int fx = listW + 20;
    int fw = width - fx - 10;
    int lw = A_LBL_W;
    int ex = fx + lw + 5;
    int ew = fw - lw - 5;
    if (ew < 80) ew = 80;

    int row = 0;
    SetWindowPos(lblPatientId,  NULL, fx, row*A_ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editPatientId, NULL, ex, row*A_ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblDate,       NULL, fx, row*A_ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editDate,      NULL, ex, row*A_ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblTime,       NULL, fx, row*A_ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editTime,      NULL, ex, row*A_ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblReason,     NULL, fx, row*A_ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editReason,    NULL, ex, row*A_ROW_H+12, ew, 24, SWP_NOZORDER); row++;

    int btnY = row*A_ROW_H+15;
    SetWindowPos(btnSchedule, NULL, fx, btnY,     fw, 30, SWP_NOZORDER);
    SetWindowPos(btnCancel,   NULL, fx, btnY+38,  fw, 30, SWP_NOZORDER);
    SetWindowPos(btnComplete, NULL, fx, btnY+76,  fw, 30, SWP_NOZORDER);
}

}
