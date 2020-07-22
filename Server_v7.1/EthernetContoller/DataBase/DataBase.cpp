#include "DataBase.h"

DataBaseController::DataBaseController(QObject *parent)
    :QObject{ parent },
     m_dataBase(QSqlDatabase::addDatabase("QPSQL")),
     m_error("")
    {
        m_dataBase.setHostName("127.0.0.1");
        m_dataBase.setDatabaseName("FoolGameDatabase");
        if(!m_dataBase.open("postgres", "123456789"))
            emit this->noConnectToDataBase(m_dataBase.lastError().text());
    }

UserInformation DataBaseController::tryLogIn(const QString& playerName, const QString& playerPassword)
{
    QSqlQuery query;
    if(query.exec(QString("SELECT * FROM users\n"
                          "WHERE nickName = '%1' and password = '%2';").arg(playerName).arg(playerPassword)))
    {
        if(!query.next())
        {
            m_error = query.lastError().text();
            return UserInformation();
        }
        else
        {
            UserID id = query.value("id").toUInt();
            QString name = query.value("nickName").toString();
            Cash noDepositCash = query.value("noDepositMoney").toUInt();
            Cash depositCash = query.value("freeMoney").toUInt();
            quint32 countOfGames = query.value("countOfGames").toUInt();
            quint32 raitingPoints = query.value("raitingPoints").toUInt();

            return UserInformation(id, name, noDepositCash, depositCash, countOfGames, raitingPoints);
        }
    }
    else
    {
        m_error = query.lastError().text();
        return UserInformation();
    }
}
UserInformation DataBaseController::tryRegistrationPlayer(const QString& playerName, const QString& playerPassword)
{
    QSqlQuery query;
    if(query.exec(QString("INSERT INTO users(nickName, password)"
                          "VALUES('%1', '%2');").arg(playerName).arg(playerPassword)))
    {
        query.clear();
        if(query.exec(QString("SELECT * FROM users\n"
                              "WHERE nickName = '%1' and password = '%2';").arg(playerName).arg(playerPassword)))
        {
            if(!query.next())
            {
                m_error = query.lastError().text();
                return UserInformation();
            }
            else
            {
                UserID id = query.value("id").toUInt();
                QString name = query.value("nickName").toString();
                Cash noDepositCash = query.value("noDepositMoney").toUInt();
                Cash depositCash = query.value("freeMoney").toUInt();
                quint32 countOfGames = query.value("countOfGames").toUInt();
                quint32 raitingPoints = query.value("raitingPoints").toUInt();

                return UserInformation(id, name, noDepositCash, depositCash, countOfGames, raitingPoints);
            }
        }
        else
        {
            m_error = query.lastError().text();
            return UserInformation();
        }
    }
    else
    {
        m_error = query.lastError().text();
        return UserInformation();
    }
}

bool DataBaseController::saveUserInDataBase(Player* user)
{
    QSqlQuery query;
    QString queryText("UPDATE users\n"
                      "SET noDepositMoney = %1, freeMoney = %2, countOfGames = %3, raitingPoints = %4\n"
                      "WHERE id = %5;");


    Cash m_noDepositCash = user->userInfo().noDepositCash();
    Cash m_depositCash = user->userInfo().depositCash();
    quint32 m_countOfGames = user->userInfo().countOfGames();
    quint32 m_raitingPoints = user->userInfo().raitingPoints();
    UserID m_ID = user->id();
    if(query.exec(queryText.arg(m_noDepositCash).arg(m_depositCash).arg(m_countOfGames).arg(m_raitingPoints).arg(m_ID)))
    {
        return true;
    }
    else
    {
        m_error = query.lastError().text();
        return false;
    }
}

const QString& DataBaseController::errorText() const
{
    return m_error;
}
