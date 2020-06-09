#include "PlayerDeck.h"

PlayerDeck::PlayerDeck(qint16 id, QObject *parent)
    :QObject{ parent },
     m_ID{ id },
     m_playerState{ QueueMove::Other },
     m_actionButtonState{ false }
    {
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
qint16 PlayerDeck::id() const
{
    return m_ID;
}
quint8 PlayerDeck::requiredCards() const
{
    //TEST
    return 4;

    //потом поменяю на тернарный оператор
    quint8 amountRequiredCards = 6 - m_playerDeck.size();
    if(amountRequiredCards < 0)
        amountRequiredCards = 0;

    return  amountRequiredCards;
}
