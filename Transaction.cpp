#include "../include/Transaction.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

const double Transaction::FINE_PER_DAY = 1.0;

// ---- Constructors ----

Transaction::Transaction()
    : transactionID(0), memberID(0), bookID(0),
      isReturned(false), fineAmount(0.0) {}

Transaction::Transaction(int txID, int memID, int bkID,
                         const string& bkTitle, const string& memName,
                         const string& issDate)
    : transactionID(txID), memberID(memID), bookID(bkID),
      bookTitle(bkTitle), memberName(memName), issueDate(issDate),
      isReturned(false), fineAmount(0.0) {
    dueDate = addDays(issDate, LOAN_DAYS);
}

// ---- Fine Calculation ----

double Transaction::calculateFine() const {
    if (isReturned) return fineAmount;
    string today = todayString();
    int overdueDays = daysBetween(dueDate, today);
    if (overdueDays <= 0) return 0.0;
    return overdueDays * FINE_PER_DAY;
}

bool Transaction::isOverdue() const {
    if (isReturned) return false;
    return daysBetween(dueDate, todayString()) > 0;
}

void Transaction::markReturned() {
    returnDate  = todayString();
    isReturned  = true;
    fineAmount  = calculateFine();
}

// ---- Display ----

void Transaction::display() const {
    cout << "  TX ID        : " << transactionID << "\n";
    cout << "  Member       : " << memberName << " (ID: " << memberID << ")\n";
    cout << "  Book         : " << bookTitle  << " (ID: " << bookID  << ")\n";
    cout << "  Issue Date   : " << issueDate  << "\n";
    cout << "  Due Date     : " << dueDate    << "\n";
    if (isReturned) {
        cout << "  Return Date  : " << returnDate << "\n";
        cout << "  Fine Paid    : $" << fixed << setprecision(2) << fineAmount << "\n";
        cout << "  Status       : Returned\n";
    } else {
        double f = calculateFine();
        cout << "  Return Date  : Not Returned\n";
        cout << "  Current Fine : $" << fixed << setprecision(2) << f << "\n";
        cout << "  Status       : " << (isOverdue() ? "OVERDUE" : "Active") << "\n";
    }
}

void Transaction::displayShort() const {
    cout << left
         << setw(6)  << transactionID
         << setw(20) << memberName.substr(0, 18)
         << setw(28) << bookTitle.substr(0, 26)
         << setw(12) << dueDate
         << setw(10) << (isReturned ? "Returned" : (isOverdue() ? "OVERDUE" : "Active"))
         << "$" << fixed << setprecision(2) << calculateFine()
         << "\n";
}

// ---- File I/O ----
// Format: txID|memID|bkID|bookTitle|memberName|issueDate|dueDate|returnDate|isReturned|fine

string Transaction::serialize() const {
    ostringstream oss;
    oss << transactionID << "|"
        << memberID      << "|"
        << bookID        << "|"
        << bookTitle     << "|"
        << memberName    << "|"
        << issueDate     << "|"
        << dueDate       << "|"
        << returnDate    << "|"
        << (isReturned ? 1 : 0) << "|"
        << fixed << setprecision(2) << fineAmount;
    return oss.str();
}

bool Transaction::deserialize(const string& line) {
    if (line.empty() || line[0] == '#') return false;
    istringstream iss(line);
    string tok;
    vector<string> fields;
    while (getline(iss, tok, '|')) fields.push_back(tok);
    if (fields.size() < 10) return false;
    try {
        transactionID = stoi(fields[0]);
        memberID      = stoi(fields[1]);
        bookID        = stoi(fields[2]);
        bookTitle     = fields[3];
        memberName    = fields[4];
        issueDate     = fields[5];
        dueDate       = fields[6];
        returnDate    = fields[7];
        isReturned    = (stoi(fields[8]) == 1);
        fineAmount    = stod(fields[9]);
    } catch (...) { return false; }
    return true;
}

// ---- Date Utilities ----

string Transaction::todayString() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return string(buf);
}

time_t Transaction::parseDate(const string& dateStr) {
    // dateStr format: "YYYY-MM-DD"
    if (dateStr.empty()) return time(nullptr);
    struct tm t = {};
    int y, m, d;
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &y, &m, &d) != 3) return time(nullptr);
    t.tm_year = y - 1900;
    t.tm_mon  = m - 1;
    t.tm_mday = d;
    t.tm_isdst = -1;
    return mktime(&t);
}

string Transaction::addDays(const string& dateStr, int days) {
    time_t t = parseDate(dateStr);
    t += (time_t)days * 86400;
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return string(buf);
}

// Returns positive if 'to' is after 'from', negative if before
int Transaction::daysBetween(const string& from, const string& to) {
    time_t t1 = parseDate(from);
    time_t t2 = parseDate(to);
    double diff = difftime(t2, t1);
    return (int)(diff / 86400.0);
}
