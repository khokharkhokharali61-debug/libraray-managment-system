/*
 * ============================================================
 *  Library Management System
 *  Author   : [Your Name]
 *  Course   : CS-2xx Object-Oriented Programming
 *  Version  : 1.0
 *  Compiler : g++ (C++17)
 * ============================================================
 */

#include "../include/Library.h"
#include "../include/UI.h"
#include <iomanip>
#include <algorithm>

// ---- Forward Declarations ----
void adminMenu(Library& lib, Admin* admin);
void memberMenu(Library& lib, Member* member);

void showAllBooks(Library& lib, const vector<Book>& books);
void bookManagementMenu(Library& lib);
void memberManagementMenu(Library& lib);
void searchMenu(Library& lib);
void reportMenu(Library& lib);

// ---- Utility: print book list as table ----
void printBookTable(const vector<Book>& books) {
    if (books.empty()) { UI::warning("No books found."); return; }
    string cols[] = {"ID", "Title", "Author", "Category", "Avail", "Shelf"};
    int    wids[] = {5,    32,      22,       15,          6,       8};
    UI::tableHeader(cols, wids, 6);
    for (const auto& b : books) {
        string row[] = {
            to_string(b.getBookID()),
            b.getTitle(),
            b.getAuthor(),
            b.getCategory(),
            to_string(b.getAvailableCopies()),
            b.getShelfLocation()
        };
        if (!b.isAvailable()) cout << Color::YELLOW;
        UI::tableRow(row, wids, 6);
        if (!b.isAvailable()) cout << Color::RESET;
    }
    UI::printLine();
    cout << "  Total: " << books.size() << " books\n";
}

// ---- Add Book Wizard ----
void addBookWizard(Library& lib) {
    UI::printHeader("ADD NEW BOOK");
    string isbn    = UI::getString("ISBN            : ");
    string title   = UI::getString("Title           : ");
    string author  = UI::getString("Author          : ");
    string pub     = UI::getString("Publisher       : ");

    cout << "\n  Categories: Programming, CS Theory, Systems, AI, Databases,\n"
         << "              Networks, Mathematics, Literature, Other\n";
    string cat  = UI::getString("Category        : ");
    string ed   = UI::getString("Edition         : ");
    int    yr   = UI::getInt   ("Publication Year: ", 1000, 2030);
    int    qty  = UI::getInt   ("Quantity        : ", 1, 1000);
    string shelf= UI::getString("Shelf Location  : ");

    Book b(0, isbn, title, author, pub, cat, ed, yr, qty, shelf);
    if (lib.addBook(b))
        UI::success("Book added successfully!");
    else
        UI::error("Failed to add book.");
}

// ---- Update Book Wizard ----
void updateBookWizard(Library& lib) {
    UI::printHeader("UPDATE BOOK");
    int id = UI::getInt("Enter Book ID to update: ", 1);
    auto results = lib.searchByID(id);
    if (results.empty()) { UI::error("Book not found."); return; }

    const Book& old = results[0];
    old.display();
    cout << "\n  (Press Enter to keep existing value)\n\n";

    auto keepOrNew = [](const string& prompt, const string& current) {
        cout << "  " << prompt << " [" << current << "]: ";
        string val;
        getline(cin, val);
        if (val.empty()) return current;
        size_t s = val.find_first_not_of(" \t");
        return (s == string::npos) ? current : val.substr(s);
    };

    string isbn  = keepOrNew("ISBN",            old.getISBN());
    string title = keepOrNew("Title",           old.getTitle());
    string auth  = keepOrNew("Author",          old.getAuthor());
    string pub   = keepOrNew("Publisher",       old.getPublisher());
    string cat   = keepOrNew("Category",        old.getCategory());
    string ed    = keepOrNew("Edition",         old.getEdition());
    string shelf = keepOrNew("Shelf Location",  old.getShelfLocation());

    cout << "  Publication Year [" << old.getPublicationYear() << "]: ";
    string yrStr; getline(cin, yrStr);
    int yr = yrStr.empty() ? old.getPublicationYear() : stoi(yrStr);

    cout << "  Quantity [" << old.getQuantity() << "]: ";
    string qtyStr; getline(cin, qtyStr);
    int qty = qtyStr.empty() ? old.getQuantity() : stoi(qtyStr);

    Book updated(id, isbn, title, auth, pub, cat, ed, yr, qty, shelf);
    if (lib.updateBook(id, updated))
        UI::success("Book updated.");
    else
        UI::error("Update failed.");
}

// ---- Book Management Menu ----
void bookManagementMenu(Library& lib) {
    while (true) {
        UI::printHeader("BOOK MANAGEMENT");
        cout << "  1. Add New Book\n"
             << "  2. Update Book\n"
             << "  3. Delete Book\n"
             << "  4. View All Books\n"
             << "  5. Sort by Title\n"
             << "  6. Sort by Author\n"
             << "  7. Sort by Year\n"
             << "  0. Back\n";
        int ch = UI::getInt("Choice: ", 0, 7);

        switch (ch) {
        case 1: addBookWizard(lib); break;
        case 2: updateBookWizard(lib); break;
        case 3: {
            int id = UI::getInt("Book ID to delete: ", 1);
            if (UI::confirm("Delete book ID " + to_string(id) + "?"))
                lib.deleteBook(id) ? UI::success("Deleted.") : UI::error("Cannot delete (active borrows or not found).");
            break;
        }
        case 4: printBookTable(lib.getAllBooks()); break;
        case 5: printBookTable(lib.sortByTitle());  break;
        case 6: printBookTable(lib.sortByAuthor()); break;
        case 7: printBookTable(lib.sortByPublicationYear()); break;
        case 0: return;
        }
        UI::pause();
    }
}

// ---- Member Management Menu ----
void memberManagementMenu(Library& lib) {
    while (true) {
        UI::printHeader("MEMBER MANAGEMENT");
        cout << "  1. Add Member\n"
             << "  2. Update Member\n"
             << "  3. Delete Member\n"
             << "  4. View All Members\n"
             << "  5. Search Member\n"
             << "  0. Back\n";
        int ch = UI::getInt("Choice: ", 0, 5);

        switch (ch) {
        case 1: {
            UI::printHeader("ADD MEMBER");
            string name  = UI::getString("Name          : ");
            string email = UI::getString("Email         : ");
            string phone = UI::getString("Phone         : ");
            string pwd   = UI::getPassword("Password      : ");
            cout << "  Membership Types: Student | Standard | Premium\n";
            string mtype = UI::getString("Membership Type: ");
            Member m(0, name, email, phone, pwd, mtype);
            lib.addMember(m) ? UI::success("Member added. ID: " + to_string(m.getUserID())) : UI::error("Failed.");
            break;
        }
        case 2: {
            int id = UI::getInt("Member ID: ", 1);
            Member* m = lib.findMember(id);
            if (!m) { UI::error("Not found."); break; }
            m->displayProfile();
            string name  = UI::getString("New Name  [" + m->getName()  + "]: ", true);
            string email = UI::getString("New Email [" + m->getEmail() + "]: ", true);
            string phone = UI::getString("New Phone [" + m->getPhone() + "]: ", true);
            string mtype = UI::getString("New Type  [" + m->getMembershipType() + "]: ", true);
            Member upd = *m;
            if (!name.empty())  upd.setName(name);
            if (!email.empty()) upd.setEmail(email);
            if (!phone.empty()) upd.setPhone(phone);
            if (!mtype.empty()) upd.setMembershipType(mtype);
            lib.updateMember(id, upd) ? UI::success("Updated.") : UI::error("Failed.");
            break;
        }
        case 3: {
            int id = UI::getInt("Member ID to delete: ", 1);
            if (UI::confirm("Delete member " + to_string(id) + "?"))
                lib.deleteMember(id) ? UI::success("Deleted.") : UI::error("Cannot delete (active borrows or not found).");
            break;
        }
        case 4: {
            UI::printHeader("ALL MEMBERS");
            string cols[] = {"ID","Name","Email","Phone","Type","Borrowed"};
            int    wids[] = {5,   22,    25,     15,     10,    8};
            UI::tableHeader(cols, wids, 6);
            for (const auto& m : lib.getAllMembers()) {
                string row[] = {
                    to_string(m.getUserID()), m.getName(), m.getEmail(),
                    m.getPhone(), m.getMembershipType(), to_string(m.getBorrowedCount())
                };
                UI::tableRow(row, wids, 6);
            }
            break;
        }
        case 5: {
            string kw = UI::getString("Search (name/email): ");
            auto res = lib.searchMembers(kw);
            if (res.empty()) { UI::warning("No members found."); break; }
            for (const auto& m : res) { m.displayProfile(); UI::printLine(); }
            break;
        }
        case 0: return;
        }
        UI::pause();
    }
}

// ---- Search Menu ----
void searchMenu(Library& lib) {
    while (true) {
        UI::printHeader("SEARCH BOOKS");
        cout << "  1. Smart Search (all fields)\n"
             << "  2. Search by Title\n"
             << "  3. Search by Author\n"
             << "  4. Search by Category\n"
             << "  5. Search by Book ID\n"
             << "  0. Back\n";
        int ch = UI::getInt("Choice: ", 0, 5);
        if (ch == 0) return;

        vector<Book> results;
        switch (ch) {
        case 1: results = lib.searchBooks(UI::getString("Keyword: ")); break;
        case 2: results = lib.searchByTitle(UI::getString("Title: ")); break;
        case 3: results = lib.searchByAuthor(UI::getString("Author: ")); break;
        case 4: results = lib.searchByCategory(UI::getString("Category: ")); break;
        case 5: results = lib.searchByID(UI::getInt("Book ID: ", 1)); break;
        }
        if (results.empty()) UI::warning("No results found.");
        else {
            printBookTable(results);
            if (results.size() == 1) {
                if (UI::confirm("View full details?")) results[0].display();
            }
        }
        UI::pause();
    }
}

// ---- Report Menu ----
void reportMenu(Library& lib) {
    while (true) {
        UI::printHeader("REPORTS");
        cout << "  1. Available Books Report\n"
             << "  2. Borrowed Books Report\n"
             << "  3. Overdue Books Report\n"
             << "  4. Member Activity Report\n"
             << "  5. Library Summary Report\n"
             << "  6. Export Available Books to File\n"
             << "  7. Export Overdue Books to File\n"
             << "  8. Export Summary to File\n"
             << "  0. Back\n";
        int ch = UI::getInt("Choice: ", 0, 8);
        auto& r = lib.getReporter();

        switch (ch) {
        case 1: r.availableBooksReport(lib.getAllBooks()); break;
        case 2: r.borrowedBooksReport(lib.getAllTransactions()); break;
        case 3: r.overdueBooksReport(lib.getAllTransactions()); break;
        case 4: r.memberActivityReport(lib.getAllMembers(), lib.getAllTransactions()); break;
        case 5: r.librarySummaryReport(lib.getAllBooks(), lib.getAllMembers(), lib.getAllTransactions()); break;
        case 6: r.exportAvailableBooks(lib.getAllBooks()) ? UI::success("Exported to reports/available_books.txt") : UI::error("Export failed."); break;
        case 7: r.exportOverdueBooks(lib.getAllTransactions())  ? UI::success("Exported to reports/overdue_books.txt")   : UI::error("Export failed."); break;
        case 8: r.exportSummaryReport(lib.getAllBooks(), lib.getAllMembers(), lib.getAllTransactions()) ? UI::success("Exported to reports/summary_report.txt") : UI::error("Export failed."); break;
        case 0: return;
        }
        if (ch != 0) UI::pause();
    }
}

// ---- Admin Menu ----
void adminMenu(Library& lib, Admin* admin) {
    while (true) {
        UI::printHeader("ADMIN PANEL — Welcome, " + admin->getName());
        admin->showMenu();
        int ch = UI::getInt("Choice: ", 0, 17);

        switch (ch) {
        case  1: bookManagementMenu(lib); continue;
        case  2: bookManagementMenu(lib); continue;  // handled inside
        case  3: bookManagementMenu(lib); continue;
        case  4: searchMenu(lib); continue;
        case  5: printBookTable(lib.getAllBooks()); UI::pause(); break;
        case  6: memberManagementMenu(lib); continue;
        case  7: memberManagementMenu(lib); continue;
        case  8: memberManagementMenu(lib); continue;
        case  9: memberManagementMenu(lib); continue;
        case 10: {
            UI::printHeader("ISSUE BOOK");
            int mid = UI::getInt("Member ID : ", 1);
            int bid = UI::getInt("Book   ID : ", 1);
            lib.issueBook(mid, bid);
            UI::pause();
            break;
        }
        case 11: {
            UI::printHeader("RETURN BOOK");
            int mid = UI::getInt("Member ID : ", 1);
            int bid = UI::getInt("Book   ID : ", 1);
            double f = lib.calculateCurrentFine(mid, bid);
            if (f > 0)
                UI::warning("Fine due: $" + to_string(f).substr(0,4) + " — collect before returning.");
            lib.returnBook(mid, bid);
            UI::pause();
            break;
        }
        case 12: {
            UI::printHeader("ISSUED BOOKS");
            auto active = lib.getActiveTransactions();
            string cols[] = {"TX#","Member","Book","Issued","Due","Status"};
            int    wids[] = {6,    20,      26,    12,      12,   10};
            UI::tableHeader(cols, wids, 6);
            for (const auto& t : active) {
                string row[] = {
                    to_string(t.getTransactionID()), t.getMemberName(), t.getBookTitle(),
                    t.getIssueDate(), t.getDueDate(), t.isOverdue() ? "OVERDUE" : "Active"
                };
                if (t.isOverdue()) cout << Color::RED;
                UI::tableRow(row, wids, 6);
                if (t.isOverdue()) cout << Color::RESET;
            }
            cout << "\n  Total active: " << active.size() << "\n";
            UI::pause();
            break;
        }
        case 13: {
            lib.getReporter().overdueBooksReport(lib.getAllTransactions());
            UI::pause();
            break;
        }
        case 14: reportMenu(lib); continue;
        case 15: lib.getReporter().statisticsDashboard(lib.getAllBooks(), lib.getAllMembers(), lib.getAllTransactions()); UI::pause(); break;
        case 16: {
            UI::printHeader("BOOK RECOMMENDATIONS");
            int mid = UI::getInt("Member ID: ", 1);
            auto recs = lib.getRecommendations(mid, 5);
            if (recs.empty()) UI::warning("No recommendations found.");
            else printBookTable(recs);
            UI::pause();
            break;
        }
        case 17: UI::success("Logged out."); return;
        case  0: UI::success("Logged out."); return;
        }
    }
}

// ---- Member Menu ----
void memberMenu(Library& lib, Member* member) {
    while (true) {
        UI::printHeader("MEMBER PORTAL — " + member->getName());
        member->showMenu();
        int ch = UI::getInt("Choice: ", 0, 8);

        switch (ch) {
        case 1: searchMenu(lib); continue;
        case 2: {
            UI::printHeader("AVAILABLE BOOKS");
            printBookTable(lib.getAvailableBooks());
            UI::pause();
            break;
        }
        case 3: {
            UI::printHeader("BORROW A BOOK");
            printBookTable(lib.getAvailableBooks());
            int bid = UI::getInt("Enter Book ID to borrow: ", 1);
            lib.issueBook(member->getUserID(), bid);
            UI::pause();
            break;
        }
        case 4: {
            UI::printHeader("RETURN A BOOK");
            auto hist = lib.getMemberHistory(member->getUserID());
            for (const auto& t : hist)
                if (!t.getIsReturned()) t.displayShort();
            int bid = UI::getInt("Enter Book ID to return: ", 1);
            double f = lib.calculateCurrentFine(member->getUserID(), bid);
            if (f > 0) UI::warning("Fine due: $" + to_string(f).substr(0, 4));
            lib.returnBook(member->getUserID(), bid);
            UI::pause();
            break;
        }
        case 5: {
            UI::printHeader("MY BORROWING HISTORY");
            auto hist = lib.getMemberHistory(member->getUserID());
            if (hist.empty()) { UI::info("No history yet."); }
            else for (const auto& t : hist) { t.display(); UI::printLine('.'); }
            UI::pause();
            break;
        }
        case 6: {
            UI::printHeader("DUE DATES");
            auto active = lib.getActiveTransactions();
            bool found = false;
            for (const auto& t : active) {
                if (t.getMemberID() == member->getUserID()) {
                    cout << "  " << t.getBookTitle()
                         << " — Due: " << t.getDueDate();
                    if (t.isOverdue())
                        cout << Color::RED << " [OVERDUE $"
                             << fixed << setprecision(2) << t.calculateFine() << "]" << Color::RESET;
                    cout << "\n";
                    found = true;
                }
            }
            if (!found) UI::info("No active borrows.");
            UI::pause();
            break;
        }
        case 7: {
            UI::printHeader("MY PROFILE");
            // Refresh pointer — borrows may have changed
            Member* m = lib.findMember(member->getUserID());
            if (m) m->displayProfile();
            UI::pause();
            break;
        }
        case 8: UI::success("Logged out."); return;
        case 0: UI::success("Logged out."); return;
        }
    }
}

// ======================================================
// MAIN
// ======================================================
int main() {
    Library lib("City University Library");
    lib.loadAll();

    // First-run: seed demo data if nothing stored
    if (lib.totalBooks() == 0) {
        lib.seedDemoData();
        cout << "\n";
        UI::info("First run detected — demo data loaded.");
        UI::info("Default admin  : ID=1, Password=admin123");
        UI::info("Sample members : ID=1-5, Password=pass123");
        cout << "\n";
    }

    while (true) {
        UI::clearScreen();
        UI::printHeader(lib.getName() + " — Management System");

        cout << Color::BOLD;
        cout << "  [1] Administrator Login\n";
        cout << "  [2] Member Login\n";
        cout << "  [3] Search Books (Guest)\n";
        cout << "  [4] View Available Books (Guest)\n";
        cout << "  [0] Exit\n";
        cout << Color::RESET << "\n";

        int choice = UI::getInt("Select an option: ", 0, 4);

        switch (choice) {
        case 1: {
            UI::printHeader("ADMINISTRATOR LOGIN");
            int id    = UI::getInt("Admin ID : ", 1);
            string pwd= UI::getPassword("Password : ");
            Admin* a  = lib.loginAdmin(id, pwd);
            if (a) {
                UI::success("Login successful.");
                adminMenu(lib, a);
            } else {
                UI::error("Invalid credentials.");
                UI::pause();
            }
            break;
        }
        case 2: {
            UI::printHeader("MEMBER LOGIN");
            int id    = UI::getInt("Member ID: ", 1);
            string pwd= UI::getPassword("Password : ");
            Member* m = lib.loginMember(id, pwd);
            if (m) {
                UI::success("Welcome, " + m->getName() + "!");
                memberMenu(lib, m);
            } else {
                UI::error("Invalid credentials.");
                UI::pause();
            }
            break;
        }
        case 3:
            searchMenu(lib);
            break;
        case 4:
            UI::printHeader("AVAILABLE BOOKS");
            printBookTable(lib.getAvailableBooks());
            UI::pause();
            break;
        case 0:
            lib.saveAll();
            UI::printHeader("Thank you for using the Library System!");
            return 0;
        }
    }
}
