#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>

class AuthManager : public QObject {
    Q_OBJECT

public:
    explicit AuthManager(QObject *parent = nullptr);
    ~AuthManager();

    bool registerUser(const QString &username, const QString &password);
    bool login(const QString &username, const QString &password);

private:
    QSqlDatabase db;
    void initDB();
};

#endif // AUTHMANAGER_H

