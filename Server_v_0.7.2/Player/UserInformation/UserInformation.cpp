#include "UserInformation.h"


UserInformation::UserInformation(UserID id, const QString& name, Cash dollars, Cash noDepositCash, Cash depositCash, quint32 countOfGames, quint32 raitingPoints, const QPixmap& avatar, QObject* parent)
    :QObject{ parent },
     m_ID{ id },
     m_name{ name },
     m_dollars{ dollars },
     m_noDepositTokens{ noDepositCash },
     m_freeTokens{ depositCash },
     m_countOfGames{ countOfGames },
     m_raitingPoints{ raitingPoints },
     m_userAvatar{ avatar }
    {
    }

UserID UserInformation::id() const
{
    return m_ID;
}
const QString& UserInformation::name() const
{
    return m_name;
}

Cash UserInformation::dollars() const
{
    return m_dollars;
}
void UserInformation::addDollars(Cash cash)
{
    m_dollars += cash;
}
void UserInformation::subDollars(Cash cash)
{
    if(cash <= m_dollars)
        m_dollars -= cash;
    else
        m_dollars = 0;
}


Cash UserInformation::noDepositTokens() const
{
    return m_noDepositTokens;
}
void UserInformation::addNoDepositTokens(Cash cash)
{
    m_noDepositTokens += cash;
}
void UserInformation::subNoDepositTokens(Cash cash)
{
    if(cash <= m_noDepositTokens)
        m_noDepositTokens -= cash;
    else
        m_noDepositTokens = 0;
}


Cash UserInformation::freeTokens() const
{
    return m_freeTokens;
}
void UserInformation::addFreeTokens(Cash cash)
{
    m_freeTokens += cash;
}
void UserInformation::subFreeTokens(Cash cash)
{
    if(cash <= m_freeTokens)
        m_freeTokens -= cash;
    else
        m_freeTokens = 0;
}


quint32 UserInformation::countOfGames() const
{
    return m_countOfGames;
}
void UserInformation::incrementCountOfGames()
{
    ++m_countOfGames;
}


quint32 UserInformation::raitingPoints() const
{
    return m_raitingPoints;
}
void UserInformation::addRaitingPoints(quint32 raitingPoints)
{
    m_raitingPoints += raitingPoints;
}
void UserInformation::subRaitingPoints(quint32 raitingPoints)
{
    if(raitingPoints <= m_raitingPoints)
        m_raitingPoints -= raitingPoints;
    else
        m_raitingPoints = 0;
}


const QPixmap& UserInformation::userAvatar() const
{
    return m_userAvatar;
}
void UserInformation::setUserAvatar(const QPixmap& newAvatar)
{
    m_userAvatar = newAvatar;
}

