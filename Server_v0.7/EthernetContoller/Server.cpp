#include "Server.h"

Server::Server(QWidget *parent)
    :QWidget{ parent },
     m_tcpServer{ new QTcpServer(this) },
     m_userID{ 1 },
     m_sharedChat{ new ChatRoom(Canal::SharedCanal, this) }
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
        m_inputPlace = new QLineEdit;
        m_inputPlace->setPlaceholderText("Enter emssage");
        QObject::connect(m_inputPlace, &QLineEdit::returnPressed, [&]()
        {
            emit this->sendServerMessage(m_inputPlace->text());
            m_textPlace->append(QString("SERVER: %1").arg(m_inputPlace->text()));
            m_inputPlace->clear();
        });


        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(new QLabel("<center><sup><i>SERVER</i></sup></center>"));
        layout->addWidget(m_textPlace);
        layout->addWidget(m_inputPlace);
        this->setLayout(layout);
    }



void Server::slotNewConnection()
{
    Player* player = new Player(m_tcpServer->nextPendingConnection(), this);

    QObject::connect(player, &Player::registrationSignalPlayerLogIn, this, &Server::slotPlayerLogIn, Qt::ConnectionType::UniqueConnection);
    QObject::connect(player, &Player::registrationSignalPlayerRegistration, this, &Server::slotPlayerRegistration, Qt::ConnectionType::UniqueConnection);

    this->m_textPlace->append(QString("New connection [%1]").arg(m_userID));
}

void Server::slotPlayerWantPlay(Player* player, SettingsStruct /*settings*/)
{
    if(player->userState() != Player::UserState::UserInLobbi)
        return;

    for(auto it = m_waitingPlayers.begin(); it != m_waitingPlayers.end(); ++it)
        if((*it)->possibleNewPlayer(player))
            return;

    WaitingRoom* newRoom = new WaitingRoom(this);
    QObject::connect(newRoom, &WaitingRoom::roomIsFull, this, &Server::doNewGameInstance);
    newRoom->possibleNewPlayer(player);
    m_waitingPlayers.push_back(newRoom);
}
void Server::doNewGameInstance(WaitingRoom* room)
{
    auto it = std::find(m_waitingPlayers.begin(), m_waitingPlayers.end(), room);
    if(it == m_waitingPlayers.end())
        return;

    Croupier::DeckSize croupierDeckSize;
    switch(room->deckSize())
    {
        case Player::DeckType::Deck_24:
        {
            croupierDeckSize = Croupier::DeckSize::DeckSize_24;
            break;
        }
        case Player::DeckType::Deck_36:
        {
            croupierDeckSize = Croupier::DeckSize::DeckSize_36;
            break;
        }
        default:
        {
            croupierDeckSize = Croupier::DeckSize::DeckSize_52;
            break;
        }
    }

    new GameInstance(croupierDeckSize, room->trancferableAbility(), room->pendingPlayersList(), this);
    m_waitingPlayers.erase(it);
    room->deleteLater();
}


void Server::slotPlayerLogIn(const QString& name, const QString& password)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
    {
        switch(m_registrationDataBase->tryLogIn(name, password))
        {
            case DataBase::LogInFlags::SuccesfullyLogIn:
            {
                QObject::connect(player, &Player::applicationSignalUserWantPlay, this, &Server::slotPlayerWantPlay, Qt::ConnectionType::UniqueConnection);
                QObject::connect(player, &Player::applicationSignalPlayerDepositMoney, this, &Server::slotPlayerDepositMoney, Qt::ConnectionType::UniqueConnection);
                QObject::connect(player, &Player::applicationSignalPlayerWithdrawMoney, this, &Server::slotPlayerWithdrawMoney, Qt::ConnectionType::UniqueConnection);
                QObject::connect(player, &Player::registrationSignalPlayerLogOut, this, &Server::slotLogOut, Qt::ConnectionType::UniqueConnection);
                QObject::connect(this, &Server::sendServerMessage, player, &Player::messageSlotReceiveServerMessage, Qt::ConnectionType::UniqueConnection);

                QObject::disconnect(player, &Player::registrationSignalPlayerLogIn, this, &Server::slotPlayerLogIn);
                QObject::disconnect(player, &Player::registrationSignalPlayerRegistration, this, &Server::slotPlayerRegistration);

                m_sharedChat->pushUser(player);
                player->registrationSlotSuccesfullySignIn(name, m_userID);
                ++m_userID;
                break;
            }
            case DataBase::LogInFlags::WrongPassword:
            {
                player->registrationSlotUnsuccesfullySignIn("Wrong password");
                break;
            }
            case DataBase::LogInFlags::UserNotRegistered:
            {
                player->registrationSlotUnsuccesfullySignIn("No user with this name was found.""Check the input is correct.");
                break;
            }
            case DataBase::LogInFlags::OtherLogInError:
            {
                player->registrationSlotUnsuccesfullySignIn("Undefined log in error");
                break;
            }
        }
    }
}
void Server::slotPlayerRegistration(const QString& name, const QString& password)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
    {
        switch(m_registrationDataBase->tryRegistrationPlayer(name, password))
        {
            case DataBase::RegistrationFlags::SuccesfullyRegistration :
            {
                QObject::connect(player, &Player::applicationSignalUserWantPlay, this, &Server::slotPlayerWantPlay, Qt::ConnectionType::UniqueConnection);
                QObject::connect(player, &Player::applicationSignalPlayerDepositMoney, this, &Server::slotPlayerDepositMoney, Qt::ConnectionType::UniqueConnection);
                QObject::connect(player, &Player::applicationSignalPlayerWithdrawMoney, this, &Server::slotPlayerWithdrawMoney, Qt::ConnectionType::UniqueConnection);
                QObject::connect(player, &Player::registrationSignalPlayerLogOut, this, &Server::slotLogOut, Qt::ConnectionType::UniqueConnection);
                QObject::connect(this, &Server::sendServerMessage, player, &Player::messageSlotReceiveServerMessage, Qt::ConnectionType::UniqueConnection);

                QObject::disconnect(player, &Player::registrationSignalPlayerLogIn, this, &Server::slotPlayerLogIn);
                QObject::disconnect(player, &Player::registrationSignalPlayerRegistration, this, &Server::slotPlayerRegistration);

                m_sharedChat->pushUser(player);
                player->registrationSlotSuccesfullySignIn(name, m_userID);
                ++m_userID;

                break;
            }
            case DataBase::RegistrationFlags::UsernameAlreadyTaken    :
            {
                player->registrationSlotUnsuccesfullySignIn("Username taken already");
                break;
            }
            case DataBase::RegistrationFlags::OtherRegistrationError  :
            {
                player->registrationSlotUnsuccesfullySignIn("Undefined registration error");
                break;
            }
        }
    }
}
void Server::slotLogOut(Player* player)
{
    if(player->userState() != Player::UserState::UserInLobbi)
        return;

    m_sharedChat->popUser(player);
    player->registrationSlotLogOut();
    QObject::disconnect(player, &Player::applicationSignalUserWantPlay, this, &Server::slotPlayerWantPlay);
    QObject::disconnect(player, &Player::applicationSignalPlayerDepositMoney, this, &Server::slotPlayerDepositMoney);
    QObject::disconnect(player, &Player::applicationSignalPlayerWithdrawMoney, this, &Server::slotPlayerWithdrawMoney);
    QObject::disconnect(player, &Player::registrationSignalPlayerLogOut, this, &Server::slotLogOut);
    QObject::disconnect(this, &Server::sendServerMessage, player, &Player::messageSlotReceiveServerMessage);

    QObject::connect(player, &Player::registrationSignalPlayerLogIn, this, &Server::slotPlayerLogIn, Qt::ConnectionType::UniqueConnection);
    QObject::connect(player, &Player::registrationSignalPlayerRegistration, this, &Server::slotPlayerRegistration, Qt::ConnectionType::UniqueConnection);
}

void Server::slotPlayerDepositMoney(Cash money)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        player->serverSlotAlertMessage(QString("You deposit money:"),
                                       (QString("Cash: %1%2").arg('+').arg(money)));

    m_registrationDataBase->userDepositCash(money);
}
void Server::slotPlayerWithdrawMoney(Cash money)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        player->serverSlotAlertMessage(QString("You withdraw money:"),
                                       (QString("Cash: %1%2").arg('-').arg(money)));

    m_registrationDataBase->userWithdrawCash(money);
}

