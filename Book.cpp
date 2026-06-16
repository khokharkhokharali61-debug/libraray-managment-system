#include "../include/Book.h"
#include <sstream>
#include <iomanip>
#include <vector>

// ---- Constructors ----

Book::Book()
    : bookID(0), publicationYear(0), quantity(0), availableCopies(0) {}

Book::Book(int id, const string& isbn, const string& title, const string& author,
           const string& publisher, const string& category, const string& edition,
           int pubYear, int qty, const string& shelf)
    : bookID(id), isbn(isbn), title(title), author(author),
      publisher(publisher), category(category), edition(edition),
      publicationYear(pubYear), quantity(qty), availableCopies(qty),
      shelfLocation(shelf) {}

// ---- Borrow / Return ----

bool Book::borrowCopy() {
    if (availableCopies <= 0) return false;
    availableCopies--;
    return true;
}

void Book::returnCopy() {
    if (availableCopies < quantity)
        availableCopies++;
}

// ---- Display ----

void Book::display() const {
    cout << "\n";
    cout << "  Book ID        : " << bookID          << "\n";
    cout << "  ISBN           : " << isbn            << "\n";
    cout << "  Title          : " << title           << "\n";
    cout << "  Author         : " << author          << "\n";
    cout << "  Publisher      : " << publisher       << "\n";
    cout << "  Category       : " << category        << "\n";
    cout << "  Edition        : " << edition         << "\n";
    cout << "  Published Year : " << publicationYear << "\n";
    cout << "  Quantity       : " << quantity        << "\n";
    cout << "  Available      : " << availableCopies << "\n";
    cout << "  Shelf Location : " << shelfLocation   << "\n";
    cout << "  Status         : " << (isAvailable() ? "Available" : "Not Available") << "\n";
}

void Book::displayShort() const {
    cout << left
         << setw(5)  << bookID
         << setw(32) << title.substr(0, 30)
         << setw(22) << author.substr(0, 20)
         << setw(15) << category
         << setw(5)  << availableCopies
         << "\n";
}

// ---- File I/O ----

// Format: ID|ISBN|Title|Author|Publisher|Category|Edition|Year|Qty|Available|Shelf
string Book::serialize() const {
    ostringstream oss;
    oss << bookID       << "|"
        << isbn         << "|"
        << title        << "|"
        << author       << "|"
        << publisher    << "|"
        << category     << "|"
        << edition      << "|"
        << publicationYear << "|"
        << quantity     << "|"
        << availableCopies << "|"
        << shelfLocation;
    return oss.str();
}

bool Book::deserialize(const string& line) {
    if (line.empty() || line[0] == '#') return false;
    istringstream iss(line);
    string tok;
    vector<string> fields;
    while (getline(iss, tok, '|')) fields.push_back(tok);
    if (fields.size() < 11) return false;

    try {
        bookID           = stoi(fields[0]);
        isbn             = fields[1];
        title            = fields[2];
        author           = fields[3];
        publisher        = fields[4];
        category         = fields[5];
        edition          = fields[6];
        publicationYear  = stoi(fields[7]);
        quantity         = stoi(fields[8]);
        availableCopies  = stoi(fields[9]);
        shelfLocation    = fields[10];
    } catch (...) {
        return false;
    }
    return true;
}
