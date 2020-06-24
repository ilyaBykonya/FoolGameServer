#include "Player.h"

Player::Player(UserID id, QTcpSocket* socket, QObject *parent)
    :QObject{ parent },
     m_nextDataBlockSize{ 0 },
     m_socket{ socket },
     m_ID{ id },
     m_userName(),
     m_inFindGame{ false }
    {
        QObject::connect(m_socket, &QTcpSocket::readyRead, this, &Player::slotReadClient);

        QObject::connect(m_socket, &QTcpSocket::disconnected, m_socket, &QObject::deleteLater);
        QObject::connect(m_socket, &QTcpSocket::disconnected, this, &QObject::deleteLater);

        serverSlotSendID();
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
        case EthernetSignals::ClientSignal::Signals::SignIn::TryLogIn            :
        {
            QString userName;
            QString userPassword;
            dataBlock >> userName >> userPassword;

            emit this->registrationSignalPlayerLogIn(userName, userPassword);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::SignIn::TryRegistration         :
        {
            QString userName;
            QString userPassword;
            dataBlock >> userName >> userPassword;
            emit this->registrationSignalPlayerRegistration(userName, userPassword);

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
            if(!m_inFindGame)
            {
                this->m_settings.m_deckType = settings.m_deckType;
                this->m_settings.m_countOfPlayers = settings.m_countOfPlayers;
                this->m_settings.m_trancferableAbility = settings.m_trancferableAbility;
                //ну и дальше будет запуск подбора матчей, а пока так:
                emit this->applicationSignalUserWantPlay(this, settings);
                m_inFindGame = true;
            }
            break;
        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerDepositMoney        :
        {
            Cash sum;
            dataBlock >> sum;
            emit this->applicationSignalPlayerDepositMoney(sum);
            break;

        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerWithdrawMoney       :
        {
            Cash sum;
            dataBlock >> sum;
            emit this->applicationSignalPlayerWithdrawMoney(sum);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::MessageSignal       :
        {
            Canal canal;
            QString message;
            dataBlock >> canal >> message;
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
            emit this->instanceSignalPlayerActionButtonWasClicked();
            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryBeat                :
        {
            PairID pairID;
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> pairID >> cardSuit >> cardDignity;
            emit this->instanceSignalPlayerTryBeat(pairID, cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryToss                :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> cardSuit >> cardDignity;
            emit this->instanceSignalPlayerTryToss(cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryTransferable        :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> cardSuit >> cardDignity;
            emit this->instanceSignalPlayerTryTransferable(cardSuit, cardDignity);

            break;
        }
    }
}





void Player::slotInstancePlayerTossedCard(UserID playerID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
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
    m_inFindGame = false;
}





void Player::serverSlotSendID()
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::SetPlayerID
              << m_ID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    this->m_socket->write(dataBlock);
}
void Player::serverSlotGetAllPlayInstanceOptions(Card::Suit trumpS, Card::Dignity trumpD, QList<Player*> players, UserID attacker, UserID defender, quint8 sizeOfDeck)
{
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




void Player::registrationSlotSuccesfullyLogIn(const QString& userName)
{
    m_userName = userName;
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::SuccesfullyLogIn;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::registrationSlotUnsuccesfullyLogIn(const QString& errorMessage)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::UnsuccesfullyLogIn
              << errorMessage;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::registrationSlotSuccesfullyRegistration(const QString& userName)
{
    m_userName = userName;
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::SuccesfullyRegistration;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::registrationSlotUnsuccesfullyRegistration(const QString& errorMessage)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::UnsuccesfullyRegistration
              << errorMessage;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}




void Player::messageSlotReceiveServerMessage(const QString& message)
{
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
