#include "Player.h"

Player::Player(qint16 id, QTcpSocket* socket, QObject *parent)
    :QObject{ parent },
     m_nextDataBlockSize{ 0 },
     m_ID{ id },
     m_socket{ socket }
    {
        QObject::connect(m_socket, &QTcpSocket::readyRead, this, &Player::slotReadClient);

        QObject::connect(m_socket, &QTcpSocket::disconnected, m_socket, &QObject::deleteLater);
        QObject::connect(m_socket, &QTcpSocket::disconnected, this, &QObject::deleteLater);

        serverSlotSendID();
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
    EthernetSignals::ClientSignal::SignIn typeSignal;
    dataBlock >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::SignIn::TryLogIn            :
        {
            QString userName;
            QString userPassword;
            dataBlock >> userName >> userPassword;

            emit this->registrationSignalPlayerLogIn(userName, userPassword);

            break;
        }
        case EthernetSignals::ClientSignal::SignIn::TryRegistration         :
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
    qDebug() << "   MyPlayer::slotReadFromAppToServ";
    EthernetSignals::ClientSignal::ApplicationSignal typeSignal;
    dataBlock >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::ApplicationSignal::PlayerWantPlay            :
        {
            quint8 deckTypeFlags;
            quint8 playersCountFlags;
            bool realCash;
            dataBlock >> deckTypeFlags >> playersCountFlags >> realCash;
            //ну и дальше будет запуск подбора матчей, а пока так:
            qDebug() << QString("READ PLAYER WANT PLAY [%1:%2:%3]").arg(deckTypeFlags).arg(playersCountFlags).arg(realCash);
            emit this->applicationSignalUserWantPlay(this, deckTypeFlags, playersCountFlags, realCash);

            break;
        }
        case EthernetSignals::ClientSignal::ApplicationSignal::PlayerDepositMoney        :
        {
            qint16 sum;
            dataBlock >> sum;
            qDebug() << QString("READ PLAYER DEPOSIT [%1]").arg(sum);
            emit this->applicationSignalPlayerDepositMoney(sum);
            break;

        }
        case EthernetSignals::ClientSignal::ApplicationSignal::PlayerWithdrawMoney       :
        {
            qint16 sum;
            dataBlock >> sum;
            qDebug() << QString("READ PLAYER WITHDRAW [%1]").arg(sum);
            emit this->applicationSignalPlayerWithdrawMoney(sum);

            break;
        }
    }
}
void Player::slotReadFromAppInstToServInst(QDataStream& dataBlock)
{
    EthernetSignals::ClientSignal::TableSignal typeSignal;
    dataBlock >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::TableSignal::ActionButtonWasClicked :
        {
            qDebug() << QString("READ PLAYER CLICKED ACTION BUTTON");
            emit this->instanceSignalPlayerActionButtonWasClicked(this);
            break;
        }
        case EthernetSignals::ClientSignal::TableSignal::TryBeat                :
        {
            qint16 pairID;
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> pairID >> cardSuit >> cardDignity;
            qDebug() << QString("READ PLAYER BEAT [%1]->[%2:%3]").arg(pairID).arg(cardSuit).arg(cardDignity);
            emit this->instanceSignalPlayerTryBeat(this, pairID, cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::TableSignal::TryToss                :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> cardSuit >> cardDignity;
            qDebug() << QString("READ PLAYER TOSS [%1:%2]").arg(cardSuit).arg(cardDignity);
            emit this->instanceSignalPlayerTryToss(this, cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::TableSignal::TryTransferable        :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            dataBlock >> cardSuit >> cardDignity;
            qDebug() << QString("READ PLAYER TRANCFERABLE [%1:%2]").arg(cardSuit).arg(cardDignity);
            emit this->instanceSignalTryTransferable(this, cardSuit, cardDignity);

            break;
        }
    }
}



void Player::slotInstancePlayerTossedCard(qint16 playerID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::PlayerTossedCard
              << playerID
              << cardSuit
              << cardDignity;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT PLAYER TOSS [%1]->[%2:%3]").arg(playerID).arg(cardSuit).arg(cardDignity);
    this->m_socket->write(dataBlock);
}
void Player::slotInstancePlayerBeatCard(qint16 playerID, qint16 pairID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::PlayerBeatCard
              << playerID
              << pairID
              << cardSuit
              << cardDignity;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT PLAYER BEAT [%1]->[%2]->[%3:%4]").arg(playerID).arg(pairID).arg(cardSuit).arg(cardDignity);
    m_socket->write(dataBlock);
}
void Player::slotInstancePlayerTransferable(qint16 playerID, Card::Suit cardSuit, Card::Dignity cardDignity, qint16 attackerID, qint16 defederID)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::PlayerTrancferable
              << playerID
              << cardSuit
              << cardDignity
              << attackerID
              << defederID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT PLAYER TRANCFERABLE [%1]->[%2]->[%3:%4]").arg(playerID).arg(cardSuit).arg(cardDignity);
    m_socket->write(dataBlock);
}

void Player::slotInstanceTakeCardFromDeck(qint16 playerID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst;
    if(playerID == m_ID)
    {
        outStream << EthernetSignals::ServerSignal::InstanceSignal::ThisPlayerTakeCardFromDeck
                  << cardSuit << cardDignity;
        qDebug() << QString("EMIT THIS PLAYER TAKE [%3:%4]").arg(cardSuit).arg(cardDignity);
    }
    else
    {
        outStream << EthernetSignals::ServerSignal::InstanceSignal::OtherPlayerTakeCardFromDeck
                  << playerID;
        qDebug() << QString("EMIT OTHER PLAYER TAKE [%3:%4]").arg(playerID);
    }
    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));

    m_socket->write(dataBlock);
}

void Player::slotInstancePlayerTakeAllCards(qint16 playerID, qint16 attackerID, qint16 defenderID)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::PlayerTakeAllCards
              << playerID
              << attackerID
              << defenderID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT PLAYER TAKE ALL CARDS [%1]").arg(playerID);
    m_socket->write(dataBlock);
}
void Player::slotInstanceMakeTurn(qint16 attackerID, qint16 defenderID)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::MakeTurn
              << attackerID
              << defenderID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT MAKE TURN");
    m_socket->write(dataBlock);
}

void Player::slotInstanceEndOfMatch(qint16 wonMoney)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::EndOfMatch
              << wonMoney;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT END OF MATCH [%1]").arg(wonMoney);
    m_socket->write(dataBlock);
}





void Player::slotInstanceUnsuccesfullyToss(Card::Suit cardSuit, Card::Dignity cardDignity, qint16 pairID)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::UnsuccesfullyAction
              << PlayerActionType::Toss
              << cardSuit
              << cardDignity
              << pairID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::slotInstanceUnsuccesfullyBeat(Card::Suit cardSuit, Card::Dignity cardDignity, qint16 pairID)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::UnsuccesfullyAction
              << PlayerActionType::Beat
              << cardSuit
              << cardDignity
              << pairID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT BEAT UNSUCCESFULLY");
    m_socket->write(dataBlock);
}
void Player::slotInstanceUnsuccesfullyTrancferable(Card::Suit cardSuit, Card::Dignity cardDignity, qint16 pairID)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::InstanceSignal::UnsuccesfullyAction
              << PlayerActionType::Trancferable
              << cardSuit
              << cardDignity
              << pairID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT TRANCFERABLE UNSUCCESFULLY");
    m_socket->write(dataBlock);
}





void Player::serverSlotSendID()
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::ServerSignal::SetPlayerID
              << m_ID;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT THIS_PLAYER_ID [%1]").arg(m_ID);
    this->m_socket->write(dataBlock);
}
void Player::serverSlotGetAllPlayInstanceOptions(Card::Suit trumpS, Card::Dignity trumpD, QList<Player*> players, qint16 attacker, qint16 defender, qint16 sizeOfDeck)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::ServerSignal::GetAllPlayInstanceOptions
              << trumpS
              << trumpD
              << qint16(players.size());
    for(Player* p: players)
    {
        outStream << p->id();
    }
    outStream << attacker
              << defender
              << sizeOfDeck;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT GAME_TABLE_OPTIONS [%1:%2:%3:%4:%5]").arg(trumpS).arg(trumpD).arg(attacker).arg(defender).arg(sizeOfDeck);
    m_socket->write(dataBlock);
}
void Player::serverSlotAlertMessage(const QString& title, const QString& message)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::ServerSignal::AlertMessage
              << title
              << message;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    qDebug() << QString("EMIT ALERT MESSAGE [%1::%2]").arg(title).arg(message);
    m_socket->write(dataBlock);
}



void Player::registrationSlotSuccesfullyLogIn()
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::SignIn::SuccesfullyLogIn;

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
              << EthernetSignals::ServerSignal::SignIn::UnsuccesfullyLogIn
              << errorMessage;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
void Player::registrationSlotSuccesfullyRegistration()
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << qint16(0)
              << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::SignIn::SuccesfullyRegistration;

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
              << EthernetSignals::ServerSignal::SignIn::UnsuccesfullyRegistration
              << errorMessage;

    outStream.device()->seek(0);
    outStream << qint16(dataBlock.size() - sizeof(qint16));
    m_socket->write(dataBlock);
}
