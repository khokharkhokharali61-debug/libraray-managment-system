#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include <vector>
#include <string>
using namespace std;

// ============================================================
// ReportManager — generates all library reports
// Demonstrates: Encapsulation, File Handling
// ============================================================
class ReportManager {
private:
    string reportsDir;

    void writeHeader(ofstream& f, const string& title) const;
    void writeFooter(ofstream& f) const;

public:
    ReportManager(const string& dir = "reports/");

    // Console reports
    void availableBooksReport(const vector<Book>& books)           const;
    void borrowedBooksReport(const vector<Transaction>& txns)      const;
    void overdueBooksReport(const vector<Transaction>& txns)       const;
    void memberActivityReport(const vector<Member>& members,
                              const vector<Transaction>& txns)     const;
    void librarySummaryReport(const vector<Book>& books,
                              const vector<Member>& members,
                              const vector<Transaction>& txns)     const;

    // Analytics / dashboard
    void statisticsDashboard(const vector<Book>& books,
                             const vector<Member>& members,
                             const vector<Transaction>& txns)      const;
    void mostBorrowedBooks(const vector<Transaction>& txns,
                           const vector<Book>& books, int topN=5)  const;
    void recentlyAddedBooks(const vector<Book>& books, int n=5)    const;

    // Export to file
    bool exportAvailableBooks(const vector<Book>& books)           const;
    bool exportOverdueBooks(const vector<Transaction>& txns)       const;
    bool exportSummaryReport(const vector<Book>& books,
                             const vector<Member>& members,
                             const vector<Transaction>& txns)      const;
};

#endif
