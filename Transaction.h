#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
using namespace std;

// ============================================================
// Transaction — records a single borrow/return event
// Demonstrates: Encapsulation, File Handling
// ============================================================
class Transaction {
private:
    int    transactionID;
    int    memberID;
    int    bookID;
    string bookTitle;
    string memberName;
    string issueDate;
    string dueDate;
    string returnDate;   // empty if not yet returned
    bool   isReturned;
    double fineAmount;

public:
    static const int LOAN_DAYS      = 7;   // free loan period
    static const double FINE_PER_DAY;      // $1 per day overdue

    Transaction();
    Transaction(int txID, int memID, int bkID,
                const string& bookTitle, const string& memberName,
                const string& issueDate);

    // Getters
    int    getTransactionID() const { return transactionID; }
    int    getMemberID()      const { return memberID; }
    int    getBookID()        const { return bookID; }
    string getBookTitle()     const { return bookTitle; }
    string getMemberName()    const { return memberName; }
    string getIssueDate()     const { return issueDate; }
    string getDueDate()       const { return dueDate; }
    string getReturnDate()    const { return returnDate; }
    bool   getIsReturned()    const { return isReturned; }
    double getFineAmount()    const { return fineAmount; }

    // Operations
    double calculateFine() const;    // compute fine based on today
    void   markReturned();           // set returnDate, compute final fine
    bool   isOverdue()     const;

    // Display
    void display()      const;
    void displayShort() const;

    // File I/O
    string serialize()           const;
    bool   deserialize(const string& line);

    // Date helpers (static utilities)
    static string todayString();
    static string addDays(const string& dateStr, int days);
    static int    daysBetween(const string& from, const string& to);
    static time_t parseDate(const string& dateStr);
};

#endif
