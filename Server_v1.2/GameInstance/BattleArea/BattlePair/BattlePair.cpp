#include "BattlePair.h"

BattlePair::BattlePair(QObject *parent)
    :QObject{ parent },
     m_firstCard{ nullptr },
     m_secondCard{ nullptr },
     m_state{ PairState::Empty },
     m_pairID{ 0 }
    {
    }

void BattlePair::tossCard(Card* card)
{
    if(card == nullptr)
        return;
    
    m_firstCard = card;
    m_state = PairState::NotBeaten;
    m_pairID = (((card->suit() + 1) * 100) + card->dignity());
}
void BattlePair::beatCard(Card* card)
{
    if(card == nullptr)
        return;
    
    m_secondCard = card;
    m_state = PairState::Beaten;
}

Card* BattlePair::firstCard()
{
    return m_firstCard;
}
Card* BattlePair::secondCard()
{
    return m_secondCard;
}

BattlePair::PairState BattlePair::state() const
{
    return m_state;
}
quint16 BattlePair::id() const
{
    return m_pairID;
}
