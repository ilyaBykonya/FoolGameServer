#include "WaitingRoom.h"

WaitingRoom::WaitingRoom(QObject *parent):
    QObject{ parent },
    m_settings{ },
    m_pendingPlayersList(),
    m_roundStartTimer{ new QTimer(this) }
    {
        QObject::connect(m_roundStartTimer, &QTimer::timeout, this, &WaitingRoom::forceExitRoom);
        m_roundStartTimer->setInterval(10 * 1000);
    }
const SettingsStruct& WaitingRoom::waitingRoomSettings() const
{
    return this->m_settings;
}
QList<Player*> WaitingRoom::pendingPlayersList()
{
    return this->m_pendingPlayersList;
}

bool WaitingRoom::possibleNewPlayer(Player* player)
{
    if(m_pendingPlayersList.isEmpty())
    {
        player->queueSlotYouAreAddedToTheQueue();
        QObject::connect(player, &Player::applicationSignalUserExitFromWaitingRoom, this, &WaitingRoom::userExitFromRoom, Qt::ConnectionType::UniqueConnection);
        QObject::connect(player, &Player::signalUserDisconnected, this, &WaitingRoom::userDisconnected, Qt::ConnectionType::UniqueConnection);
        m_pendingPlayersList.push_back(player);
        this->m_settings = player->playerSettings();

        return true;
    }
    else
    {
        if(!(this->m_settings == player->playerSettings())) {
            return false;
        } else {
            player->queueSlotYouAreAddedToTheQueue();
            QObject::connect(player, &Player::applicationSignalUserExitFromWaitingRoom, this, &WaitingRoom::userExitFromRoom, Qt::ConnectionType::UniqueConnection);
            QObject::connect(player, &Player::signalUserDisconnected, this, &WaitingRoom::userDisconnected, Qt::ConnectionType::UniqueConnection);
            m_pendingPlayersList.push_back(player);
            this->m_settings &= player->playerSettings();
            checkFullRoom();
            return true;
        }
    }
}


void WaitingRoom::userExitFromRoom(Player* player)
{
    m_roundStartTimer->stop();
    auto it = std::find(m_pendingPlayersList.begin(), m_pendingPlayersList.end(), player);

    if(it == m_pendingPlayersList.end())
    {
        checkFullRoom();
        return;
    }
    player->queueSlotYouAreRemovedFromTheQueue();
    QObject::disconnect(player, &Player::applicationSignalUserExitFromWaitingRoom, this, &WaitingRoom::userExitFromRoom);
    QObject::disconnect(player, &Player::signalUserDisconnected, this, &WaitingRoom::userDisconnected);
    m_pendingPlayersList.erase(it);


    if(!m_pendingPlayersList.isEmpty())
    {
        this->m_settings = m_pendingPlayersList.first()->playerSettings();
        for(auto it = m_pendingPlayersList.begin(); it != m_pendingPlayersList.end(); ++it)
            this->m_settings &= (*it)->playerSettings();

        checkFullRoom();
    }
}
void WaitingRoom::userDisconnected()
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        this->userExitFromRoom(player);
}



void WaitingRoom::checkFullRoom()
{
    if(m_pendingPlayersList.size() < 2)
    {
        m_roundStartTimer->start();
        return;
    }

    quint16 res = (qint16(1) << (m_pendingPlayersList.size() - 2));
    qDebug() << QString("[%1 & %2] = %3").arg(res).arg(this->m_settings.m_countOfPlayers).arg(this->m_settings.m_countOfPlayers & res);
    if((this->m_settings.m_countOfPlayers & res))
    {
        m_roundStartTimer->stop();

        if(this->m_settings.m_deckType & DeckType::Deck_24)
            this->m_settings.m_deckType = DeckType::Deck_24;
        else if(this->m_settings.m_deckType & DeckType::Deck_36)
            this->m_settings.m_deckType = DeckType::Deck_36;
        else
            this->m_settings.m_deckType = DeckType::Deck_52;

        emit this->signalRoomIsFull(this);
    }
    else
    {
        m_roundStartTimer->start();
    }
}
void WaitingRoom::forceExitRoom()
{
    for(auto it = m_pendingPlayersList.begin(); it != m_pendingPlayersList.end(); ++it)
    {
        (*it)->queueSlotYouAreRemovedFromTheQueue();
        (*it)->serverSlotAlertMessage("Erorr", "превышено время ожидания игры");
        QObject::disconnect((*it), &Player::applicationSignalUserExitFromWaitingRoom, this, &WaitingRoom::userExitFromRoom);
    }
    m_pendingPlayersList.clear();
    m_roundStartTimer->stop();
}

