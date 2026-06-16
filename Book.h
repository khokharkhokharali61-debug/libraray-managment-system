#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <iostream>
using namespace std;

// ============================================================
// Book Class - Represents a single book in the library system
// Demonstrates: Encapsulation, Operator Overloading
// ============================================================
class Book {
private:
    int     bookID;
    string  isbn;
    string  title;
    string  author;
    string  publisher;
    string  category;
    string  edition;
    int     publicationYear;
    int     quantity;
    int     availableCopies;
    string  shelfLocation;

public:
    // Constructors
    Book();
    Book(int id, const string& isbn, const string& title, const string& author,
         const string& publisher, const string& category, const string& edition,
         int pubYear, int qty, const string& shelf);

    // Getters
    int     getBookID()         const { return bookID; }
    string  getISBN()           const { return isbn; }
    string  getTitle()          const { return title; }
    string  getAuthor()         const { return author; }
    string  getPublisher()      const { return publisher; }
    string  getCategory()       const { return category; }
    string  getEdition()        const { return edition; }
    int     getPublicationYear()const { return publicationYear; }
    int     getQuantity()       const { return quantity; }
    int     getAvailableCopies()const { return availableCopies; }
    string  getShelfLocation()  const { return shelfLocation; }
    bool    isAvailable()       const { return availableCopies > 0; }

    // Setters
    void setISBN(const string& v)        { isbn = v; }
    void setTitle(const string& v)       { title = v; }
    void setAuthor(const string& v)      { author = v; }
    void setPublisher(const string& v)   { publisher = v; }
    void setCategory(const string& v)    { category = v; }
    void setEdition(const string& v)     { edition = v; }
    void setPublicationYear(int v)       { publicationYear = v; }
    void setQuantity(int v)              { quantity = v; }
    void setAvailableCopies(int v)       { availableCopies = v; }
    void setShelfLocation(const string& v){ shelfLocation = v; }

    // Borrow / Return operations
    bool borrowCopy();   // decrements availableCopies
    void returnCopy();   // increments availableCopies

    // Display
    void display() const;
    void displayShort() const;

    // Operator overloading — compare books by ID
    bool operator==(const Book& other) const { return bookID == other.bookID; }
    bool operator<(const Book& other)  const { return title < other.title; }

    // File I/O helpers
    string serialize()   const;   // convert to pipe-delimited string
    bool   deserialize(const string& line); // parse from pipe-delimited string
};

#endif
