#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include <windows.h>
#include <vector>

namespace scapms {

struct ControlBox {
    HWND hwnd;
    int width;
    int height;
    int stretchFactor;
};

class LayoutManager {
public:
    // Vertical Box Layout arranges controls sequentially from top to bottom
    static void applyVBoxLayout(HWND parent, const std::vector<HWND>& controls, int startX, int startY, int totalWidth, int controlHeight, int spacing);

    // Horizontal Box Layout arranges controls sequentially from left to right
    static void applyHBoxLayout(HWND parent, const std::vector<HWND>& controls, int startX, int startY, int totalWidth, int controlHeight, int spacing);

    // Form Layout aligns label controls on the left and input fields on the right
    static void applyFormLayout(HWND parent, const std::vector<std::pair<HWND, HWND>>& formRows, int startX, int startY, int labelWidth, int fieldWidth, int rowHeight, int spacing);

    // Grid Layout positions controls into regular rows and columns
    static void applyGridLayout(HWND parent, const std::vector<HWND>& controls, int startX, int startY, int columns, int cellWidth, int cellHeight, int hSpacing, int vSpacing);

    // Tooltip helper attaches a native tooltip to any control
    static void addTooltip(HWND parent, HWND control, const char* text);
};

}

#endif
