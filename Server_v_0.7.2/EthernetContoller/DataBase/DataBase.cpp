#include "DataBase.h"

DataBaseController::DataBaseController(QObject *parent)
    :QObject{ parent },
     m_dataBase(QSqlDatabase::addDatabase("QPSQL")),
     m_errorString()
    {
        m_dataBase.setHostName("127.0.0.1");
        m_dataBase.setDatabaseName("FoolGameDatabase");
        if(!m_dataBase.open("postgres", "123456789"))
            emit this->noConnectToDataBase(m_dataBase.lastError().text());
    }

UserInformation* DataBaseController::tryLogIn(const QString& playerName, const QString& playerPassword)
{
    QSqlQuery query;
    if(query.exec(QString("SELECT * FROM users\n"
                          "WHERE nickName = '%1';").arg(playerName)))
    {
        if(!query.next())
        {
            m_errorString = "User with this name does not exist.";
            return nullptr;
        }
        else
        {
            if(query.value("password") != playerPassword)
            {
                m_errorString = "Wrong password.";
                return nullptr;
            }

            UserID id = query.value("id").toUInt();
            QString name = query.value("nickName").toString();
            Cash dollars = query.value("dollars").toUInt();
            Cash noDepositTokens = query.value("noDepositTokens").toUInt();
            Cash freeTokens = query.value("freeTokens").toUInt();
            quint32 countOfGames = query.value("countOfGames").toUInt();
            quint32 raitingPoints = query.value("raitingPoints").toUInt();
            QPixmap userAvatar = QPixmap(QString("D:/FoolGame/FoolGameServer/resources/users_avatars/%1.png").arg(id));
            if(userAvatar.isNull())
                userAvatar.load("D:/FoolGame/FoolGameServer/resources/template_user_avatar/user_standart.png");

            return new UserInformation(id, name, dollars, noDepositTokens, freeTokens, countOfGames, raitingPoints, userAvatar);
        }
    }
    else
    {
        m_errorString = query.lastError().text();
        return nullptr;
    }
}
UserInformation* DataBaseController::tryRegistrationPlayer(const QString& playerName, const QString& playerPassword)
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
                m_errorString = query.lastError().text();
                return nullptr;
            }
            else
            {
                UserID id = query.value("id").toUInt();
                QString name = query.value("nickName").toString();
                Cash dollars = query.value("dollars").toUInt();
                Cash noDepositTokens = query.value("noDepositTokens").toUInt();
                Cash freeTokens = query.value("freeTokens").toUInt();
                quint32 countOfGames = query.value("countOfGames").toUInt();
                quint32 raitingPoints = query.value("raitingPoints").toUInt();
                QPixmap userAvatar = QPixmap("D:/FoolGame/FoolGameServer/resources/template_user_avatar/user_standart.png");

                userAvatar.save(QString("D:/FoolGame/FoolGameServer/resources/users_avatars/%1.png").arg(id), "PNG");

                return new UserInformation(id, name, dollars, noDepositTokens, freeTokens, countOfGames, raitingPoints, userAvatar);
            }
        }
        else
        {
            m_errorString = query.lastError().text();
            return nullptr;
        }
    }
    else
    {
        m_errorString = QString("This nickname is already taken [%1]").arg(query.lastError().type());
        return nullptr;
    }
}

bool DataBaseController::saveUserInDataBase(Player* user)
{
    QSqlQuery query;
    QString queryText("UPDATE users\n"
                      "SET dollars = %1, noDepositTokens = %2, freeTokens = %3, countOfGames = %4, raitingPoints = %5\n"
                      "WHERE id = %6;");


    Cash dollars = user->userInfo()->dollars();
    Cash noDepositTokens = user->userInfo()->noDepositTokens();
    Cash freeTokens = user->userInfo()->freeTokens();
    quint32 countOfGames = user->userInfo()->countOfGames();
    quint32 raitingPoints = user->userInfo()->raitingPoints();
    UserID ID = user->id();
    if(query.exec(queryText.arg(dollars).arg(noDepositTokens).arg(freeTokens).arg(countOfGames).arg(raitingPoints).arg(ID)))
    {
        user->userInfo()->userAvatar().save(QString("D:/FoolGame/FoolGameServer/resources/users_avatars/%1.png").arg(user->userInfo()->id()), "PNG");
        return true;
    }
    else
    {
        m_errorString = query.lastError().text();
        return false;
    }
}
const QString& DataBaseController::errorText() const
{
    return m_errorString;
}
