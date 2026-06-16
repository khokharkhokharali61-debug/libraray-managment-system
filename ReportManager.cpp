#include "../include/ReportManager.h"
#include "../include/UI.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <ctime>
using namespace std;

ReportManager::ReportManager(const string& dir) : reportsDir(dir) {}

void ReportManager::writeHeader(ofstream& f, const string& title) const {
    f << "======================================================================\n";
    f << "  " << title << "\n";
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "  Generated: %Y-%m-%d %H:%M:%S", localtime(&now));
    f << buf << "\n";
    f << "======================================================================\n\n";
}

void ReportManager::writeFooter(ofstream& f) const {
    f << "\n--- End of Report ---\n";
}

// ---- Console Reports ----

void ReportManager::availableBooksReport(const vector<Book>& books) const {
    UI::printHeader("AVAILABLE BOOKS REPORT");
    string cols[] = {"ID", "Title", "Author", "Category", "Available", "Shelf"};
    int    wids[] = {5,    30,      22,       15,          10,          10};
    UI::tableHeader(cols, wids, 6);
    int shown = 0;
    for (const auto& b : books) {
        if (b.isAvailable()) {
            string row[] = {
                to_string(b.getBookID()),
                b.getTitle(),
                b.getAuthor(),
                b.getCategory(),
                to_string(b.getAvailableCopies()),
                b.getShelfLocation()
            };
            UI::tableRow(row, wids, 6);
            shown++;
        }
    }
    UI::printLine();
    cout << "  Total available books: " << shown << "\n";
}

void ReportManager::borrowedBooksReport(const vector<Transaction>& txns) const {
    UI::printHeader("CURRENTLY BORROWED BOOKS");
    string cols[] = {"TX#", "Member", "Book Title", "Due Date", "Status"};
    int    wids[] = {6,     22,       28,            12,         12};
    UI::tableHeader(cols, wids, 5);
    int cnt = 0;
    for (const auto& t : txns) {
        if (!t.getIsReturned()) {
            string row[] = {
                to_string(t.getTransactionID()),
                t.getMemberName(),
                t.getBookTitle(),
                t.getDueDate(),
                t.isOverdue() ? "OVERDUE" : "Active"
            };
            UI::tableRow(row, wids, 5);
            cnt++;
        }
    }
    UI::printLine();
    cout << "  Total borrowed: " << cnt << "\n";
}

void ReportManager::overdueBooksReport(const vector<Transaction>& txns) const {
    UI::printHeader("OVERDUE BOOKS REPORT");
    string cols[] = {"TX#","Member","Book","Due Date","Days Late","Fine"};
    int    wids[] = {6,    22,      26,    12,         10,         8};
    UI::tableHeader(cols, wids, 6);
    int cnt = 0;
    double totalFine = 0;
    for (const auto& t : txns) {
        if (!t.getIsReturned() && t.isOverdue()) {
            double fine = t.calculateFine();
            totalFine += fine;
            int days = Transaction::daysBetween(t.getDueDate(), Transaction::todayString());
            string row[] = {
                to_string(t.getTransactionID()),
                t.getMemberName(),
                t.getBookTitle(),
                t.getDueDate(),
                to_string(days) + " days",
                "$" + to_string((int)fine)
            };
            UI::tableRow(row, wids, 6);
            cnt++;
        }
    }
    UI::printLine();
    cout << "  Overdue count   : " << cnt << "\n";
    cout << "  Total fines due : $" << fixed << setprecision(2) << totalFine << "\n";
}

void ReportManager::memberActivityReport(const vector<Member>& members,
                                          const vector<Transaction>& txns) const {
    UI::printHeader("MEMBER ACTIVITY REPORT");
    for (const auto& m : members) {
        cout << Color::BOLD << "  " << m.getName()
             << " (ID: " << m.getUserID() << ")\n" << Color::RESET;
        int cnt = 0;
        for (const auto& t : txns) {
            if (t.getMemberID() == m.getUserID()) {
                cout << "    - " << t.getBookTitle()
                     << " | Issued: " << t.getIssueDate()
                     << " | " << (t.getIsReturned() ? "Returned " + t.getReturnDate() : "Active")
                     << "\n";
                cnt++;
            }
        }
        if (cnt == 0) cout << "    No activity.\n";
        cout << "\n";
    }
}

void ReportManager::librarySummaryReport(const vector<Book>& books,
                                          const vector<Member>& members,
                                          const vector<Transaction>& txns) const {
    UI::printHeader("LIBRARY SUMMARY REPORT");
    int totalBooks = 0, totalCopies = 0, availCopies = 0;
    for (const auto& b : books) {
        totalBooks++;
        totalCopies  += b.getQuantity();
        availCopies  += b.getAvailableCopies();
    }
    int active = 0, overdue = 0;
    double finesOwed = 0;
    for (const auto& t : txns) {
        if (!t.getIsReturned()) { active++; finesOwed += t.calculateFine(); }
        if (t.isOverdue())       overdue++;
    }

    cout << "  Total Book Titles    : " << totalBooks   << "\n";
    cout << "  Total Copies         : " << totalCopies  << "\n";
    cout << "  Available Copies     : " << availCopies  << "\n";
    cout << "  Books Borrowed Now   : " << active       << "\n";
    cout << "  Overdue              : " << overdue      << "\n";
    cout << "  Total Members        : " << members.size()<< "\n";
    cout << "  Fines Outstanding    : $" << fixed << setprecision(2) << finesOwed << "\n";
}

void ReportManager::statisticsDashboard(const vector<Book>& books,
                                         const vector<Member>& members,
                                         const vector<Transaction>& txns) const {
    UI::printHeader("LIBRARY STATISTICS DASHBOARD");
    // Category breakdown
    map<string, int> catCount;
    for (const auto& b : books) catCount[b.getCategory()] += b.getQuantity();
    cout << Color::BOLD << "  Books by Category:\n" << Color::RESET;
    for (const auto& p : catCount)
        cout << "    " << left << setw(20) << p.first << " : " << p.second << " copies\n";

    cout << "\n";
    mostBorrowedBooks(txns, books, 5);
    cout << "\n";
    recentlyAddedBooks(books, 5);
}

void ReportManager::mostBorrowedBooks(const vector<Transaction>& txns,
                                       const vector<Book>& books, int topN) const {
    map<int, int> freq;
    map<int, string> bkTitle;
    for (const auto& t : txns) { freq[t.getBookID()]++; bkTitle[t.getBookID()] = t.getBookTitle(); }

    vector<pair<int,int>> sorted(freq.begin(), freq.end());
    sort(sorted.begin(), sorted.end(), [](auto& a, auto& b){ return a.second > b.second; });

    cout << Color::BOLD << "  Most Borrowed Books (Top " << topN << "):\n" << Color::RESET;
    int shown = 0;
    for (auto& p : sorted) {
        if (shown >= topN) break;
        cout << "    " << (shown + 1) << ". " << bkTitle[p.first]
             << " — borrowed " << p.second << " times\n";
        shown++;
    }
    if (shown == 0) cout << "    No borrow history yet.\n";
}

void ReportManager::recentlyAddedBooks(const vector<Book>& books, int n) const {
    cout << Color::BOLD << "  Recently Added Books (Last " << n << "):\n" << Color::RESET;
    int start = max(0, (int)books.size() - n);
    for (int i = (int)books.size() - 1; i >= start; i--) {
        cout << "    - " << books[i].getTitle()
             << " by " << books[i].getAuthor()
             << " (" << books[i].getPublicationYear() << ")\n";
    }
}

// ---- File Exports ----

bool ReportManager::exportAvailableBooks(const vector<Book>& books) const {
    string path = reportsDir + "available_books.txt";
    ofstream f(path);
    if (!f) return false;
    writeHeader(f, "AVAILABLE BOOKS REPORT");
    f << left << setw(5) << "ID" << setw(30) << "Title"
      << setw(22) << "Author" << setw(10) << "Available" << "\n";
    f << string(67, '-') << "\n";
    for (const auto& b : books) {
        if (b.isAvailable())
            f << setw(5) << b.getBookID() << setw(30) << b.getTitle()
              << setw(22) << b.getAuthor() << setw(10) << b.getAvailableCopies() << "\n";
    }
    writeFooter(f);
    return true;
}

bool ReportManager::exportOverdueBooks(const vector<Transaction>& txns) const {
    string path = reportsDir + "overdue_books.txt";
    ofstream f(path);
    if (!f) return false;
    writeHeader(f, "OVERDUE BOOKS REPORT");
    for (const auto& t : txns) {
        if (!t.getIsReturned() && t.isOverdue()) {
            f << "TX: " << t.getTransactionID()
              << " | Member: " << t.getMemberName()
              << " | Book: " << t.getBookTitle()
              << " | Due: " << t.getDueDate()
              << " | Fine: $" << fixed << setprecision(2) << t.calculateFine()
              << "\n";
        }
    }
    writeFooter(f);
    return true;
}

bool ReportManager::exportSummaryReport(const vector<Book>& books,
                                         const vector<Member>& members,
                                         const vector<Transaction>& txns) const {
    string path = reportsDir + "summary_report.txt";
    ofstream f(path);
    if (!f) return false;
    writeHeader(f, "LIBRARY SUMMARY REPORT");
    f << "Total Books   : " << books.size()   << "\n";
    f << "Total Members : " << members.size() << "\n";
    int active = 0;
    for (const auto& t : txns) if (!t.getIsReturned()) active++;
    f << "Books Borrowed: " << active << "\n";
    writeFooter(f);
    return true;
}
