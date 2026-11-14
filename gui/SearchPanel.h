#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <windows.h>
#include <commctrl.h>
#include "../controller/ClinicManager.h"

namespace scapms {

class SearchPanel {
private:
    HWND parentHwnd;
    HWND containerHwnd;
    ClinicManager& manager;

    // Controls
    HWND editQuery;
    HWND cbSearchMethod;
    HWND btnSearch;
    HWND cbSortAlgorithm;
    HWND cbSortField;
    HWND btnSort;
    HWND lblExplanation;
    HWND listView;

    void createControls();
    void updateExplanation();

public:
    SearchPanel(HWND parent, ClinicManager& mgr);
    ~SearchPanel() = default;

    HWND getContainer() const;
    void handleCommand(WPARAM wParam, LPARAM lParam);
    void handleResize(int width, int height);
    void onSearch();
    void onSort();
    void refreshListWithRecords(const std::vector<std::shared_ptr<PatientRecord>>& records);
};

}

#endif
