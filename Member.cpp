#include "../include/Member.h"
#include "../include/UI.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// ---- Constructors ----

Member::Member()
    : User(), borrowedCount(0), totalFinesPaid(0.0) {
    role = "member";
}

Member::Member(int id, const string& name, const string& email,
               const string& phone, const string& password,
               const string& memType)
    : User(id, name, email, phone, password, "member"),
      membershipType(memType), borrowedCount(0), totalFinesPaid(0.0) {
    // Registration date = today
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    registrationDate = string(buf);
}

// ---- Polymorphic Menu ----

void Member::showMenu() {
    cout << "\n  1. Search Books\n"
         << "  2. View Available Books\n"
         << "  3. Borrow a Book\n"
         << "  4. Return a Book\n"
         << "  5. My Borrowing History\n"
         << "  6. Check Due Dates\n"
         << "  7. View My Profile\n"
         << "  8. Logout\n";
}

// ---- Display ----

void Member::displayProfile() const {
    User::displayProfile();
    cout << "  Membership     : " << membershipType   << "\n";
    cout << "  Registered On  : " << registrationDate << "\n";
    cout << "  Books Borrowed : " << borrowedCount    << "\n";
    cout << "  Total Fines    : $" << fixed << setprecision(2) << totalFinesPaid << "\n";
    cout << "  Borrow Limit   : " << MAX_BORROW_LIMIT << "\n";
}

// ---- File I/O ----
// Format: ID|name|email|phone|passwordHash|role|regDate|memType|borrowedCount|totalFines

string Member::serialize() const {
    ostringstream oss;
    oss << userID         << "|"
        << name           << "|"
        << email          << "|"
        << phone          << "|"
        << passwordHash   << "|"
        << role           << "|"
        << registrationDate << "|"
        << membershipType << "|"
        << borrowedCount  << "|"
        << fixed << setprecision(2) << totalFinesPaid;
    return oss.str();
}

bool Member::deserialize(const string& line) {
    if (line.empty() || line[0] == '#') return false;
    istringstream iss(line);
    string tok;
    vector<string> fields;
    while (getline(iss, tok, '|')) fields.push_back(tok);
    if (fields.size() < 10) return false;
    try {
        userID           = stoi(fields[0]);
        name             = fields[1];
        email            = fields[2];
        phone            = fields[3];
        passwordHash     = fields[4];
        role             = fields[5];
        registrationDate = fields[6];
        membershipType   = fields[7];
        borrowedCount    = stoi(fields[8]);
        totalFinesPaid   = stod(fields[9]);
    } catch (...) { return false; }
    return true;
}
