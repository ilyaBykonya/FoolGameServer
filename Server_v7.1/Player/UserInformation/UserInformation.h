#ifndef USERINFORMATION_H
#define USERINFORMATION_H
#include "TypesAliases.h"

class UserInformation
{
private:
    UserID m_ID;
    QString m_name;
    Cash m_noDepositCash;
    Cash m_depositCash;
    quint32 m_countOfGames;
    quint32 m_raitingPoints;

public:
    UserInformation();
    UserInformation(UserID, const QString&, Cash, Cash, quint32, quint32);

    UserID id() const;
    const QString& name() const;

    Cash noDepositCash() const;
    void addNoDepositCash(Cash);
    void subNoDepositCash(Cash);

    Cash depositCash() const;
    void addDepositCash(Cash);
    void subDepositCash(Cash);

    quint32 countOfGames() const;
    void incrementCountOfGames();

    quint32 raitingPoints() const;
    void addRaitingPoints(quint32);
    void subRaitingPoints(quint32);

    bool isNull() const;
    void reset();
};

#endif // USERINFORMATION_H
