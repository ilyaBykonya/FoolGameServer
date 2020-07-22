#include "PlayerDeck.h"

PlayerDeck::PlayerDeck(Player* player, QObject *parent)
    :QObject{ parent },
     m_controlThisDeckPlayer{ player },
     m_playerState{ QueueMove::Other },
     m_actionButtonState{ false }
    {
        QObject::connect(player, &Player::instanceSignalPlayerTryBeat, this, &PlayerDeck::slotThisPlayerTryBeat, Qt::ConnectionType::UniqueConnection);
        QObject::connect(player, &Player::instanceSignalPlayerTryToss, this, &PlayerDeck::slotThisPlayerTryToss, Qt::ConnectionType::UniqueConnection);
        QObject::connect(player, &Player::instanceSignalPlayerTryTransferable, this, &PlayerDeck::slotThisPlayerTryTransferable, Qt::ConnectionType::UniqueConnection);
        QObject::connect(player, &Player::instanceSignalPlayerActionButtonWasClicked, this, &PlayerDeck::slotThisPlayerClickedActionButton, Qt::ConnectionType::UniqueConnection);
        QObject::connect(this, &PlayerDeck::signalUnsuccesfullyAction, player, &Player::slotInstanceUnsuccesfullyAction);


        QObject::connect(this, &PlayerDeck::instanceSignalPlayerTossedCard, player, &Player::slotInstancePlayerTossedCard, Qt::ConnectionType::UniqueConnection);
        QObject::connect(this, &PlayerDeck::instanceSignalPlayerBeatCard, player, &Player::slotInstancePlayerBeatCard, Qt::ConnectionType::UniqueConnection);
        QObject::connect(this, &PlayerDeck::instanceSignalPlayerTrancferableCard, player, &Player::slotInstancePlayerTransferable, Qt::ConnectionType::UniqueConnection);

        QObject::connect(this, &PlayerDeck::instanceSignalTakeCardFromDeck, player, &Player::slotInstanceTakeCardFromDeck, Qt::ConnectionType::UniqueConnection);
        QObject::connect(this, &PlayerDeck::instanceSignalPlayerTakeAllCards, player, &Player::slotInstancePlayerTakeAllCards, Qt::ConnectionType::UniqueConnection);
        QObject::connect(this, &PlayerDeck::instanceSignalMakeTurn, player, &Player::slotInstanceMakeTurn, Qt::ConnectionType::UniqueConnection);

        QObject::connect(this, &PlayerDeck::signalGetAllPlayInstanceOptions, player, &Player::serverSlotGetAllPlayInstanceOptions, Qt::ConnectionType::UniqueConnection);
        QObject::connect(this, &PlayerDeck::instanceSignalEndOfMatch, player, &Player::slotInstanceEndOfMatch, Qt::ConnectionType::UniqueConnection);
    }

void PlayerDeck::putCard(Card* card)
{
    if(card == nullptr)
        return;

    m_playerDeck.push_back(card);
}
Card* PlayerDeck::takeCard(Card::Suit cardSuit, Card::Dignity cardDignity)
{
    for(auto it = m_playerDeck.begin(); it != m_playerDeck.end(); ++it)
    {
        if(((*it)->suit() == cardSuit) && ((*it)->dignity() == cardDignity))
        {
            Card* buf = (*it);
            m_playerDeck.erase(it);
            return buf;
        }
    }

    return nullptr;
}

QueueMove PlayerDeck::playerState() const
{
    return m_playerState;
}
void PlayerDeck::setPlayerState(QueueMove newState)
{
    m_playerState = newState;
}

bool PlayerDeck::actionButtonState() const
{
    return m_actionButtonState;
}
void PlayerDeck::setActionButtonState(bool newState)
{
    m_actionButtonState = newState;
}


quint8 PlayerDeck::playerDeckSize() const
{
    return m_playerDeck.size();
}
UserID PlayerDeck::id() const
{
    return m_controlThisDeckPlayer->id();
}

Player* PlayerDeck::player()
{
    return m_controlThisDeckPlayer;
}


void PlayerDeck::slotThisPlayerTryBeat(PairID pairID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    emit this->signalThisPlayerTryBeat(this, pairID, cardSuit, cardDignity);
}
void PlayerDeck::slotThisPlayerTryToss(Card::Suit cardSuit, Card::Dignity cardDignity)
{
    emit this->signalThisPlayerTryToss(this, cardSuit, cardDignity);
}
void PlayerDeck::slotThisPlayerTryTransferable(Card::Suit cardSuit, Card::Dignity cardDignity)
{
    emit this->signalThisPlayerTryTransferable(this, cardSuit, cardDignity);
}
void PlayerDeck::slotThisPlayerClickedActionButton()
{
    emit this->signalThisPlayerClickedActionButton(this);
}
