#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>
using namespace std;

// ============================================================
// Abstract Base Class: User
// Demonstrates: Abstraction, Inheritance base, Polymorphism
// ============================================================
class User {
protected:
    int    userID;
    string name;
    string email;
    string phone;
    string passwordHash; // simple hash stored as string
    string role;         // "admin" or "member"

public:
    User();
    User(int id, const string& name, const string& email,
         const string& phone, const string& password, const string& role);

    virtual ~User() {}

    // Getters
    int    getUserID()       const { return userID; }
    string getName()         const { return name; }
    string getEmail()        const { return email; }
    string getPhone()        const { return phone; }
    string getRole()         const { return role; }

    // Setters
    void setName(const string& v)  { name = v; }
    void setEmail(const string& v) { email = v; }
    void setPhone(const string& v) { phone = v; }

    // Authentication
    bool authenticate(const string& password) const;
    void setPassword(const string& password);

    // Pure virtual — forces subclasses to implement their own menu
    virtual void showMenu() = 0;

    // Virtual display — can be overridden
    virtual void displayProfile() const;

    // File I/O
    virtual string serialize()            const = 0;
    virtual bool   deserialize(const string& line) = 0;

protected:
    // Simple hash — not cryptographic, sufficient for a uni project
    static string hashPassword(const string& pwd);
};

#endif
