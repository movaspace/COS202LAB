#include "LayoutManager.h"
#include <commctrl.h>

namespace scapms {

void LayoutManager::applyVBoxLayout(HWND parent, const std::vector<HWND>& controls, int startX, int startY, int totalWidth, int controlHeight, int spacing) {
    int currentY = startY;
    for (HWND ctrl : controls) {
        if (ctrl && IsWindow(ctrl)) {
            SetWindowPos(ctrl, NULL, startX, currentY, totalWidth, controlHeight, SWP_NOZORDER | SWP_NOACTIVATE);
            currentY += controlHeight + spacing;
        }
    }
}

void LayoutManager::applyHBoxLayout(HWND parent, const std::vector<HWND>& controls, int startX, int startY, int totalWidth, int controlHeight, int spacing) {
    if (controls.empty()) return;
    int count = static_cast<int>(controls.size());
    int totalGaps = (count - 1) * spacing;
    int itemWidth = (totalWidth - totalGaps) / count;
    int currentX = startX;

    for (HWND ctrl : controls) {
        if (ctrl && IsWindow(ctrl)) {
            SetWindowPos(ctrl, NULL, currentX, startY, itemWidth, controlHeight, SWP_NOZORDER | SWP_NOACTIVATE);
            currentX += itemWidth + spacing;
        }
    }
}

void LayoutManager::applyFormLayout(HWND parent, const std::vector<std::pair<HWND, HWND>>& formRows, int startX, int startY, int labelWidth, int fieldWidth, int rowHeight, int spacing) {
    int currentY = startY;
    for (const auto& row : formRows) {
        HWND lbl = row.first;
        HWND field = row.second;
        if (lbl && IsWindow(lbl)) {
            SetWindowPos(lbl, NULL, startX, currentY + 3, labelWidth, rowHeight - 3, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (field && IsWindow(field)) {
            SetWindowPos(field, NULL, startX + labelWidth + spacing, currentY, fieldWidth, rowHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        currentY += rowHeight + spacing;
    }
}

void LayoutManager::applyGridLayout(HWND parent, const std::vector<HWND>& controls, int startX, int startY, int columns, int cellWidth, int cellHeight, int hSpacing, int vSpacing) {
    if (columns <= 0) columns = 1;
    for (size_t i = 0; i < controls.size(); ++i) {
        HWND ctrl = controls[i];
        if (ctrl && IsWindow(ctrl)) {
            int row = static_cast<int>(i / columns);
            int col = static_cast<int>(i % columns);
            int x = startX + col * (cellWidth + hSpacing);
            int y = startY + row * (cellHeight + vSpacing);
            SetWindowPos(ctrl, NULL, x, y, cellWidth, cellHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}

void LayoutManager::addTooltip(HWND parent, HWND control, const char* text) {
    if (!parent || !control || !text) return;
    HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
                                 WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 parent, NULL, GetModuleHandle(NULL), NULL);

    TOOLINFO ti = {};
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd = parent;
    ti.uId = (UINT_PTR)control;
    ti.lpszText = (LPSTR)text;
    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

}
