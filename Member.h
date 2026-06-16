#ifndef MEMBER_H
#define MEMBER_H

#include "User.h"
#include <vector>

// ============================================================
// Member Class — inherits from User
// Demonstrates: Inheritance, Polymorphism
// ============================================================
class Member : public User {
private:
    string registrationDate;
    string membershipType;   // "Standard" | "Premium" | "Student"
    int    borrowedCount;    // total books currently borrowed
    double totalFinesPaid;

public:
    // Max books a member can borrow at once
    static const int MAX_BORROW_LIMIT = 5;

    Member();
    Member(int id, const string& name, const string& email,
           const string& phone, const string& password,
           const string& memType);

    // Getters
    string getRegistrationDate() const { return registrationDate; }
    string getMembershipType()   const { return membershipType; }
    int    getBorrowedCount()    const { return borrowedCount; }
    double getTotalFinesPaid()   const { return totalFinesPaid; }

    // Setters
    void setMembershipType(const string& v) { membershipType = v; }
    void incrementBorrowed()  { borrowedCount++; }
    void decrementBorrowed()  { if (borrowedCount > 0) borrowedCount--; }
    void addFine(double f)    { totalFinesPaid += f; }
    bool canBorrow()          const { return borrowedCount < MAX_BORROW_LIMIT; }

    // Polymorphic overrides
    void showMenu()       override;
    void displayProfile() const override;

    // File I/O
    string serialize()            const override;
    bool   deserialize(const string& line) override;
};

#endif
