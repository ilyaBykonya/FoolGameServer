#include "WaitingRoom.h"

WaitingRoom::WaitingRoom(QObject *parent):
    QObject{ parent },
    m_amountOfPlayers{ 0 },
    m_deckSize{ 0 },
    m_trancferableAbility{ false },
    m_minCash{ 50 },
    m_maxCash{ 5000 },
    m_pendingPlayersList(),
    m_roundStartTimer{ new QTimer(this) }

    {
        QObject::connect(m_roundStartTimer, &QTimer::timeout, this, &WaitingRoom::forceRoundStart);
        m_roundStartTimer->setInterval(10 * 1000);
    }

bool WaitingRoom::possibleNewPlayer(Player* player)
{
    if(m_pendingPlayersList.isEmpty())
    {
        player->queueSlotYouAreAddedToTheQueue();
        QObject::connect(player, &Player::applicationSignalUserExitFromWaitingRoom, this, &WaitingRoom::userExitFromRoom, Qt::ConnectionType::UniqueConnection);
        m_pendingPlayersList.push_back(player);
        m_amountOfPlayers = player->amountOfPlayers();
        m_deckSize = player->deckSize();
        m_trancferableAbility = player->trancferableAbility();
        m_minCash = player->minCash();
        m_maxCash = player->maxCash();

        return true;
    }
    else
    {
        if(!(m_amountOfPlayers & player->amountOfPlayers())) {
            return false;
        } else if(!(m_deckSize & player->deckSize())) {
            return false;
        } else if(m_trancferableAbility != player->trancferableAbility()) {
            return false;
        } else if(!((player->minCash() < this->m_maxCash) && (this->m_maxCash > player->minCash()))) {
            return false;
        } else {
            player->queueSlotYouAreAddedToTheQueue();
            QObject::connect(player, &Player::applicationSignalUserExitFromWaitingRoom, this, &WaitingRoom::userExitFromRoom, Qt::ConnectionType::UniqueConnection);
            m_pendingPlayersList.push_back(player);
            m_amountOfPlayers &= player->amountOfPlayers();
            m_deckSize &= player->deckSize();
            m_minCash = std::max(m_minCash, player->minCash());
            m_maxCash = std::min(m_maxCash, player->maxCash());
            checkFullRoom();
            return true;
        }
    }
}
void WaitingRoom::userExitFromRoom(Player* player)
{
    m_roundStartTimer->stop();
    auto it = std::find(m_pendingPlayersList.begin(), m_pendingPlayersList.end(), player);
    if(it != m_pendingPlayersList.end())
    {
        player->queueSlotYouAreRemovedFromTheQueue();
        QObject::disconnect(player, &Player::applicationSignalUserExitFromWaitingRoom, this, &WaitingRoom::userExitFromRoom);
        m_pendingPlayersList.erase(it);
        if(!m_pendingPlayersList.isEmpty())
        {
            m_minCash = m_pendingPlayersList.first()->minCash();
            m_maxCash = m_pendingPlayersList.first()->maxCash();
            m_deckSize = m_pendingPlayersList.first()->deckSize();
            m_amountOfPlayers = m_pendingPlayersList.first()->amountOfPlayers();
            m_trancferableAbility = m_pendingPlayersList.first()->trancferableAbility();
            for(auto it = m_pendingPlayersList.begin(); it != m_pendingPlayersList.end(); ++it)
            {
                m_amountOfPlayers &= player->amountOfPlayers();
                m_deckSize &= player->deckSize();
                m_minCash = std::max(m_minCash, player->minCash());
                m_maxCash = std::min(m_maxCash, player->maxCash());
            }
            checkFullRoom();
        }
        else
        {
            m_amountOfPlayers = 0;
            m_deckSize = 0;
            m_trancferableAbility = false;
        }
    }
    else
    {
        checkFullRoom();
    }
}
void WaitingRoom::checkFullRoom()
{
    bool emitSignal = false;
    if(m_pendingPlayersList.size() == 2)
    {
        if(m_amountOfPlayers & (~Player::CountPlayers::Players_2)) {
            m_roundStartTimer->start();
        } else {
            emitSignal = true;
            m_roundStartTimer->stop();
        }
    }
    else if(m_pendingPlayersList.size() == 3)
    {
        if(m_amountOfPlayers & (~Player::CountPlayers::Players_3)) {
            m_roundStartTimer->start();
        } else {
            emitSignal = true;
            m_roundStartTimer->stop();
        }
    }
    else if(m_pendingPlayersList.size() == 4)
    {
        if(m_amountOfPlayers & (~Player::CountPlayers::Players_4)) {
            m_roundStartTimer->start();
        } else {
            emitSignal = true;
            m_roundStartTimer->stop();
        }
    }
    else if(m_pendingPlayersList.size() == 5)
    {
        if(m_amountOfPlayers & (~Player::CountPlayers::Players_5)) {
            m_roundStartTimer->start();
        } else {
            emitSignal = true;
            m_roundStartTimer->stop();
        }
    }
    else if(m_pendingPlayersList.size() == 6)
    {
        emitSignal = true;
        m_roundStartTimer->stop();
    }




    if(emitSignal)
    {
        if(m_deckSize & Player::DeckType::Deck_24)
            m_deckSize = Player::DeckType::Deck_24;
        else if(m_deckSize & Player::DeckType::Deck_36)
            m_deckSize = Player::DeckType::Deck_36;
        else
            m_deckSize = Player::DeckType::Deck_52;

        emit this->roomIsFull(this);
    }
}

void WaitingRoom::forceRoundStart()
{
    if(m_deckSize & Player::DeckType::Deck_24)
        m_deckSize = Player::DeckType::Deck_24;
    else if(m_deckSize & Player::DeckType::Deck_36)
        m_deckSize = Player::DeckType::Deck_36;
    else
        m_deckSize = Player::DeckType::Deck_52;

    m_amountOfPlayers = ((0x01) << (m_pendingPlayersList.size() - 2));

    emit this->roomIsFull(this);
}

Cash WaitingRoom::minCash() const
{
    return m_minCash;
}
Cash WaitingRoom::maxCash() const
{
    return m_maxCash;
}
quint8 WaitingRoom::amountOfPlayers() const
{
    return m_amountOfPlayers;
}
quint8 WaitingRoom::deckSize() const
{
    return m_deckSize;
}
bool WaitingRoom::trancferableAbility() const
{
    return m_trancferableAbility;
}
QList<Player*> WaitingRoom::pendingPlayersList()
{
    return m_pendingPlayersList;
}
