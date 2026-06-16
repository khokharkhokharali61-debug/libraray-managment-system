# Library Management System — Project Documentation

**Course:** CS-2xx Object-Oriented Programming  
**Semester:** 2nd Semester  
**Language:** C++ (C++17)  
**Build System:** GNU Make  

---

## 1. Project Overview

A console-based Library Management System built entirely with OOP principles.
It supports two user roles (Admin, Member), full book and member management,
a borrowing system with fine calculation, file-based persistence, and reports.

---

## 2. UML Class Diagram

```
                        +------------------+
                        |    <<abstract>>  |
                        |      User        |
                        |------------------|
                        | # userID: int    |
                        | # name: string   |
                        | # email: string  |
                        | # phone: string  |
                        | # passwordHash   |
                        | # role: string   |
                        |------------------|
                        | + authenticate() |
                        | + setPassword()  |
                        | + showMenu()=0   |  <-- pure virtual
                        | + serialize()=0  |  <-- pure virtual
                        +--------+---------+
                                 |
               +-----------------+-----------------+
               |                                   |
    +----------+----------+           +-----------+-----------+
    |        Member       |           |         Admin         |
    |---------------------|           |-----------------------|
    | - registrationDate  |           | - department: string  |
    | - membershipType    |           | - adminLevel: string  |
    | - borrowedCount:int |           |-----------------------|
    | - totalFinesPaid    |           | + showMenu() override |
    |---------------------|           | + displayProfile()    |
    | + canBorrow(): bool |           | + serialize() override|
    | + showMenu() override           +-----------------------+
    | + displayProfile()  |
    | + serialize()       |
    +---------------------+


    +----------------------------+
    |            Book            |
    |----------------------------|
    | - bookID: int              |
    | - isbn, title, author      |
    | - publisher, category      |
    | - edition: string          |
    | - publicationYear: int     |
    | - quantity: int            |
    | - availableCopies: int     |
    | - shelfLocation: string    |
    |----------------------------|
    | + borrowCopy(): bool       |
    | + returnCopy()             |
    | + isAvailable(): bool      |
    | + display()                |
    | + operator==               |
    | + operator<                |
    | + serialize() / deserialize|
    +----------------------------+


    +------------------------------+
    |         Transaction          |
    |------------------------------|
    | - transactionID: int         |
    | - memberID, bookID: int      |
    | - bookTitle, memberName      |
    | - issueDate, dueDate         |
    | - returnDate: string         |
    | - isReturned: bool           |
    | - fineAmount: double         |
    |------------------------------|
    | + calculateFine(): double    |
    | + markReturned()             |
    | + isOverdue(): bool          |
    | + todayString() [static]     |
    | + addDays() [static]         |
    | + daysBetween() [static]     |
    | + serialize() / deserialize  |
    +------------------------------+


    +---------------------------------------------+
    |                   Library                    |
    |---------------------------------------------|
    | - books: vector<Book>                        |
    | - members: vector<Member>                    |
    | - admins: vector<Admin>                      |
    | - transactions: vector<Transaction>          |
    | - reporter: ReportManager                    |
    | - nextBookID, nextMemberID, ... : int        |
    |---------------------------------------------|
    | + addBook() / updateBook() / deleteBook()    |
    | + searchByID() / searchByTitle() ...         |  (overloaded)
    | + addMember() / updateMember() / delete...   |
    | + issueBook() / returnBook()                 |
    | + getRecommendations()                       |
    | + sortByTitle() / sortByAuthor() ...         |
    | + loginAdmin() / loginMember()               |
    | + loadAll() / saveAll()                      |
    | + seedDemoData()                             |
    +---------------------------------------------+
                     | uses
    +----------------+------------------+
    |          ReportManager            |
    |-----------------------------------|
    | - reportsDir: string              |
    |-----------------------------------|
    | + availableBooksReport()          |
    | + borrowedBooksReport()           |
    | + overdueBooksReport()            |
    | + memberActivityReport()          |
    | + librarySummaryReport()          |
    | + statisticsDashboard()           |
    | + mostBorrowedBooks()             |
    | + exportAvailableBooks()          |
    | + exportOverdueBooks()            |
    | + exportSummaryReport()           |
    +-----------------------------------+
```

---

## 3. OOP Concepts Demonstrated

| Concept              | Where Used |
|----------------------|------------|
| **Classes & Objects**| All 7 classes; objects created in Library, main.cpp |
| **Encapsulation**    | All private data accessed through getters/setters |
| **Inheritance**      | Member → User, Admin → User |
| **Polymorphism**     | `showMenu()`, `displayProfile()`, `serialize()` are virtual; called through User* |
| **Abstraction**      | User is abstract (pure virtual `showMenu()`, `serialize()`) |
| **Constructors**     | Default + parameterized in every class |
| **Destructors**      | Library destructor calls `saveAll()` automatically |
| **File Handling**    | 4 data files; pipe-delimited serialization/deserialization |
| **Function Overloading** | `searchByID()`, `searchByTitle()`, `searchByAuthor()`, `searchByCategory()` |
| **Operator Overloading** | `Book::operator==` (compare by ID), `Book::operator<` (compare by title) |

---

## 4. File Structure

```
LibraryMS/
├── Makefile
├── README.md           ← this file
├── include/
│   ├── Book.h
│   ├── User.h
│   ├── Member.h
│   ├── Admin.h
│   ├── Transaction.h
│   ├── ReportManager.h
│   ├── Library.h
│   └── UI.h
├── src/
│   ├── main.cpp
│   ├── Book.cpp
│   ├── User.cpp
│   ├── Member.cpp
│   ├── Admin.cpp
│   ├── Transaction.cpp
│   ├── ReportManager.cpp
│   ├── Library.cpp     ← persistence + book/search
│   ├── Library2.cpp    ← members, transactions, analytics, seed
│   └── UI.cpp
├── data/               ← auto-created on first run
│   ├── books.txt
│   ├── members.txt
│   ├── admins.txt
│   ├── issued_books.txt
│   └── history.txt
└── reports/            ← auto-created on first run
    ├── available_books.txt
    ├── overdue_books.txt
    └── summary_report.txt
```

---

## 5. Data File Formats

**books.txt** — pipe-delimited:
```
ID|ISBN|Title|Author|Publisher|Category|Edition|Year|Qty|AvailCopies|Shelf
1|978-0-13|C++ Primer|Stanley Lippman|Addison-Wesley|Programming|5th|2012|4|4|A1-01
```

**members.txt** — pipe-delimited:
```
ID|Name|Email|Phone|PasswordHash|Role|RegDate|MemberType|BorrowCount|TotalFines
1|Ali Ahmed|ali@student.edu|...|HASH|member|2026-06-16|Student|0|0.00
```

**issued_books.txt** (active borrows):
```
TxID|MemID|BkID|BookTitle|MemberName|IssueDate|DueDate|ReturnDate|IsReturned|Fine
```

**history.txt** (returned books — same format, IsReturned=1):

---

## 6. Fine Rules

- **First 7 days:** free  
- **After due date:** $1.00 per day  
- Fine is computed live (`calculateFine()` checks today's date vs due date)  
- On return, final fine is stored in the transaction record and added to member's total

---

## 7. How to Build and Run

```bash
# Requires: g++ with C++17 support
make          # compile
make run      # compile + run
make clean    # remove object files and binary
```

First run seeds 15 books, 5 members, and 1 admin automatically.

**Default credentials:**
- Admin:  ID=1, Password=`admin123`  
- Members: ID=1–5, Password=`pass123`

---

## 8. Feature Checklist

### Admin Features
- [x] Add / Update / Delete books  
- [x] Add / Update / Delete members  
- [x] Issue book to member  
- [x] Return book (with fine calculation)  
- [x] View all books / members  
- [x] View issued books  
- [x] View overdue books  
- [x] Generate 5 report types  
- [x] Export reports to text files  
- [x] Statistics dashboard  
- [x] Book recommendations  

### Member Features
- [x] Password-protected login  
- [x] Search books (smart search across all fields)  
- [x] View available books  
- [x] Borrow books (enforces max 5 limit)  
- [x] Return books  
- [x] View borrowing history  
- [x] Check due dates and outstanding fines  
- [x] View profile  

### System Features
- [x] Colored console interface (ANSI codes)  
- [x] Persistent file storage (survives restart)  
- [x] Sorting by title, author, year  
- [x] Smart search (all text fields)  
- [x] Category-based recommendations  
- [x] Most borrowed books analytics  
- [x] Recently added books  
- [x] Export reports to `reports/` directory  
- [x] Guest access (search + view without login)  

---

## 9. Design Decisions

**Why pipe (`|`) delimiters?** Commas appear inside book titles/author names. Pipes are safer without adding a CSV parser dependency.

**Why split Library into Library.cpp + Library2.cpp?** Single file exceeded 400 lines. Split by responsibility: persistence+search vs. members+transactions+analytics.

**Why djb2 for password hashing?** A real system needs bcrypt. For a university project, a deterministic hash is sufficient to demonstrate the concept without adding dependencies.

**Why no reservations?** The spec lists it as a bonus. The Transaction model can be extended with a `status` field ("reserved"|"issued"|"returned") to add it.

---

## 10. Known Limitations

1. Password input is not masked (no asterisks). Fixing this requires `termios.h` on Linux or `conio.h` on Windows — excluded for cross-platform simplicity.  
2. No concurrent access control — single-user console application.  
3. File format breaks if a book title/author contains a `|` character (validation not enforced).
