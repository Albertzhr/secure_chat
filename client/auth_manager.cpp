#include "auth_manager.h"
#include "sm3_utils.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

AuthManager::AuthManager(QObject *parent) : QObject(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("auth/users.db");
    db.open();
    initDB();
}

AuthManager::~AuthManager() {
    db.close();
}

void AuthManager::initDB() {
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "username TEXT PRIMARY KEY,"
               "salt TEXT,"
               "hashed TEXT)");
}

bool AuthManager::registerUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) return false;

    // Check if exists
    QSqlQuery check;
    check.prepare("SELECT username FROM users WHERE username = ?");
    check.addBindValue(username);
    if (check.exec() && check.next()) return false;

    QString salt = SM3Utils::generateSalt();
    QString hashed = SM3Utils::hashPassword(password, salt);

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, salt, hashed) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(salt);
    query.addBindValue(hashed);

    return query.exec();
}

bool AuthManager::login(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT salt, hashed FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        QString salt = query.value(0).toString();
        QString hashedStored = query.value(1).toString();
        QString hashedInput = SM3Utils::hashPassword(password, salt);
        return hashedStored == hashedInput;
    }
    return false;
}

