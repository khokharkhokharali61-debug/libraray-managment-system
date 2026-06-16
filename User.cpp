#include "../include/User.h"
#include <sstream>
#include <functional>

// ---- Constructors ----

User::User() : userID(0) {}

User::User(int id, const string& name, const string& email,
           const string& phone, const string& password, const string& role)
    : userID(id), name(name), email(email), phone(phone), role(role) {
    setPassword(password);
}

// ---- Authentication ----

bool User::authenticate(const string& password) const {
    return passwordHash == hashPassword(password);
}

void User::setPassword(const string& password) {
    passwordHash = hashPassword(password);
}

// Simple djb2-style hash — not for production, fine for a uni project
string User::hashPassword(const string& pwd) {
    size_t h = 5381;
    for (char c : pwd) h = ((h << 5) + h) + (unsigned char)c;
    ostringstream oss;
    oss << h;
    return oss.str();
}

// ---- Display ----

void User::displayProfile() const {
    cout << "  User ID  : " << userID << "\n";
    cout << "  Name     : " << name   << "\n";
    cout << "  Email    : " << email  << "\n";
    cout << "  Phone    : " << phone  << "\n";
    cout << "  Role     : " << role   << "\n";
}
