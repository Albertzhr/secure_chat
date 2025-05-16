#include "sm3_utils.h"
#include <QCryptographicHash>
#include <QRandomGenerator>

QString SM3Utils::generateSalt() {
    QByteArray salt;
    for (int i = 0; i < 8; ++i)
        salt.append(QRandomGenerator::global()->generate() % 256);
    return salt.toHex();
}

QString SM3Utils::hashPassword(const QString &password, const QString &salt) {
    QByteArray input = (salt + password).toUtf8();
    QByteArray hash = QCryptographicHash::hash(input, QCryptographicHash::Sha3_256);
    return hash.toHex();
}

