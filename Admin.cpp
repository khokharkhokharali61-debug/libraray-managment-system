#include <vector>
#include "../include/Admin.h"
#include <sstream>

Admin::Admin()
    : User(), department("General"), adminLevel("Admin") {
    role = "admin";
}

Admin::Admin(int id, const string& name, const string& email,
             const string& phone, const string& password,
             const string& dept, const string& level)
    : User(id, name, email, phone, password, "admin"),
      department(dept), adminLevel(level) {}

void Admin::showMenu() {
    cout << "\n  --- Book Management ---\n"
         << "   1. Add New Book\n"
         << "   2. Update Book\n"
         << "   3. Delete Book\n"
         << "   4. Search Books\n"
         << "   5. View All Books\n"
         << "\n  --- Member Management ---\n"
         << "   6. Add Member\n"
         << "   7. Update Member\n"
         << "   8. Delete Member\n"
         << "   9. View All Members\n"
         << "\n  --- Circulation ---\n"
         << "  10. Issue Book\n"
         << "  11. Return Book\n"
         << "  12. View Issued Books\n"
         << "  13. View Overdue Books\n"
         << "\n  --- Reports & Analytics ---\n"
         << "  14. Generate Reports\n"
         << "  15. Statistics Dashboard\n"
         << "  16. Book Recommendations\n"
         << "\n  17. Logout\n";
}

void Admin::displayProfile() const {
    User::displayProfile();
    cout << "  Department : " << department << "\n";
    cout << "  Admin Level: " << adminLevel << "\n";
}

// Format: ID|name|email|phone|passwordHash|role|dept|level
string Admin::serialize() const {
    ostringstream oss;
    oss << userID       << "|"
        << name         << "|"
        << email        << "|"
        << phone        << "|"
        << passwordHash << "|"
        << role         << "|"
        << department   << "|"
        << adminLevel;
    return oss.str();
}

bool Admin::deserialize(const string& line) {
    if (line.empty() || line[0] == '#') return false;
    istringstream iss(line);
    string tok;
    vector<string> fields;
    while (getline(iss, tok, '|')) fields.push_back(tok);
    if (fields.size() < 8) return false;
    try {
        userID       = stoi(fields[0]);
        name         = fields[1];
        email        = fields[2];
        phone        = fields[3];
        passwordHash = fields[4];
        role         = fields[5];
        department   = fields[6];
        adminLevel   = fields[7];
    } catch (...) { return false; }
    return true;
}
