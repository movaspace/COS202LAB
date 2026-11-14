#include "PatientPanel.h"
#include "LayoutManager.h"
#include "../model/GeneralPatient.h"
#include "../model/EmergencyPatient.h"
#include "../model/FollowUpPatient.h"
#include <sstream>

#define IDC_PAT_LIST    1001
#define IDC_PAT_TYPE    1002
#define IDC_PAT_NAME    1003
#define IDC_PAT_AGE     1004
#define IDC_PAT_CONTACT 1005
#define IDC_PAT_EXTRA1  1006
#define IDC_PAT_EXTRA2  1007
#define IDC_PAT_BTN_ADD   1008
#define IDC_PAT_BTN_EDIT  1009
#define IDC_PAT_BTN_DEL   1010
#define IDC_PAT_BTN_QUEUE 1011

// Form geometry constants (relative to form column start)
static const int LBL_W  = 100;
static const int FLD_OFF = 105;
static const int ROW_H   = 35;

namespace scapms {

PatientPanel::PatientPanel(HWND parent, ClinicManager& mgr)
    : parentHwnd(parent), containerHwnd(NULL), manager(mgr),
      listView(NULL),
      lblType(NULL), lblName(NULL), lblAge(NULL), lblContact(NULL),
      lblExtra1(NULL), lblExtra2(NULL),
      cbType(NULL), editName(NULL), editAge(NULL), editContact(NULL),
      editExtra1(NULL), editExtra2(NULL),
      btnAdd(NULL), btnEdit(NULL), btnDelete(NULL), btnQueue(NULL) {
    createControls();
}

HWND PatientPanel::getContainer() const { return containerHwnd; }

void PatientPanel::createControls() {
    HINSTANCE hi = GetModuleHandle(NULL);
    containerHwnd = CreateWindowEx(0, "SCAPMS_Panel", "", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 800, 560, parentHwnd, NULL, hi, NULL);

    listView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        10, 10, 540, 530, containerHwnd, (HMENU)IDC_PAT_LIST, hi, NULL);
    ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMN lvc; lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvc.iSubItem = 0; lvc.cx = 65;  lvc.pszText = (LPSTR)"ID";           ListView_InsertColumn(listView, 0, &lvc);
    lvc.iSubItem = 1; lvc.cx = 100; lvc.pszText = (LPSTR)"Type";         ListView_InsertColumn(listView, 1, &lvc);
    lvc.iSubItem = 2; lvc.cx = 120; lvc.pszText = (LPSTR)"Name";         ListView_InsertColumn(listView, 2, &lvc);
    lvc.iSubItem = 3; lvc.cx = 40;  lvc.pszText = (LPSTR)"Age";          ListView_InsertColumn(listView, 3, &lvc);
    lvc.iSubItem = 4; lvc.cx = 90;  lvc.pszText = (LPSTR)"Contact";      ListView_InsertColumn(listView, 4, &lvc);
    lvc.iSubItem = 5; lvc.cx = 150; lvc.pszText = (LPSTR)"Details";      ListView_InsertColumn(listView, 5, &lvc);

    // Form column starts at x=560 (default window size)
    int fx = 560;
    int row = 0;

    lblType  = CreateWindowEx(0, "STATIC", "Patient Type:", WS_CHILD | WS_VISIBLE, fx, row*ROW_H+15, LBL_W, 20, containerHwnd, NULL, hi, NULL);
    cbType   = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                              fx+FLD_OFF, row*ROW_H+12, 145, 120, containerHwnd, (HMENU)IDC_PAT_TYPE, hi, NULL);
    SendMessage(cbType, CB_ADDSTRING, 0, (LPARAM)"General");
    SendMessage(cbType, CB_ADDSTRING, 0, (LPARAM)"Emergency");
    SendMessage(cbType, CB_ADDSTRING, 0, (LPARAM)"FollowUp");
    SendMessage(cbType, CB_SETCURSEL, 0, 0);
    row++;

    lblName  = CreateWindowEx(0, "STATIC", "Full Name:", WS_CHILD | WS_VISIBLE, fx, row*ROW_H+15, LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editName = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                              fx+FLD_OFF, row*ROW_H+12, 145, 24, containerHwnd, (HMENU)IDC_PAT_NAME, hi, NULL);
    row++;

    lblAge   = CreateWindowEx(0, "STATIC", "Age (0-120):", WS_CHILD | WS_VISIBLE, fx, row*ROW_H+15, LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editAge  = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER,
                              fx+FLD_OFF, row*ROW_H+12, 145, 24, containerHwnd, (HMENU)IDC_PAT_AGE, hi, NULL);
    row++;

    lblContact  = CreateWindowEx(0, "STATIC", "Contact:", WS_CHILD | WS_VISIBLE, fx, row*ROW_H+15, LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editContact = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                 fx+FLD_OFF, row*ROW_H+12, 145, 24, containerHwnd, (HMENU)IDC_PAT_CONTACT, hi, NULL);
    row++;

    lblExtra1  = CreateWindowEx(0, "STATIC", "Department:", WS_CHILD | WS_VISIBLE, fx, row*ROW_H+15, LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editExtra1 = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                fx+FLD_OFF, row*ROW_H+12, 145, 24, containerHwnd, (HMENU)IDC_PAT_EXTRA1, hi, NULL);
    row++;

    lblExtra2  = CreateWindowEx(0, "STATIC", "Extra:", WS_CHILD, fx, row*ROW_H+15, LBL_W, 20, containerHwnd, NULL, hi, NULL);
    editExtra2 = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | ES_AUTOHSCROLL,
                                fx+FLD_OFF, row*ROW_H+12, 145, 24, containerHwnd, (HMENU)IDC_PAT_EXTRA2, hi, NULL);
    row++;

    int btnY = row*ROW_H+15;
    btnAdd    = CreateWindowEx(0, "BUTTON", "Register Patient",    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, fx, btnY,     250, 30, containerHwnd, (HMENU)IDC_PAT_BTN_ADD,   hi, NULL);
    btnEdit   = CreateWindowEx(0, "BUTTON", "Update Selected",     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, fx, btnY+38, 250, 30, containerHwnd, (HMENU)IDC_PAT_BTN_EDIT,  hi, NULL);
    btnDelete = CreateWindowEx(0, "BUTTON", "Delete Selected",     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, fx, btnY+76, 250, 30, containerHwnd, (HMENU)IDC_PAT_BTN_DEL,   hi, NULL);
    btnQueue  = CreateWindowEx(0, "BUTTON", "Send to Waiting Queue", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, fx, btnY+114, 250, 30, containerHwnd, (HMENU)IDC_PAT_BTN_QUEUE, hi, NULL);

    LayoutManager::addTooltip(containerHwnd, btnAdd, "Register a new patient with the entered details");
    LayoutManager::addTooltip(containerHwnd, btnEdit, "Save changes to the selected patient's record");
    LayoutManager::addTooltip(containerHwnd, btnDelete, "Delete selected patient record (Undoable in Admin Panel)");
    LayoutManager::addTooltip(containerHwnd, btnQueue, "Place selected patient into the consultation waiting queue");

    updateDynamicFields();
    refreshList();
}

void PatientPanel::updateDynamicFields() {
    int sel = (int)SendMessage(cbType, CB_GETCURSEL, 0, 0);
    if (sel == 0) {
        SetWindowText(lblExtra1, "Department:");
        ShowWindow(lblExtra2, SW_HIDE);
        ShowWindow(editExtra2, SW_HIDE);
    } else if (sel == 1) {
        SetWindowText(lblExtra1, "Severity (1-5):");
        SetWindowText(lblExtra2, "Triage Notes:");
        ShowWindow(lblExtra2, SW_SHOW);
        ShowWindow(editExtra2, SW_SHOW);
    } else {
        SetWindowText(lblExtra1, "Prev Doctor:");
        SetWindowText(lblExtra2, "Diagnosis:");
        ShowWindow(lblExtra2, SW_SHOW);
        ShowWindow(editExtra2, SW_SHOW);
    }
}

void PatientPanel::refreshList() {
    ListView_DeleteAllItems(listView);
    const auto& patients = manager.getDatabase().getAllPatients();
    int i = 0;
    for (const auto& p : patients) {
        LVITEM lvi; lvi.mask = LVIF_TEXT; lvi.iItem = i; lvi.iSubItem = 0;
        std::string id = p->getId(); lvi.pszText = (LPSTR)id.c_str();
        ListView_InsertItem(listView, &lvi);

        std::string typeDesc = p->getPatientType();
        if (typeDesc == "Emergency") typeDesc += " [HIGH]";
        else if (typeDesc == "General") typeDesc += " [NORMAL]";
        else if (typeDesc == "FollowUp") typeDesc += " [SCHED]";

        ListView_SetItemText(listView, i, 1, (LPSTR)typeDesc.c_str());
        std::string name = p->getName();         ListView_SetItemText(listView, i, 2, (LPSTR)name.c_str());
        std::string age  = std::to_string(p->getAge()); ListView_SetItemText(listView, i, 3, (LPSTR)age.c_str());
        std::string cont = p->getContactInfo();  ListView_SetItemText(listView, i, 4, (LPSTR)cont.c_str());
        std::string det  = p->getDetails();      ListView_SetItemText(listView, i, 5, (LPSTR)det.c_str());
        i++;
    }
}

void PatientPanel::handleCommand(WPARAM wParam, LPARAM lParam) {
    WORD id = LOWORD(wParam), code = HIWORD(wParam);
    if (id == IDC_PAT_TYPE && code == CBN_SELCHANGE) updateDynamicFields();
    else if (id == IDC_PAT_BTN_ADD)   onAddPatient();
    else if (id == IDC_PAT_BTN_EDIT)  onEditPatient();
    else if (id == IDC_PAT_BTN_DEL)   onDeletePatient();
    else if (id == IDC_PAT_BTN_QUEUE) onAddToQueue();
}

void PatientPanel::handleNotify(WPARAM wParam, LPARAM lParam) {
    LPNMHDR lpnmhdr = (LPNMHDR)lParam;
    if (lpnmhdr->idFrom == IDC_PAT_LIST) {
        if (lpnmhdr->code == NM_CLICK || lpnmhdr->code == LVN_ITEMCHANGED) {
            populateFormFromSelection();
        }
    }
}

void PatientPanel::populateFormFromSelection() {
    int sel = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (sel < 0) return;

    char idBuf[64] = {0};
    ListView_GetItemText(listView, sel, 0, idBuf, sizeof(idBuf));
    auto pat = manager.getDatabase().findPatientById(idBuf);
    if (!pat) return;

    std::string type = pat->getPatientType();
    if (type == "General") {
        SendMessage(cbType, CB_SETCURSEL, 0, 0);
        updateDynamicFields();
        auto gen = std::dynamic_pointer_cast<GeneralPatient>(pat);
        SetWindowText(editExtra1, gen ? gen->getDepartment().c_str() : "");
    } else if (type == "Emergency") {
        SendMessage(cbType, CB_SETCURSEL, 1, 0);
        updateDynamicFields();
        auto emg = std::dynamic_pointer_cast<EmergencyPatient>(pat);
        if (emg) {
            SetWindowText(editExtra1, std::to_string(emg->getSeverityLevel()).c_str());
            SetWindowText(editExtra2, emg->getTriageNotes().c_str());
        }
    } else if (type == "FollowUp") {
        SendMessage(cbType, CB_SETCURSEL, 2, 0);
        updateDynamicFields();
        auto fol = std::dynamic_pointer_cast<FollowUpPatient>(pat);
        if (fol) {
            SetWindowText(editExtra1, fol->getPreviousDoctor().c_str());
            SetWindowText(editExtra2, fol->getOriginalDiagnosis().c_str());
        }
    }

    SetWindowText(editName, pat->getName().c_str());
    SetWindowText(editAge, std::to_string(pat->getAge()).c_str());
    SetWindowText(editContact, pat->getContactInfo().c_str());
}

void PatientPanel::onAddPatient() {
    char nameBuf[128]={}, ageBuf[16]={}, contactBuf[128]={}, ex1Buf[128]={}, ex2Buf[128]={};
    GetWindowText(editName, nameBuf, sizeof(nameBuf));
    GetWindowText(editAge, ageBuf, sizeof(ageBuf));
    GetWindowText(editContact, contactBuf, sizeof(contactBuf));
    GetWindowText(editExtra1, ex1Buf, sizeof(ex1Buf));
    GetWindowText(editExtra2, ex2Buf, sizeof(ex2Buf));

    int age = 0;
    try { age = std::stoi(ageBuf); } catch (...) {
        MessageBox(parentHwnd, "Age must be a number between 0 and 120.", "Validation Error", MB_ICONWARNING); return;
    }
    if (age < 0 || age > 120) {
        MessageBox(parentHwnd, "Age must be between 0 and 120.", "Validation Error", MB_ICONWARNING); return;
    }

    int sel = (int)SendMessage(cbType, CB_GETCURSEL, 0, 0);
    std::string newId, err; bool ok = false;
    if (sel == 0)      ok = manager.registerGeneralPatient(nameBuf, age, contactBuf, ex1Buf, newId, err);
    else if (sel == 1) { int sev=1; try { sev=std::stoi(ex1Buf); } catch(...){} ok = manager.registerEmergencyPatient(nameBuf, age, contactBuf, sev, ex2Buf, newId, err); }
    else               ok = manager.registerFollowUpPatient(nameBuf, age, contactBuf, ex1Buf, ex2Buf, newId, err);

    if (ok) {
        SetWindowText(editName, ""); SetWindowText(editAge, "");
        SetWindowText(editContact, ""); SetWindowText(editExtra1, ""); SetWindowText(editExtra2, "");
        refreshList();
        std::string msg = "Patient registered: " + newId;
        MessageBox(parentHwnd, msg.c_str(), "Success", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(parentHwnd, err.c_str(), "Error", MB_ICONERROR);
    }
}

void PatientPanel::onEditPatient() {
    int sel = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (sel < 0) { MessageBox(parentHwnd, "Select a patient from the list first to update.", "Selection Required", MB_ICONWARNING); return; }
    char idBuf[64]={}, nameBuf[128]={}, ageBuf[16]={}, contactBuf[128]={}, ex1Buf[128]={}, ex2Buf[128]={};
    ListView_GetItemText(listView, sel, 0, idBuf, sizeof(idBuf));
    GetWindowText(editName, nameBuf, sizeof(nameBuf));
    GetWindowText(editAge, ageBuf, sizeof(ageBuf));
    GetWindowText(editContact, contactBuf, sizeof(contactBuf));
    GetWindowText(editExtra1, ex1Buf, sizeof(ex1Buf));
    GetWindowText(editExtra2, ex2Buf, sizeof(ex2Buf));

    if (strlen(nameBuf) == 0 && strlen(ageBuf) == 0) {
        populateFormFromSelection();
        MessageBox(parentHwnd, "Patient details loaded into form. Edit the desired fields and click Update Selected to save changes.", "Patient Loaded", MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (strlen(nameBuf) == 0) {
        MessageBox(parentHwnd, "Patient name cannot be empty.", "Validation Error", MB_ICONWARNING);
        return;
    }

    int age = 0;
    try { age = std::stoi(ageBuf); } catch (...) { MessageBox(parentHwnd, "Age must be a valid number between 0 and 120.", "Validation Error", MB_ICONWARNING); return; }
    if (age < 0 || age > 120) {
        MessageBox(parentHwnd, "Age must be between 0 and 120.", "Validation Error", MB_ICONWARNING);
        return;
    }

    std::string err;
    if (manager.updatePatient(idBuf, nameBuf, age, contactBuf, ex1Buf, ex2Buf, err)) {
        refreshList();
        std::string msg = "Patient " + std::string(idBuf) + " updated successfully.";
        MessageBox(parentHwnd, msg.c_str(), "Update Success", MB_OK | MB_ICONINFORMATION);
    } else { MessageBox(parentHwnd, err.c_str(), "Update Failed", MB_ICONERROR); }
}

void PatientPanel::onDeletePatient() {
    int sel = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (sel < 0) { MessageBox(parentHwnd, "Select a patient from the list first.", "Selection Required", MB_ICONWARNING); return; }
    char idBuf[64]={};
    ListView_GetItemText(listView, sel, 0, idBuf, sizeof(idBuf));
    if (MessageBox(parentHwnd, "Delete this patient record?", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        std::string err;
        if (manager.deletePatient(idBuf, err)) { refreshList(); MessageBox(parentHwnd, "Deleted. Use Undo in Admin to restore.", "Deleted", MB_OK | MB_ICONINFORMATION); }
        else MessageBox(parentHwnd, err.c_str(), "Delete Failed", MB_ICONERROR);
    }
}

void PatientPanel::onAddToQueue() {
    int sel = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (sel < 0) { MessageBox(parentHwnd, "Select a patient to add to queue.", "Selection Required", MB_ICONWARNING); return; }
    char idBuf[64]={};
    ListView_GetItemText(listView, sel, 0, idBuf, sizeof(idBuf));
    std::string err;
    if (manager.enqueuePatient(idBuf, err)) { MessageBox(parentHwnd, (std::string(idBuf) + " added to queue.").c_str(), "Queue Updated", MB_OK | MB_ICONINFORMATION); }
    else MessageBox(parentHwnd, err.c_str(), "Queue Error", MB_ICONERROR);
}

void PatientPanel::handleResize(int width, int height) {
    if (!containerHwnd) return;

    int formW = 255;
    int listW = width - formW - 25;
    if (listW < 280) listW = 280;
    SetWindowPos(listView, NULL, 10, 10, listW, height - 20, SWP_NOZORDER);

    int fx = listW + 20;
    int fw = width - fx - 10;
    int lw = 100;
    int ex = fx + lw + 5;
    int ew = fw - lw - 5;
    if (ew < 80) ew = 80;

    int row = 0;
    SetWindowPos(lblType,    NULL, fx, row*ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(cbType,     NULL, ex, row*ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblName,    NULL, fx, row*ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editName,   NULL, ex, row*ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblAge,     NULL, fx, row*ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editAge,    NULL, ex, row*ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblContact, NULL, fx, row*ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editContact,NULL, ex, row*ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblExtra1,  NULL, fx, row*ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editExtra1, NULL, ex, row*ROW_H+12, ew, 24, SWP_NOZORDER); row++;
    SetWindowPos(lblExtra2,  NULL, fx, row*ROW_H+15, lw, 20, SWP_NOZORDER);
    SetWindowPos(editExtra2, NULL, ex, row*ROW_H+12, ew, 24, SWP_NOZORDER); row++;

    int btnY = row*ROW_H+15;
    SetWindowPos(btnAdd,    NULL, fx, btnY,      fw, 30, SWP_NOZORDER);
    SetWindowPos(btnEdit,   NULL, fx, btnY+38,   fw, 30, SWP_NOZORDER);
    SetWindowPos(btnDelete, NULL, fx, btnY+76,   fw, 30, SWP_NOZORDER);
    SetWindowPos(btnQueue,  NULL, fx, btnY+114,  fw, 30, SWP_NOZORDER);
}

}
