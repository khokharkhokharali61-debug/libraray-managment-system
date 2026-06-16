#include "../include/UI.h"
#include <iomanip>
#include <algorithm>

namespace UI {

void printLine(char ch, int width) {
    cout << Color::CYAN;
    for (int i = 0; i < width; i++) cout << ch;
    cout << Color::RESET << "\n";
}

void printHeader(const string& title) {
    int width = 70;
    cout << "\n" << Color::BOLD << Color::BLUE;
    for (int i = 0; i < width; i++) cout << "=";
    cout << "\n";
    int pad = (width - (int)title.size()) / 2;
    cout << string(pad, ' ') << title << "\n";
    for (int i = 0; i < width; i++) cout << "=";
    cout << Color::RESET << "\n\n";
}

void success(const string& msg) {
    cout << Color::GREEN << "  [OK] " << msg << Color::RESET << "\n";
}

void error(const string& msg) {
    cout << Color::RED << "  [ERROR] " << msg << Color::RESET << "\n";
}

void warning(const string& msg) {
    cout << Color::YELLOW << "  [WARN] " << msg << Color::RESET << "\n";
}

void info(const string& msg) {
    cout << Color::CYAN << "  [INFO] " << msg << Color::RESET << "\n";
}

int getInt(const string& prompt, int minVal, int maxVal) {
    int val;
    while (true) {
        cout << Color::BOLD << "  " << prompt << Color::RESET;
        if (cin >> val && val >= minVal && val <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        error("Invalid input. Please enter a number between " +
              to_string(minVal) + " and " + to_string(maxVal) + ".");
    }
}

double getDouble(const string& prompt, double minVal) {
    double val;
    while (true) {
        cout << Color::BOLD << "  " << prompt << Color::RESET;
        if (cin >> val && val >= minVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        error("Invalid input. Please enter a valid number.");
    }
}

string getString(const string& prompt, bool allowEmpty) {
    string val;
    while (true) {
        cout << Color::BOLD << "  " << prompt << Color::RESET;
        getline(cin, val);
        // Trim
        size_t s = val.find_first_not_of(" \t");
        size_t e = val.find_last_not_of(" \t");
        val = (s == string::npos) ? "" : val.substr(s, e - s + 1);
        if (!val.empty() || allowEmpty) return val;
        error("Input cannot be empty.");
    }
}

string getPassword(const string& prompt) {
    // Simple input — for a real system use termios to hide characters
    cout << Color::BOLD << "  " << prompt << Color::RESET;
    string pwd;
    getline(cin, pwd);
    return pwd;
}

bool confirm(const string& msg) {
    cout << Color::YELLOW << "  " << msg << " (y/n): " << Color::RESET;
    string ans;
    getline(cin, ans);
    return (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y'));
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << Color::CYAN << "\n  Press Enter to continue..." << Color::RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (cin.peek() == '\n') cin.ignore();
}

void tableHeader(const string cols[], int widths[], int n) {
    cout << Color::BOLD << Color::BLUE;
    for (int i = 0; i < n; i++)
        cout << left << setw(widths[i]) << cols[i];
    cout << Color::RESET << "\n";
    tableDivider(widths, n);
}

void tableRow(const string cols[], int widths[], int n) {
    for (int i = 0; i < n; i++)
        cout << left << setw(widths[i]) << cols[i].substr(0, widths[i] - 1);
    cout << "\n";
}

void tableDivider(int widths[], int n) {
    int total = 0;
    for (int i = 0; i < n; i++) total += widths[i];
    cout << Color::CYAN;
    for (int i = 0; i < total; i++) cout << "-";
    cout << Color::RESET << "\n";
}

} // namespace UI
