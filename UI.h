#ifndef UI_H
#define UI_H

#include <string>
#include <iostream>
#include <limits>
using namespace std;

// ============================================================
// UI Utilities — console colors, input helpers, menus
// ============================================================

// ANSI color codes
namespace Color {
    const string RESET   = "\033[0m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string BLUE    = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN    = "\033[36m";
    const string WHITE   = "\033[37m";
    const string BOLD    = "\033[1m";
    const string BG_BLUE = "\033[44m";
}

namespace UI {
    // Print a horizontal divider
    void printLine(char ch = '-', int width = 70);

    // Print a centered title inside a box
    void printHeader(const string& title);

    // Print colored status messages
    void success(const string& msg);
    void error(const string& msg);
    void warning(const string& msg);
    void info(const string& msg);

    // Input helpers — validate and re-prompt on bad input
    int    getInt(const string& prompt, int min = 0, int max = 999999);
    double getDouble(const string& prompt, double min = 0);
    string getString(const string& prompt, bool allowEmpty = false);
    string getPassword(const string& prompt = "Password: ");

    // Confirmation prompt
    bool confirm(const string& msg);

    // Clear screen (cross-platform)
    void clearScreen();

    // Pause and wait
    void pause();

    // Table row printing helpers
    void tableHeader(const string cols[], int widths[], int n);
    void tableRow(const string cols[], int widths[], int n);
    void tableDivider(int widths[], int n);
}

#endif
