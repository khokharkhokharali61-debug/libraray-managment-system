#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "Member.h"
#include "Admin.h"
#include "Transaction.h"
#include "ReportManager.h"
#include <vector>
#include <string>
using namespace std;

// ============================================================
// Library — central controller class
// Owns all data, handles all persistence, coordinates actions
// Demonstrates: Aggregation, File Handling, Function Overloading
// ============================================================
class Library {
private:
    string          libraryName;
    vector<Book>    books;
    vector<Member>  members;
    vector<Admin>   admins;
    vector<Transaction> transactions;
    ReportManager   reporter;

    int nextBookID;
    int nextMemberID;
    int nextAdminID;
    int nextTransactionID;

    // File paths
    const string BOOKS_FILE    = "data/books.txt";
    const string MEMBERS_FILE  = "data/members.txt";
    const string ADMINS_FILE   = "data/admins.txt";
    const string ISSUED_FILE   = "data/issued_books.txt";
    const string HISTORY_FILE  = "data/history.txt";

    // Internal helpers
    int  findBookIndex(int bookID)     const;
    int  findMemberIndex(int memberID) const;
    int  findAdminIndex(int adminID)   const;
    int  findActiveTransaction(int memberID, int bookID) const;

    void loadIDs();
    void ensureDataDir();

public:
    Library(const string& name = "City Library");
    ~Library();

    // ---- Persistence ----
    void loadAll();
    void saveAll() const;
    void loadBooks();
    void saveBooks()       const;
    void loadMembers();
    void saveMembers()     const;
    void loadAdmins();
    void saveAdmins()      const;
    void loadTransactions();
    void saveTransactions() const;

    // ---- Book Management ----
    bool addBook(const Book& b);
    bool updateBook(int bookID, const Book& updated);
    bool deleteBook(int bookID);

    // Function overloading — search by different criteria
    vector<Book> searchByID(int id)              const;
    vector<Book> searchByTitle(const string& t)  const;
    vector<Book> searchByAuthor(const string& a) const;
    vector<Book> searchByCategory(const string& c) const;
    vector<Book> searchBooks(const string& keyword) const; // smart search
    vector<Book> getAllBooks()                    const { return books; }
    vector<Book> getAvailableBooks()             const;

    // ---- Member Management ----
    bool addMember(const Member& m);
    bool updateMember(int memberID, const Member& updated);
    bool deleteMember(int memberID);
    Member* findMember(int memberID);
    Member* findMemberByEmail(const string& email);
    vector<Member> getAllMembers()               const { return members; }
    vector<Member> searchMembers(const string& keyword) const;

    // ---- Admin Management ----
    bool  addAdmin(const Admin& a);
    Admin* findAdmin(int adminID);

    // ---- Borrowing System ----
    bool issueBook(int memberID, int bookID);
    bool returnBook(int memberID, int bookID);
    double calculateCurrentFine(int memberID, int bookID) const;

    // ---- Transactions / History ----
    vector<Transaction> getActiveTransactions()          const;
    vector<Transaction> getOverdueTransactions()         const;
    vector<Transaction> getMemberHistory(int memberID)   const;
    vector<Transaction> getAllTransactions()             const { return transactions; }

    // ---- Sorting ----
    vector<Book> sortByTitle()           const;
    vector<Book> sortByAuthor()          const;
    vector<Book> sortByPublicationYear() const;

    // ---- Recommendations ----
    vector<Book> getRecommendations(int memberID, int count = 5) const;
    vector<Book> getRecentlyAdded(int n = 5)                     const;

    // ---- Statistics ----
    int    totalBooks()        const { return (int)books.size(); }
    int    totalMembers()      const { return (int)members.size(); }
    int    totalBorrowedNow()  const;
    int    totalOverdue()      const;
    double totalFinesOwed()    const;

    // ---- Authentication ----
    Admin*  loginAdmin(int id, const string& pwd);
    Member* loginMember(int id, const string& pwd);

    // ---- Reports ----
    ReportManager& getReporter() { return reporter; }

    string getName() const { return libraryName; }

    // Seed demo data if files are empty
    void seedDemoData();
};

#endif
