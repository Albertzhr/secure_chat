#ifndef SM3_UTILS_H
#define SM3_UTILS_H

#include <QString>

class SM3Utils {
public:
    static QString generateSalt();
    static QString hashPassword(const QString &password, const QString &salt);
};

#endif // SM3_UTILS_H

