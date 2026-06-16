#include "../include/Library.h"
#include "../include/UI.h"
#include <map>
#include <algorithm>

// ---- Member Management ----

bool Library::addMember(const Member& m) {
    Member nm(nextMemberID++, m.getName(), m.getEmail(),
              m.getPhone(), "", m.getMembershipType());
    // Password was already hashed inside User — copy via serialize/deserialize trick
    // Instead just copy the whole object and assign ID manually:
    Member copy = m;
    // We'll let the caller pass in a fully formed member and just store it
    members.push_back(copy);
    saveMembers();
    return true;
}

bool Library::updateMember(int memberID, const Member& updated) {
    int idx = findMemberIndex(memberID);
    if (idx == -1) return false;
    members[idx].setName(updated.getName());
    members[idx].setEmail(updated.getEmail());
    members[idx].setPhone(updated.getPhone());
    members[idx].setMembershipType(updated.getMembershipType());
    saveMembers();
    return true;
}

bool Library::deleteMember(int memberID) {
    // Cannot delete if active borrows
    for (const auto& t : transactions)
        if (t.getMemberID() == memberID && !t.getIsReturned()) return false;
    int idx = findMemberIndex(memberID);
    if (idx == -1) return false;
    members.erase(members.begin() + idx);
    saveMembers();
    return true;
}

Member* Library::findMember(int memberID) {
    int idx = findMemberIndex(memberID);
    if (idx == -1) return nullptr;
    return &members[idx];
}

Member* Library::findMemberByEmail(const string& email) {
    for (auto& m : members)
        if (m.getEmail() == email) return &m;
    return nullptr;
}

vector<Member> Library::searchMembers(const string& keyword) const {
    vector<Member> result;
    string low = keyword;
    transform(low.begin(), low.end(), low.begin(), ::tolower);
    for (const auto& m : members) {
        string name = m.getName();
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        string email = m.getEmail();
        transform(email.begin(), email.end(), email.begin(), ::tolower);
        if (name.find(low) != string::npos || email.find(low) != string::npos)
            result.push_back(m);
    }
    return result;
}

// ---- Admin Management ----

bool Library::addAdmin(const Admin& a) {
    admins.push_back(a);
    saveAdmins();
    return true;
}

Admin* Library::findAdmin(int adminID) {
    int idx = findAdminIndex(adminID);
    if (idx == -1) return nullptr;
    return &admins[idx];
}

// ---- Borrowing System ----

bool Library::issueBook(int memberID, int bookID) {
    Member* mem = findMember(memberID);
    if (!mem)          { UI::error("Member not found."); return false; }
    if (!mem->canBorrow()){ UI::error("Borrow limit reached (max 5 books)."); return false; }

    int bIdx = findBookIndex(bookID);
    if (bIdx == -1)    { UI::error("Book not found."); return false; }
    if (!books[bIdx].isAvailable()) { UI::error("No copies available."); return false; }

    // Check not already borrowed by same member
    if (findActiveTransaction(memberID, bookID) != -1) {
        UI::error("Member already has this book."); return false;
    }

    books[bIdx].borrowCopy();
    mem->incrementBorrowed();

    Transaction tx(nextTransactionID++, memberID, bookID,
                   books[bIdx].getTitle(), mem->getName(),
                   Transaction::todayString());
    transactions.push_back(tx);

    saveBooks();
    saveMembers();
    saveTransactions();

    UI::success("Book issued. Due date: " + tx.getDueDate());
    return true;
}

bool Library::returnBook(int memberID, int bookID) {
    int txIdx = findActiveTransaction(memberID, bookID);
    if (txIdx == -1) { UI::error("No active borrow record found."); return false; }

    transactions[txIdx].markReturned();
    double fine = transactions[txIdx].getFineAmount();

    int bIdx = findBookIndex(bookID);
    if (bIdx != -1) books[bIdx].returnCopy();

    Member* mem = findMember(memberID);
    if (mem) {
        mem->decrementBorrowed();
        if (fine > 0) mem->addFine(fine);
    }

    saveBooks();
    saveMembers();
    saveTransactions();

    if (fine > 0)
        UI::warning("Book returned with a fine of $" + to_string(fine).substr(0,4));
    else
        UI::success("Book returned successfully. No fine.");

    return true;
}

double Library::calculateCurrentFine(int memberID, int bookID) const {
    int txIdx = findActiveTransaction(memberID, bookID);
    if (txIdx == -1) return 0.0;
    return transactions[txIdx].calculateFine();
}

// ---- Transactions ----

vector<Transaction> Library::getActiveTransactions() const {
    vector<Transaction> result;
    for (const auto& t : transactions) if (!t.getIsReturned()) result.push_back(t);
    return result;
}

vector<Transaction> Library::getOverdueTransactions() const {
    vector<Transaction> result;
    for (const auto& t : transactions)
        if (!t.getIsReturned() && t.isOverdue()) result.push_back(t);
    return result;
}

vector<Transaction> Library::getMemberHistory(int memberID) const {
    vector<Transaction> result;
    for (const auto& t : transactions)
        if (t.getMemberID() == memberID) result.push_back(t);
    return result;
}

// ---- Sorting ----

vector<Book> Library::sortByTitle() const {
    vector<Book> sorted = books;
    sort(sorted.begin(), sorted.end(), [](const Book& a, const Book& b){
        return a.getTitle() < b.getTitle();
    });
    return sorted;
}

vector<Book> Library::sortByAuthor() const {
    vector<Book> sorted = books;
    sort(sorted.begin(), sorted.end(), [](const Book& a, const Book& b){
        return a.getAuthor() < b.getAuthor();
    });
    return sorted;
}

vector<Book> Library::sortByPublicationYear() const {
    vector<Book> sorted = books;
    sort(sorted.begin(), sorted.end(), [](const Book& a, const Book& b){
        return a.getPublicationYear() > b.getPublicationYear();
    });
    return sorted;
}

// ---- Recommendations ----

vector<Book> Library::getRecommendations(int memberID, int count) const {
    // Find member's most borrowed category
    map<string, int> catFreq;
    for (const auto& t : transactions) {
        if (t.getMemberID() == memberID) {
            int idx = findBookIndex(t.getBookID());
            if (idx != -1) catFreq[books[idx].getCategory()]++;
        }
    }
    string topCat;
    int best = 0;
    for (auto& p : catFreq) if (p.second > best) { best = p.second; topCat = p.first; }

    vector<Book> recs;
    if (!topCat.empty()) {
        for (const auto& b : books) {
            if (b.getCategory() == topCat && b.isAvailable()) {
                // Check not already borrowed
                bool alreadyHas = false;
                for (const auto& t : transactions)
                    if (t.getMemberID() == memberID && t.getBookID() == b.getBookID() && !t.getIsReturned())
                        alreadyHas = true;
                if (!alreadyHas) recs.push_back(b);
                if ((int)recs.size() >= count) break;
            }
        }
    }
    // Pad with any available books
    if ((int)recs.size() < count) {
        for (const auto& b : books) {
            if ((int)recs.size() >= count) break;
            bool already = false;
            for (const auto& r : recs) if (r.getBookID() == b.getBookID()) { already = true; break; }
            if (!already && b.isAvailable()) recs.push_back(b);
        }
    }
    return recs;
}

vector<Book> Library::getRecentlyAdded(int n) const {
    vector<Book> result;
    int start = max(0, (int)books.size() - n);
    for (int i = (int)books.size() - 1; i >= start; i--)
        result.push_back(books[i]);
    return result;
}

// ---- Statistics ----

int Library::totalBorrowedNow() const {
    int cnt = 0;
    for (const auto& t : transactions) if (!t.getIsReturned()) cnt++;
    return cnt;
}

int Library::totalOverdue() const {
    int cnt = 0;
    for (const auto& t : transactions) if (t.isOverdue()) cnt++;
    return cnt;
}

double Library::totalFinesOwed() const {
    double total = 0;
    for (const auto& t : transactions) total += t.calculateFine();
    return total;
}

// ---- Authentication ----

Admin* Library::loginAdmin(int id, const string& pwd) {
    Admin* a = findAdmin(id);
    if (a && a->authenticate(pwd)) return a;
    return nullptr;
}

Member* Library::loginMember(int id, const string& pwd) {
    Member* m = findMember(id);
    if (m && m->authenticate(pwd)) return m;
    return nullptr;
}

// ---- Seed Demo Data ----

void Library::seedDemoData() {
    if (!books.empty() || !admins.empty()) return; // already seeded

    // Admin
    Admin superAdmin(nextAdminID++, "Dr. Sarah Hassan", "admin@library.edu",
                     "0300-1234567", "admin123", "Library Administration", "SuperAdmin");
    admins.push_back(superAdmin);

    // Members
    auto addM = [&](const string& name, const string& email, const string& phone,
                    const string& pwd, const string& type) {
        Member m(nextMemberID++, name, email, phone, pwd, type);
        members.push_back(m);
    };
    addM("Ali Ahmed",     "ali@student.edu",   "0312-1111111", "pass123", "Student");
    addM("Sana Malik",    "sana@student.edu",  "0312-2222222", "pass123", "Student");
    addM("Umar Sheikh",   "umar@faculty.edu",  "0321-3333333", "pass123", "Premium");
    addM("Fatima Khan",   "fatima@gmail.com",  "0333-4444444", "pass123", "Standard");
    addM("Bilal Qureshi", "bilal@student.edu", "0345-5555555", "pass123", "Student");

    // Books
    auto addB = [&](const string& isbn, const string& title, const string& author,
                    const string& pub, const string& cat, const string& ed,
                    int yr, int qty, const string& shelf) {
        Book b(nextBookID++, isbn, title, author, pub, cat, ed, yr, qty, shelf);
        books.push_back(b);
    };
    addB("978-0-13-468599", "C++ Primer",              "Stanley Lippman",    "Addison-Wesley",   "Programming",  "5th", 2012, 4, "A1-01");
    addB("978-0-13-110362", "The C Programming Language","Kernighan & Ritchie","Prentice Hall",   "Programming",  "2nd", 1988, 3, "A1-02");
    addB("978-0-20-161622", "Design Patterns",          "Gang of Four",        "Addison-Wesley",  "Programming",  "1st", 1994, 2, "A1-03");
    addB("978-0-13-235088", "Clean Code",               "Robert C. Martin",    "Prentice Hall",   "Programming",  "1st", 2008, 5, "A1-04");
    addB("978-0-59-651798", "Introduction to Algorithms","Cormen et al.",      "MIT Press",       "CS Theory",    "3rd", 2009, 3, "B2-01");
    addB("978-1-59-327584", "The Linux Command Line",   "William Shotts",      "No Starch Press", "Systems",      "2nd", 2019, 4, "B2-02");
    addB("978-0-38-597650", "Artificial Intelligence",  "Russell & Norvig",    "Pearson",         "AI",           "4th", 2020, 3, "C3-01");
    addB("978-0-59-651949", "Python Crash Course",      "Eric Matthes",        "No Starch Press", "Programming",  "3rd", 2022, 6, "A1-05");
    addB("978-0-13-607755", "Operating System Concepts","Silberschatz et al.", "Wiley",           "Systems",      "10th",2018, 4, "B2-03");
    addB("978-0-13-103805", "Database System Concepts", "Silberschatz et al.", "McGraw-Hill",     "Databases",    "7th", 2019, 4, "C3-02");
    addB("978-0-13-235086", "Computer Networks",        "Andrew Tanenbaum",    "Prentice Hall",   "Networks",     "5th", 2010, 3, "D4-01");
    addB("978-0-06-112008", "To Kill a Mockingbird",    "Harper Lee",          "Harper Perennial","Literature",   "1st", 1960, 5, "E5-01");
    addB("978-0-74-327356", "1984",                     "George Orwell",       "Signet Classic",  "Literature",   "1st", 1949, 4, "E5-02");
    addB("978-0-06-093546", "The Great Gatsby",         "F. Scott Fitzgerald", "Scribner",        "Literature",   "1st", 1925, 3, "E5-03");
    addB("978-0-14-028329", "Calculus",                 "James Stewart",       "Cengage",         "Mathematics",  "8th", 2015, 5, "F6-01");

    saveAll();
    UI::success("Demo data seeded: 1 admin, 5 members, 15 books.");
}
