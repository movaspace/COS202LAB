#include "SearchPanel.h"
#include "LayoutManager.h"
#include <sstream>

#define IDC_SRCH_QUERY 3001
#define IDC_SRCH_METHOD 3002
#define IDC_SRCH_BTN 3003
#define IDC_SORT_ALGO 3004
#define IDC_SORT_FIELD 3005
#define IDC_SORT_BTN 3006
#define IDC_SRCH_LIST 3007

namespace scapms {

SearchPanel::SearchPanel(HWND parent, ClinicManager& mgr)
    : parentHwnd(parent), containerHwnd(NULL), manager(mgr),
      editQuery(NULL), cbSearchMethod(NULL), btnSearch(NULL),
      cbSortAlgorithm(NULL), cbSortField(NULL), btnSort(NULL),
      lblExplanation(NULL), listView(NULL) {
    createControls();
}

HWND SearchPanel::getContainer() const {
    return containerHwnd;
}

void SearchPanel::createControls() {
    containerHwnd = CreateWindowEx(0, "SCAPMS_Panel", "", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 800, 600, parentHwnd, NULL, GetModuleHandle(NULL), NULL);

    // Search Controls
    CreateWindowEx(0, "STATIC", "Search:", WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 10, 15, 55, 20, containerHwnd, NULL, GetModuleHandle(NULL), NULL);
    editQuery = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 70, 12, 175, 24, containerHwnd, (HMENU)IDC_SRCH_QUERY, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "STATIC", "Method:", WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 260, 15, 60, 20, containerHwnd, NULL, GetModuleHandle(NULL), NULL);
    cbSearchMethod = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 325, 12, 210, 140, containerHwnd, (HMENU)IDC_SRCH_METHOD, GetModuleHandle(NULL), NULL);
    SendMessage(cbSearchMethod, CB_ADDSTRING, 0, (LPARAM)"Linear Search (by ID)");
    SendMessage(cbSearchMethod, CB_ADDSTRING, 0, (LPARAM)"Binary Search (by ID)");
    SendMessage(cbSearchMethod, CB_ADDSTRING, 0, (LPARAM)"Recursive Search (by Name)");
    SendMessage(cbSearchMethod, CB_SETCURSEL, 0, 0);

    btnSearch = CreateWindowEx(0, "BUTTON", "Search", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 545, 11, 80, 26, containerHwnd, (HMENU)IDC_SRCH_BTN, GetModuleHandle(NULL), NULL);

    // Sort Controls
    CreateWindowEx(0, "STATIC", "Sort By:", WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 10, 50, 55, 20, containerHwnd, NULL, GetModuleHandle(NULL), NULL);
    cbSortField = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 70, 48, 175, 120, containerHwnd, (HMENU)IDC_SORT_FIELD, GetModuleHandle(NULL), NULL);
    SendMessage(cbSortField, CB_ADDSTRING, 0, (LPARAM)"Patient Name");
    SendMessage(cbSortField, CB_ADDSTRING, 0, (LPARAM)"Patient ID");
    SendMessage(cbSortField, CB_ADDSTRING, 0, (LPARAM)"Age");
    SendMessage(cbSortField, CB_SETCURSEL, 0, 0);

    CreateWindowEx(0, "STATIC", "Method:", WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 260, 50, 60, 20, containerHwnd, NULL, GetModuleHandle(NULL), NULL);
    cbSortAlgorithm = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 325, 48, 210, 120, containerHwnd, (HMENU)IDC_SORT_ALGO, GetModuleHandle(NULL), NULL);
    SendMessage(cbSortAlgorithm, CB_ADDSTRING, 0, (LPARAM)"Selection Sort");
    SendMessage(cbSortAlgorithm, CB_ADDSTRING, 0, (LPARAM)"Quick Sort");
    SendMessage(cbSortAlgorithm, CB_ADDSTRING, 0, (LPARAM)"Insertion Sort");
    SendMessage(cbSortAlgorithm, CB_SETCURSEL, 0, 0);

    btnSort = CreateWindowEx(0, "BUTTON", "Sort", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 545, 47, 80, 26, containerHwnd, (HMENU)IDC_SORT_BTN, GetModuleHandle(NULL), NULL);

    LayoutManager::addTooltip(containerHwnd, btnSearch, "Search clinic patient records using selected method");
    LayoutManager::addTooltip(containerHwnd, btnSort, "Sort patient records using selected field and algorithm");

    lblExplanation = CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 82, 760, 36, containerHwnd, NULL, GetModuleHandle(NULL), NULL);

    // Results ListView
    listView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL, 10, 125, 760, 430, containerHwnd, (HMENU)IDC_SRCH_LIST, GetModuleHandle(NULL), NULL);
    ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvc.iSubItem = 0; lvc.cx = 80; lvc.pszText = (LPSTR)"ID";
    ListView_InsertColumn(listView, 0, &lvc);
    lvc.iSubItem = 1; lvc.cx = 90; lvc.pszText = (LPSTR)"Type";
    ListView_InsertColumn(listView, 1, &lvc);
    lvc.iSubItem = 2; lvc.cx = 140; lvc.pszText = (LPSTR)"Name";
    ListView_InsertColumn(listView, 2, &lvc);
    lvc.iSubItem = 3; lvc.cx = 60; lvc.pszText = (LPSTR)"Age";
    ListView_InsertColumn(listView, 3, &lvc);
    lvc.iSubItem = 4; lvc.cx = 110; lvc.pszText = (LPSTR)"Contact";
    ListView_InsertColumn(listView, 4, &lvc);
    lvc.iSubItem = 5; lvc.cx = 250; lvc.pszText = (LPSTR)"Details";
    ListView_InsertColumn(listView, 5, &lvc);

    updateExplanation();
    refreshListWithRecords(manager.getDatabase().getAllPatients());
}

void SearchPanel::updateExplanation() {
    int fieldSel = (int)SendMessage(cbSortField, CB_GETCURSEL, 0, 0);
    std::string fieldName = "Name";
    if (fieldSel == 1) fieldName = "ID";
    else if (fieldSel == 2) fieldName = "Age";
    std::string text = "Showing clinic patient records sorted by " + fieldName + ".";
    SetWindowText(lblExplanation, text.c_str());
}

void SearchPanel::refreshListWithRecords(const std::vector<std::shared_ptr<PatientRecord>>& records) {
    ListView_DeleteAllItems(listView);

    int itemIdx = 0;
    for (const auto& p : records) {
        if (!p) continue;
        LVITEM lvi;
        lvi.mask = LVIF_TEXT;
        lvi.iItem = itemIdx;
        lvi.iSubItem = 0;
        std::string id = p->getId();
        lvi.pszText = (LPSTR)id.c_str();
        ListView_InsertItem(listView, &lvi);

        std::string type = p->getPatientType();
        ListView_SetItemText(listView, itemIdx, 1, (LPSTR)type.c_str());
        std::string name = p->getName();
        ListView_SetItemText(listView, itemIdx, 2, (LPSTR)name.c_str());
        std::string ageStr = std::to_string(p->getAge());
        ListView_SetItemText(listView, itemIdx, 3, (LPSTR)ageStr.c_str());
        std::string contact = p->getContactInfo();
        ListView_SetItemText(listView, itemIdx, 4, (LPSTR)contact.c_str());
        std::string details = p->getDetails();
        ListView_SetItemText(listView, itemIdx, 5, (LPSTR)details.c_str());

        itemIdx++;
    }
}

void SearchPanel::handleCommand(WPARAM wParam, LPARAM lParam) {
    WORD id = LOWORD(wParam);
    WORD code = HIWORD(wParam);

    if (id == IDC_SRCH_BTN) {
        onSearch();
    } else if (id == IDC_SORT_BTN) {
        onSort();
    } else if ((id == IDC_SORT_ALGO || id == IDC_SORT_FIELD) && code == CBN_SELCHANGE) {
        updateExplanation();
    }
}

void SearchPanel::onSearch() {
    char qBuf[128] = {0};
    GetWindowText(editQuery, qBuf, sizeof(qBuf));
    std::string query = qBuf;

    if (query.empty()) {
        refreshListWithRecords(manager.getDatabase().getAllPatients());
        return;
    }

    int methodSel = (int)SendMessage(cbSearchMethod, CB_GETCURSEL, 0, 0);
    std::string method = "Linear Search";
    if (methodSel == 1) method = "Binary Search";
    else if (methodSel == 2) method = "Recursive Search";

    auto record = manager.searchPatient(query, method);
    std::vector<std::shared_ptr<PatientRecord>> results;
    if (record) {
        results.push_back(record);
        refreshListWithRecords(results);
        std::string msg = "Record found: " + record->getName() + " (" + record->getId() + ")";
        MessageBox(containerHwnd, msg.c_str(), "Search Success", MB_OK | MB_ICONINFORMATION);
    } else {
        refreshListWithRecords(results);
        MessageBox(containerHwnd, "No matching patient record found.", "Search Result", MB_ICONINFORMATION);
    }
}

void SearchPanel::onSort() {
    int algoSel = (int)SendMessage(cbSortAlgorithm, CB_GETCURSEL, 0, 0);
    int fieldSel = (int)SendMessage(cbSortField, CB_GETCURSEL, 0, 0);

    std::string algo = "Selection Sort";
    if (algoSel == 1) algo = "Quick Sort";
    else if (algoSel == 2) algo = "Insertion Sort";

    std::string field = "Name";
    if (fieldSel == 1) field = "ID";
    else if (fieldSel == 2) field = "Age";

    updateExplanation();
    auto sorted = manager.getSortedPatients(algo, field);
    refreshListWithRecords(sorted);
}

void SearchPanel::handleResize(int width, int height) {
    if (!containerHwnd) return;
    SetWindowPos(lblExplanation, NULL, 10, 82, width - 20, 36, SWP_NOZORDER);
    SetWindowPos(listView, NULL, 10, 125, width - 20, height - 135, SWP_NOZORDER);
}

}
