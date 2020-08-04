#ifndef USERINFORMATION_H
#define USERINFORMATION_H
#include "TypesAliases.h"
#include <QPixmap>
class UserInformation: public QObject
{
private:
    UserID m_ID;
    QString m_name;
    Cash m_dollars;
    Cash m_noDepositTokens;
    Cash m_freeTokens;
    quint32 m_countOfGames;
    quint32 m_raitingPoints;
    QPixmap m_userAvatar;

public:
    UserInformation(UserID, const QString&, Cash, Cash, Cash, quint32, quint32, const QPixmap&, QObject* = nullptr);

    UserID id() const;
    const QString& name() const;


    Cash dollars() const;
    void addDollars(Cash);
    void subDollars(Cash);

    Cash noDepositTokens() const;
    void addNoDepositTokens(Cash);
    void subNoDepositTokens(Cash);

    Cash freeTokens() const;
    void addFreeTokens(Cash);
    void subFreeTokens(Cash);

    quint32 countOfGames() const;
    void incrementCountOfGames();

    quint32 raitingPoints() const;
    void addRaitingPoints(quint32);
    void subRaitingPoints(quint32);

    const QPixmap& userAvatar() const;
    void setUserAvatar(const QPixmap&);
};

#endif // USERINFORMATION_H
