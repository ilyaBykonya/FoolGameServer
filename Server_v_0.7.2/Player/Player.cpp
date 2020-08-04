#include "Player.h"

Player::Player(QWebSocket* socket, QObject *parent)
    :QObject{ parent },
     m_webSocket{ socket },
     m_userInfo{ nullptr },
     m_userState{ UserState::UserNoSignIn }
    {
        m_webSocket->setParent(this);
        QObject::connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &Player::slotReadClient);
        QObject::connect(m_webSocket, &QWebSocket::disconnected, this, &Player::signalUserDisconnected);
        QObject::connect(m_webSocket, &QWebSocket::disconnected, [this](){ QTimer::singleShot(200, this, &QObject::deleteLater); });
    }

Player::~Player()
{
    if(m_userInfo)
        emit this->saveUserInfoInDataBase(this);
}


QDataStream& operator<<(QDataStream& out, const Player& player)
{
    out << player.id() << player.m_userInfo->name() << player.m_userInfo->userAvatar();
    return out;
}





void Player::slotReadClient(const QByteArray& dataBlock)
{
    QDataStream inStream(dataBlock);
    inStream.setVersion(QDataStream::Version::Qt_5_14);

    EthernetSignals::ClientSignal::TypeSignal  typeSignal;
    inStream >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::TypeSignal::FromAppToServ             :
        {
            this->slotReadFromAppToServ(inStream);
            break;
        }
        case EthernetSignals::ClientSignal::TypeSignal::FromAppInstToServInst     :
        {
            this->slotReadFromAppInstToServInst(inStream);
            break;
        }
        case EthernetSignals::ClientSignal::TypeSignal::SignInSignals             :
        {
            this->slotReadSignIn(inStream);
            break;
        }
    }

}
void Player::slotReadSignIn(QDataStream& inStream)
{
    EthernetSignals::ClientSignal::Signals::SignIn typeSignal;
    inStream >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::Signals::SignIn::TryLogIn               :
        {
            QString userName;
            QString userPassword;
            inStream >> userName >> userPassword;
            if(m_userState == UserState::UserNoSignIn)
                emit this->registrationSignalPlayerLogIn(userName, userPassword);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::SignIn::TryRegistration         :
        {
            QString userName;
            QString userPassword;
            inStream >> userName >> userPassword;
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
void Player::slotReadFromAppToServ(QDataStream& inStream)
{
    EthernetSignals::ClientSignal::Signals::ApplicationSignal typeSignal;
    inStream >> typeSignal;
    switch (typeSignal)
    {
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerWantPlay            :
        {
            if(m_userState == UserState::UserInLobbi)
            {
                SettingsStruct settings;
                inStream >> settings.m_deckType
                         >> settings.m_countOfPlayers
                        >> settings.m_trancferableAbility
                         >> settings.m_rateIndex;

                m_settings = settings;
                emit this->applicationSignalUserWantPlay(this);
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
            inStream >> sum;
            if(m_userState == UserState::UserInLobbi)
                emit this->applicationSignalPlayerDepositMoney(sum);
            break;

        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::PlayerWithdrawMoney       :
        {
            Cash sum;
            inStream >> sum;
            if(m_userState == UserState::UserInLobbi)
                emit this->applicationSignalPlayerWithdrawMoney(sum);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::MessageSignal              :
        {
            Canal canal;
            QString message;
            inStream >> canal >> message;
            if(m_userState != UserState::UserNoSignIn)
                emit this->messageSignalUserSendMessage(this, canal, message);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::ApplicationSignal::UpdateUserAvatar           :
        {
            QPixmap newUserAvatar;
            inStream >> newUserAvatar;
            this->userInfo()->setUserAvatar(newUserAvatar);

            break;
        }
    }
}
void Player::slotReadFromAppInstToServInst(QDataStream& inStream)
{
    EthernetSignals::ClientSignal::Signals::TableSignal typeSignal;
    inStream >> typeSignal;
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
            inStream >> pairID >> cardSuit >> cardDignity;
            if(m_userState == UserState::UserInGame)
                emit this->instanceSignalPlayerTryBeat(pairID, cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryToss                :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            inStream >> cardSuit >> cardDignity;
            if(m_userState == UserState::UserInGame)
                emit this->instanceSignalPlayerTryToss(cardSuit, cardDignity);

            break;
        }
        case EthernetSignals::ClientSignal::Signals::TableSignal::TryTransferable        :
        {
            Card::Suit cardSuit;
            Card::Dignity cardDignity;
            inStream >> cardSuit >> cardDignity;
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
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::YouAreAddedToTheQueue;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::queueSlotYouAreRemovedFromTheQueue()
{
    if(m_userState != UserState::UserWaitGame)
        return;
    m_userState = UserState::UserInLobbi;


    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::YouAreRemovedFromTheQueue;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::serverSlotGetAllPlayInstanceOptions(Card::Suit trumpS, Card::Dignity trumpD, QList<Player*> players, UserID attacker, UserID defender, quint8 sizeOfDeck)
{
    if(m_userState != UserState::UserWaitGame)
        return;
    m_userState = UserState::UserInGame;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
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

    this->m_webSocket->sendBinaryMessage(dataBlock);
}





void Player::slotInstancePlayerTossedCard(UserID playerID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerTossedCard
              << playerID
              << cardSuit
              << cardDignity;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::slotInstancePlayerBeatCard(UserID playerID, PairID pairID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerBeatCard
              << playerID
              << pairID
              << cardSuit
              << cardDignity;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::slotInstancePlayerTransferable(UserID playerID, Card::Suit cardSuit, Card::Dignity cardDignity, UserID attackerID, UserID defederID)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerTrancferable
              << playerID
              << cardSuit
              << cardDignity
              << attackerID
              << defederID;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::slotInstanceUnsuccesfullyAction()
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::UnsuccesfullyAction;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}

void Player::slotInstanceTakeCardFromDeck(UserID playerID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst;
    if(playerID == this->id())
    {
        outStream << EthernetSignals::ServerSignal::Signals::InstanceSignal::ThisPlayerTakeCardFromDeck
                  << cardSuit << cardDignity;
    }
    else
    {
        outStream << EthernetSignals::ServerSignal::Signals::InstanceSignal::OtherPlayerTakeCardFromDeck
                  << playerID;
    }

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::slotInstancePlayerTakeAllCards(UserID playerID, UserID attackerID, UserID defenderID)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::PlayerTakeAllCards
              << playerID
              << attackerID
              << defenderID;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::slotInstanceMakeTurn(UserID attackerID, UserID defenderID)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);

    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::MakeTurn
              << attackerID
              << defenderID;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}

void Player::slotInstanceEndOfMatch(QList<Player*> m_winnersList)
{
    if(m_userState != UserState::UserInGame)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServInstToAppInst
              << EthernetSignals::ServerSignal::Signals::InstanceSignal::EndOfMatch;

    outStream << quint8(m_winnersList.size());
    for(auto it = m_winnersList.begin(); it != m_winnersList.end(); ++it)
    {
        outStream << (*(*it));
    }

    this->m_webSocket->sendBinaryMessage(dataBlock);
    this->m_userState = UserState::UserInLobbi;
    this->userInfo()->incrementCountOfGames();
}






void Player::serverSlotAlertMessage(const QString& title, const QString& message)
{
    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::AlertMessage
              << title
              << message;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}


void Player::registrationSlotSuccesfullySignIn(UserInformation* info)
{
    if(m_userState != UserState::UserNoSignIn)
        return;

    m_userInfo = info;
    m_userInfo->setParent(this);

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::SuccesfullySignIn
              << m_userInfo->id()
              << m_userInfo->name()
              << m_userInfo->dollars()
              << m_userInfo->freeTokens() + m_userInfo->noDepositTokens()
              << m_userInfo->countOfGames()
              << m_userInfo->raitingPoints()
              << m_userInfo->userAvatar();

    this->m_webSocket->sendBinaryMessage(dataBlock);
    this->m_userState = UserState::UserInLobbi;
}
void Player::registrationSlotUnsuccesfullySignIn(const QString& errorMessage)
{
    if(m_userState != UserState::UserNoSignIn)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::UnsuccesfullySignIn
              << errorMessage;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::registrationSlotLogOut()
{
    if(m_userState != UserState::UserInLobbi)
        return;

    m_userState = UserState::UserNoSignIn;
    m_userInfo->deleteLater();
    m_userInfo = nullptr;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::SignInSignals
              << EthernetSignals::ServerSignal::Signals::SignIn::LogOut;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}


void Player::messageSlotReceiveServerMessage(const QString& message)
{
    if(m_userState == UserState::UserNoSignIn)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::ServerMessageSignal
              << message;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
void Player::messageSlotReceiveUserMessage(Player* user, Canal chatCanal, const QString& message)
{
    if(m_userState == UserState::UserNoSignIn)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::UserMessageSignal
              << (*user)
              << chatCanal
              << message;

    this->m_webSocket->sendBinaryMessage(dataBlock);
}











void Player::cashSlotDepositDollars(Cash money)
{
    this->userInfo()->addDollars(money);
    this->slotUpdateUserInfo();
}
void Player::cashSlotWithdrawDollars(Cash money)
{
    this->userInfo()->subDollars(money);
    this->slotUpdateUserInfo();
}








void Player::slotUpdateUserInfo()
{
    if(m_userState == UserState::UserNoSignIn)
        return;

    QByteArray dataBlock;
    QDataStream outStream(&dataBlock, QIODevice::OpenModeFlag::WriteOnly);
    outStream.setVersion(QDataStream::Version::Qt_5_14);
    outStream << EthernetSignals::ServerSignal::TypeSignal::FromServToApp
              << EthernetSignals::ServerSignal::Signals::ServerSignal::UpdateUserInformation
              << m_userInfo->name()
              << m_userInfo->dollars()
              << m_userInfo->noDepositTokens() + m_userInfo->freeTokens()
              << m_userInfo->countOfGames()
              << m_userInfo->raitingPoints();

    this->m_webSocket->sendBinaryMessage(dataBlock);
}
