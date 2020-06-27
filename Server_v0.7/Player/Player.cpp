#include "Player.h"

Player::Player(QTcpSocket* socket, QObject *parent)
    :QObject{ parent },
     m_nextDataBlockSize{ 0 },
     m_socket{ socket },
     m_ID{ 0 },
     m_userName(),
     m_userState{ UserState::UserNoSignIn }
    {
        QObject::connect(m_socket, &QTcpSocket::readyRead, this, &Player::slotReadClient);

        QObject::connect(m_socket, &QTcpSocket::disconnected, m_socket, &QObject::deleteLater);
        QObject::connect(m_socket, &QTcpSocket::disconnected, this, &QObject::deleteLater);
    }

QDataStream& operator<<(QDataStream& out, const Player& player)
{
    out << player.m_ID << player.m_userName;
    return out;
}





void Player::slotReadClient()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(this->sender());
    QDataStream dataBlock(socket);
    dataBlock.setVersion(QDataStream::Version::Qt_5_14);
    for (;;)
    {
        if(!m_nextDataBlockSize)
        {
            if(socket->bytesAvailable() < sizeof(qint16))
                break;

            dataBlock >> m_nextDataBlockSize;
        }
        if(socket->bytesAvailable() < m_nextDataBlockSize)
            break;

        EthernetSignals::ClientSignal::TypeSignal  typeSignal;
        dataBlock >> typeSignal;
        switch (typeSignal)
        {
            case EthernetSignals::ClientSignal::TypeSignal::FromAppToServ             :
            {
                this->slotReadFromAppToServ(dataBlock);
                break;
            }
            case EthernetSignals::ClientSignal::TypeSignal::FromAppInstToServInst     :
            {
                this->slotReadFromAppInstToServInst(dataBlock);
                break;
            }
            case EthernetSignals::ClientSignal::TypeSignal::SignInSignals             :
            {
                this->slotReadSignIn(dataBlock);
                break;
            }
        }

        m_nextDataBlockSize = 0;
    }
}
void Player::slotReadSignIn(QDataStream& dataBlock)
{
    EthernetSignals::ClientSignal::Signals::SignIn typeSignal;
    dataBlock >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::Signals::SignIn::TryLogIn               :
        {
            QString userName;
            QString userPassword;
            dataBlock >> userName >> userPassword;
            if(m_userState == UserState::UserNoSignIn)
                emit this->registrationSignalPlayerLogIn(userName, userPassword);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::SignIn::TryRegistration         :
        {
            QString userName;
            QString userPassword;
            dataBlock >> userName >> userPassword;
            if(m_userState == UserState::UserNoSignIn)
                emit this->registrationSignalPlayerRegistration(userName, userPassword);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::SignIn::LogOut                  :
        {
            if(m_userState == UserState::UserInLobbi)
                emit this->registrationSignalPlayerLogOut(this);

            break;
        }
    }
}
void Player::slotReadFromAppToServ(QDataStream& dataBlock)
{
    EthernetSignals::ClientSignal::Signals::ApplicationSignal typeSignal;
    dataBlock >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerWantPlay            :
        {
            SettingsStruct settings;
            dataBlock >> settings.m_deckType >> settings.m_countOfPlayers >> settings.m_trancferableAbility;
            if(m_userState == UserState::UserInLobbi)
            {
                this->m_settings.m_deckType = settings.m_deckType;
                this->m_settings.m_countOfPlayers = settings.m_countOfPlayers;
                this->m_settings.m_trancferableAbility = settings.m_trancferableAbility;
                //ну и дальше будет запуск подбора матчей, а пока так:
                emit this->applicationSignalUserWantPlay(this, settings);
            }
            break;
        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerExitFromWaitingRoom :
        {
            if(m_userState == UserState::UserWaitGame)
            {
                emit this->applicationSignalUserExitFromWaitingRoom(this);
            }
            break;
        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerDepositMoney        :
        {
            Cash sum;
            dataBlock >> sum;
            if(m_userState == UserState::UserInLobbi)
                emit this->applicationSignalPlayerDepositMoney(sum);
            break;

        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerWithdrawMoney       :
        {
            Cash sum;
            dataBlock >> sum;
            if(m_userState == UserState::UserInLobbi)
                emit this->applicationSignalPlayerWithdrawMoney(sum);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::MessageSignal       :
        {
            Canal canal;
            QString message;
            dataBlock >> canal >> message;
            if(m_userState != UserState::UserNoSignIn)
                emit this->messageSignalUserSendMessage(this, canal, message);

            break;
        }
    }
}
void Player::slotReadFromAppInstToServInst(QDataStream& dataBlock)
{
    EthernetSignals::ClientSignal::Signals::TableSignal typeSignal;
    dataBlock >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::Signals::TableSignal::ActionButtonWasClicked :
        {
            if(m_userState == UserState::UserInGame)
                emit this->instanceSignalPlayerActionButtonWasClicked();
            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryBeat                :
        {
            PairID pairID;
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> pairID >> cardSuit >> cardDignity;
            if(m_userState == UserState::UserInGame)
                emit this->instanceSignalPlayerTryBeat(pairID, cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryToss                :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> cardSuit >> cardDignity;
            if(m_userState == UserState::UserInGame)
                emit this->instanceSignalPlayerTryToss(cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryTransferable        :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> cardSuit >> cardDignity;
            if(m_userState == UserState::UserInGame)
                emit this->instanceSignalPlayerTryTransferable(cardSuit, cardDignity);

            break;
        }
    }
}




void Player::queueSlotYouAreAddedToTheQueue()
{
    if(m_userState != UserState::UserInLobbi)
        return;
    m_userState = UserState::UserWaitGame;


    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::YouAreAddedToTheQueue;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    this->m_socket->write(dataBlock);
}
void Player::queueSlotYouAreRemovedFromTheQueue()
{
    if(m_userState != UserState::UserWaitGame)
        return;
    m_userState = UserState::UserInLobbi;


    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::YouAreRemovedFromTheQueue;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    this->m_socket->write(dataBlock);
}
void Player::serverSlotGetAllPlayInstanceOptions(Card::Suit trumpS, Card::Dignity trumpD, QList<Player*> players, UserID attacker, UserID defender, quint8 sizeOfDeck)
{
    if(m_userState != UserState::UserWaitGame)
        return;
    m_userState = UserState::UserInGame;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::GetAllPlayInstanceOptions
              << trumpS
              << trumpD
              << quint8(players.size());
    for(Player* p: players)
    {
        outStream << (*p);
    }
    outStream << attacker
              << defender
              << sizeOfDeck;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}


void Player::slotInstancePlayerTossedCard(UserID playerID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerTossedCard
              << playerID
              << cardSuit
              << cardDignity;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    this->m_socket->write(dataBlock);
}
void Player::slotInstancePlayerBeatCard(UserID playerID, PairID pairID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerBeatCard
              << playerID
              << pairID
              << cardSuit
              << cardDignity;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::slotInstancePlayerTransferable(UserID playerID, Card::Suit cardSuit, Card::Dignity cardDignity, UserID attackerID, UserID defederID)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerTrancferable
              << playerID
              << cardSuit
              << cardDignity
              << attackerID
              << defederID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::slotInstanceUnsuccesfullyAction()
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::UnsuccesfullyAction;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}

void Player::slotInstanceTakeCardFromDeck(UserID playerID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst;
    if(playerID == m_ID)
    {
        outStream << EthernetSignals::ServerSignal::Signals::InstanceSignal::ThisPlayerTakeCardFromDeck
                  << cardSuit << cardDignity;
    }
    else
    {
        outStream << EthernetSignals::ServerSignal::Signals::InstanceSignal::OtherPlayerTakeCardFromDeck
                  << playerID;
    }
    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));

    m_socket->write(dataBlock);
}
void Player::slotInstancePlayerTakeAllCards(UserID playerID, UserID attackerID, UserID defenderID)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerTakeAllCards
              << playerID
              << attackerID
              << defenderID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::slotInstanceMakeTurn(UserID attackerID, UserID defenderID)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::MakeTurn
              << attackerID
              << defenderID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}

void Player::slotInstanceEndOfMatch(UserID winnerID)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::EndOfMatch
              << winnerID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
    m_userState = UserState::UserInLobbi;
}






void Player::serverSlotAlertMessage(const QString& title, const QString& message)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::AlertMessage
              << title
              << message;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}


void Player::registrationSlotSuccesfullySignIn(const QString& userName, UserID userID)
{
    if(m_userState != UserState::UserNoSignIn)
        return;

    m_userName = userName;
    m_ID = userID;
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::SuccesfullySignIn
              << userID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
    m_userState = UserState::UserInLobbi;
}
void Player::registrationSlotUnsuccesfullySignIn(const QString& errorMessage)
{
    if(m_userState != UserState::UserNoSignIn)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::UnsuccesfullySignIn
              << errorMessage;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::registrationSlotLogOut()
{
    if(m_userState != UserState::UserInLobbi)
        return;

    m_userState = UserState::UserNoSignIn;
    m_ID = 0;
    m_userName.clear();


    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::LogOut;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}


void Player::messageSlotReceiveServerMessage(const QString& message)
{
    if(m_userState == UserState::UserNoSignIn)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::ServerMessageSignal
              << message;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::messageSlotReceiveUserMessage(Player* user, Canal chatCanal, const QString& message)
{
    if(m_userState == UserState::UserNoSignIn)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::UserMessageSignal
              << (*user)
              << chatCanal
              << message;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
