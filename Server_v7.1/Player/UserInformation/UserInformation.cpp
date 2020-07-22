#include "UserInformation.h"

UserInformation::UserInformation()
{
    reset();
}
UserInformation::UserInformation(UserID id, const QString& name, Cash noDepositCash, Cash depositCash, quint32 countOfGames, quint32 raitingPoints)
    :m_ID{ id },
     m_name{ name },
     m_noDepositCash{ noDepositCash },
     m_depositCash{ depositCash },
     m_countOfGames{ countOfGames },
     m_raitingPoints{ raitingPoints }
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


Cash UserInformation::noDepositCash() const
{
    return m_noDepositCash;
}
void UserInformation::addNoDepositCash(Cash cash)
{
    m_noDepositCash += cash;
}
void UserInformation::subNoDepositCash(Cash cash)
{
    if(cash <= m_noDepositCash)
        m_noDepositCash -= cash;
    else
        m_noDepositCash = 0;
}


Cash UserInformation::depositCash() const
{
    return m_depositCash;
}
void UserInformation::addDepositCash(Cash cash)
{
    m_depositCash += cash;
}
void UserInformation::subDepositCash(Cash cash)
{
    if(cash <= m_depositCash)
        m_depositCash -= cash;
    else
        m_depositCash = 0;
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


bool UserInformation::isNull() const
{
    return ((m_ID            == 0)               &&
            (m_name.isEmpty() | m_name.isNull()) &&
            (m_noDepositCash == 0)               &&
            (m_depositCash   == 0)               &&
            (m_countOfGames  == 0)               &&
            (m_raitingPoints == 0));
}
void UserInformation::reset()
{
    m_ID = 0;
    m_name.clear();
    m_noDepositCash = 0;
    m_depositCash = 0;
    m_countOfGames = 0;
    m_raitingPoints = 0;
}
