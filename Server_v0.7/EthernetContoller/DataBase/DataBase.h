#ifndef DATABASE_H
#define DATABASE_H
#include <QDir>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include "TypesAliases.h"

class DataBase : public QObject
{
    Q_OBJECT
public:
    enum RegistrationFlags
    {
        SuccesfullyRegistration = 0,
        UsernameAlreadyTaken,
        OtherRegistrationError
    };
    enum LogInFlags
    {
        SuccesfullyLogIn = 0,
        UserNotRegistered,
        WrongPassword,
        OtherLogInError
    };

    explicit DataBase(QObject *parent = nullptr);

    LogInFlags tryLogIn(const QString&, const QString&);
    RegistrationFlags tryRegistrationPlayer(const QString&, const QString&);

    void userDepositCash(Cash);
    void userWithdrawCash(Cash);
};

#endif // DATABASE_H
