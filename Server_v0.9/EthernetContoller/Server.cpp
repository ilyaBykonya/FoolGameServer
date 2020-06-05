#include "Server.h"

Server::Server(QWidget *parent)
    :QWidget{ parent },
     m_tcpServer{ new QTcpServer(this) },
     m_userID{ 0 }
    {
        if(!m_tcpServer->listen(QHostAddress::SpecialAddress::Any, 2323))
        {
            QMessageBox::critical(this, "Port error", QString("Port: ") + QString::number(2323) + "\n" + m_tcpServer->errorString());
            m_tcpServer->close();

            return;
        }
        QObject::connect(m_tcpServer, &QTcpServer::newConnection, this, &Server::slotNewConnection);

        m_registrationDataBase = new DataBase(this);

        m_textPlace = new QTextEdit();
        m_textPlace->setReadOnly(true);



        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(new QLabel("<center><sup><i>SERVER</i></sup></center>"));
        layout->addWidget(m_textPlace);
        this->setLayout(layout);
    }



void Server::slotNewConnection()
{
    Player* player = new Player(m_userID,  m_tcpServer->nextPendingConnection(), this);
    QObject::connect(player, &Player::applicationSignalUserWantPlay, this, &Server::slotPlayerWantPlay);
    QObject::connect(player, &Player::applicationSignalPlayerDepositMoney, this, &Server::slotPlayerDepositMoney);
    QObject::connect(player, &Player::applicationSignalPlayerWithdrawMoney, this, &Server::slotPlayerWithdrawMoney);

    QObject::connect(player, &Player::registrationSignalPlayerLogIn, this, &Server::slotPlayerLogIn);
    QObject::connect(player, &Player::registrationSignalPlayerRegistration, this, &Server::slotPlayerRegistration);

    this->m_textPlace->append(QString("New connection [%1]").arg(m_userID));
    ++m_userID;
}

void Server::slotPlayerWantPlay(Player* player, quint8 deckTypeFlags, quint8 playersCountFlags, bool realCash)
{
    auto it = std::find(m_waitingPlayers.begin(), m_waitingPlayers.end(), player);
    if(it != m_waitingPlayers.end())
        return;

    m_waitingPlayers.push_back(player);
    tryDoNewGameInstance();
}
void Server::tryDoNewGameInstance()
{
    if(m_waitingPlayers.size() > 1)
    {
        new GameInstance(m_waitingPlayers, this);
        m_waitingPlayers.clear();
    }
}


void Server::slotPlayerLogIn(const QString& name, const QString& password)
{
    this->m_textPlace->append(QString("Player log in [%1:%2]").arg(name).arg(password));
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
    {
        switch(m_registrationDataBase->tryLogIn(name, password))
        {
            case DataBase::LogInFlags::SuccesfullyLogIn:
            {
                player->registrationSlotSuccesfullyLogIn();
            }
            case DataBase::LogInFlags::WrongPassword:
            {
                player->registrationSlotUnsuccesfullyLogIn("Wrong password");
            }
            case DataBase::LogInFlags::UserNotRegistered:
            {
                player->registrationSlotUnsuccesfullyLogIn("No user with this name was found.""Check the input is correct.");
            }
            case DataBase::LogInFlags::OtherLogInError:
            {
                player->registrationSlotUnsuccesfullyLogIn("Undefined log in error");
            }
        }
    }
}
void Server::slotPlayerRegistration(const QString& name, const QString& password)
{
    this->m_textPlace->append(QString("Player log in [%1:%2]").arg(name).arg(password));
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
    {
        switch(m_registrationDataBase->tryRegistrationPlayer(name, password))
        {
            case DataBase::RegistrationFlags::SuccesfullyRegistration :
            {
                player->registrationSlotSuccesfullyRegistration();
            }
            case DataBase::RegistrationFlags::UsernameAlreadyTaken    :
            {
                player->registrationSlotUnsuccesfullyRegistration("Username taken already");
            }
            case DataBase::RegistrationFlags::OtherRegistrationError  :
            {
                player->registrationSlotUnsuccesfullyRegistration("Undefined registration error");
            }
        }
    }
}


void Server::slotPlayerDepositMoney(qint16 money)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        player->serverSlotAlertMessage(QString("You deposit money:"),
                                       (QString("Cash: %1").arg(money)));
}
void Server::slotPlayerWithdrawMoney(qint16 money)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        player->serverSlotAlertMessage(QString("You withdraw money:"),
                                       (QString("Cash: %1").arg(money)));
}

void Server::printText(const QString& str)
{
    this->m_textPlace->append(str);
}

