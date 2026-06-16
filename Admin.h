#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

// ============================================================
// Admin Class — inherits from User
// Demonstrates: Inheritance, Polymorphism
// ============================================================
class Admin : public User {
private:
    string department;
    string adminLevel; // "SuperAdmin" | "Admin"

public:
    Admin();
    Admin(int id, const string& name, const string& email,
          const string& phone, const string& password,
          const string& dept, const string& level = "Admin");

    // Getters
    string getDepartment() const { return department; }
    string getAdminLevel() const { return adminLevel; }

    // Polymorphic overrides
    void showMenu()       override;
    void displayProfile() const override;

    // File I/O
    string serialize()            const override;
    bool   deserialize(const string& line) override;
};

#endif
