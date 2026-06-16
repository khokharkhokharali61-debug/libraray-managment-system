#include "../include/Library.h"
#include "../include/UI.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <sys/stat.h>

// ---- Constructor / Destructor ----

Library::Library(const string& name)
    : libraryName(name), reporter("reports/"),
      nextBookID(1), nextMemberID(1), nextAdminID(1), nextTransactionID(1) {
    ensureDataDir();
}

Library::~Library() {
    saveAll();
}

void Library::ensureDataDir() {
    mkdir("data", 0755);
    mkdir("reports", 0755);
}

// ---- Persistence ----

void Library::loadAll() {
    loadBooks();
    loadMembers();
    loadAdmins();
    loadTransactions();
    loadIDs();
}

void Library::saveAll() const {
    saveBooks();
    saveMembers();
    saveAdmins();
    saveTransactions();
}

void Library::loadIDs() {
    if (!books.empty())
        nextBookID = max_element(books.begin(), books.end(),
            [](const Book& a, const Book& b){ return a.getBookID() < b.getBookID(); })->getBookID() + 1;
    if (!members.empty())
        nextMemberID = max_element(members.begin(), members.end(),
            [](const Member& a, const Member& b){ return a.getUserID() < b.getUserID(); })->getUserID() + 1;
    if (!admins.empty())
        nextAdminID = max_element(admins.begin(), admins.end(),
            [](const Admin& a, const Admin& b){ return a.getUserID() < b.getUserID(); })->getUserID() + 1;
    if (!transactions.empty())
        nextTransactionID = max_element(transactions.begin(), transactions.end(),
            [](const Transaction& a, const Transaction& b){ return a.getTransactionID() < b.getTransactionID(); })->getTransactionID() + 1;
}

void Library::loadBooks() {
    books.clear();
    ifstream f(BOOKS_FILE);
    if (!f) return;
    string line;
    while (getline(f, line)) {
        Book b;
        if (b.deserialize(line)) books.push_back(b);
    }
}

void Library::saveBooks() const {
    ofstream f(BOOKS_FILE);
    for (const auto& b : books) f << b.serialize() << "\n";
}

void Library::loadMembers() {
    members.clear();
    ifstream f(MEMBERS_FILE);
    if (!f) return;
    string line;
    while (getline(f, line)) {
        Member m;
        if (m.deserialize(line)) members.push_back(m);
    }
}

void Library::saveMembers() const {
    ofstream f(MEMBERS_FILE);
    for (const auto& m : members) f << m.serialize() << "\n";
}

void Library::loadAdmins() {
    admins.clear();
    ifstream f(ADMINS_FILE);
    if (!f) return;
    string line;
    while (getline(f, line)) {
        Admin a;
        if (a.deserialize(line)) admins.push_back(a);
    }
}

void Library::saveAdmins() const {
    ofstream f(ADMINS_FILE);
    for (const auto& a : admins) f << a.serialize() << "\n";
}

void Library::loadTransactions() {
    transactions.clear();
    ifstream f(ISSUED_FILE);
    string line;
    while (getline(f, line)) {
        Transaction t;
        if (t.deserialize(line)) transactions.push_back(t);
    }
    // Also load history
    ifstream fh(HISTORY_FILE);
    while (getline(fh, line)) {
        Transaction t;
        if (t.deserialize(line)) {
            // Only add if not already loaded
            bool found = false;
            for (const auto& ex : transactions)
                if (ex.getTransactionID() == t.getTransactionID()) { found = true; break; }
            if (!found) transactions.push_back(t);
        }
    }
}

void Library::saveTransactions() const {
    ofstream fActive(ISSUED_FILE);
    ofstream fHistory(HISTORY_FILE);
    for (const auto& t : transactions) {
        if (!t.getIsReturned())
            fActive  << t.serialize() << "\n";
        else
            fHistory << t.serialize() << "\n";
    }
}

// ---- Internal Helpers ----

int Library::findBookIndex(int bookID) const {
    for (int i = 0; i < (int)books.size(); i++)
        if (books[i].getBookID() == bookID) return i;
    return -1;
}

int Library::findMemberIndex(int memberID) const {
    for (int i = 0; i < (int)members.size(); i++)
        if (members[i].getUserID() == memberID) return i;
    return -1;
}

int Library::findAdminIndex(int adminID) const {
    for (int i = 0; i < (int)admins.size(); i++)
        if (admins[i].getUserID() == adminID) return i;
    return -1;
}

int Library::findActiveTransaction(int memberID, int bookID) const {
    for (int i = 0; i < (int)transactions.size(); i++) {
        const auto& t = transactions[i];
        if (t.getMemberID() == memberID && t.getBookID() == bookID && !t.getIsReturned())
            return i;
    }
    return -1;
}

// ---- Book Management ----

bool Library::addBook(const Book& b) {
    // Assign auto-ID
    Book nb = b;
    // Use reflection via const cast + reassign
    Book newBook(nextBookID++,
                 nb.getISBN(), nb.getTitle(), nb.getAuthor(),
                 nb.getPublisher(), nb.getCategory(), nb.getEdition(),
                 nb.getPublicationYear(), nb.getQuantity(), nb.getShelfLocation());
    books.push_back(newBook);
    saveBooks();
    return true;
}

bool Library::updateBook(int bookID, const Book& updated) {
    int idx = findBookIndex(bookID);
    if (idx == -1) return false;
    // Keep ID and available copies consistent
    int avail = books[idx].getAvailableCopies();
    int oldQty = books[idx].getQuantity();
    int diff = updated.getQuantity() - oldQty;
    Book nb(bookID, updated.getISBN(), updated.getTitle(), updated.getAuthor(),
            updated.getPublisher(), updated.getCategory(), updated.getEdition(),
            updated.getPublicationYear(), updated.getQuantity(), updated.getShelfLocation());
    // Adjust available copies proportionally
    int newAvail = max(0, avail + diff);
    nb.setAvailableCopies(newAvail);
    books[idx] = nb;
    saveBooks();
    return true;
}

bool Library::deleteBook(int bookID) {
    // Check if any active transaction
    for (const auto& t : transactions)
        if (t.getBookID() == bookID && !t.getIsReturned()) return false;
    int idx = findBookIndex(bookID);
    if (idx == -1) return false;
    books.erase(books.begin() + idx);
    saveBooks();
    return true;
}

// ---- Search (Function Overloading) ----

vector<Book> Library::searchByID(int id) const {
    vector<Book> result;
    int idx = findBookIndex(id);
    if (idx != -1) result.push_back(books[idx]);
    return result;
}

vector<Book> Library::searchByTitle(const string& t) const {
    vector<Book> result;
    string low = t;
    transform(low.begin(), low.end(), low.begin(), ::tolower);
    for (const auto& b : books) {
        string bt = b.getTitle();
        transform(bt.begin(), bt.end(), bt.begin(), ::tolower);
        if (bt.find(low) != string::npos) result.push_back(b);
    }
    return result;
}

vector<Book> Library::searchByAuthor(const string& a) const {
    vector<Book> result;
    string low = a;
    transform(low.begin(), low.end(), low.begin(), ::tolower);
    for (const auto& b : books) {
        string ba = b.getAuthor();
        transform(ba.begin(), ba.end(), ba.begin(), ::tolower);
        if (ba.find(low) != string::npos) result.push_back(b);
    }
    return result;
}

vector<Book> Library::searchByCategory(const string& c) const {
    vector<Book> result;
    string low = c;
    transform(low.begin(), low.end(), low.begin(), ::tolower);
    for (const auto& b : books) {
        string bc = b.getCategory();
        transform(bc.begin(), bc.end(), bc.begin(), ::tolower);
        if (bc.find(low) != string::npos) result.push_back(b);
    }
    return result;
}

// Smart search — checks all text fields
vector<Book> Library::searchBooks(const string& keyword) const {
    vector<Book> result;
    string low = keyword;
    transform(low.begin(), low.end(), low.begin(), ::tolower);
    for (const auto& b : books) {
        auto contains = [&](const string& s) {
            string sl = s;
            transform(sl.begin(), sl.end(), sl.begin(), ::tolower);
            return sl.find(low) != string::npos;
        };
        if (contains(b.getTitle()) || contains(b.getAuthor()) ||
            contains(b.getCategory()) || contains(b.getPublisher()) ||
            contains(b.getISBN()))
            result.push_back(b);
    }
    return result;
}

vector<Book> Library::getAvailableBooks() const {
    vector<Book> result;
    for (const auto& b : books) if (b.isAvailable()) result.push_back(b);
    return result;
}
