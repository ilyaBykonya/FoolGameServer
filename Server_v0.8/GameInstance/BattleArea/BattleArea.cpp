#include "BattleArea.h"

BattleArea::BattleArea(Card::Suit trump, qint16 attackerID, qint16 defenderID, QObject *parent)
    :QObject{ parent },
     m_attackerID{ attackerID },
     m_defenderID{ defenderID },
     m_trump{ trump }
    {
    }

//![функции ходов игроков]
    //![отбивание карт]
    bool BattleArea::checkPossibilityBeat(qint16 playerID, Card* card, quint16 pairID)
    {
        if(playerID != m_defenderID)
            return false;
        BattlePair* pair = findPairOfID(pairID);
        if(pair != nullptr)
            if(pair->state() == BattlePair::PairState::NotBeaten)
            {
                beatCard(pair, card);
                return true;
            }

        return false;

        /**************************************************************
        *Релизная версия проверок
        *
        if(playerID != m_defenderID)
            return false;

        BattlePair* pair = findPairOfID(pairID);
        if(pair == nullptr)
            return false;

        if(pair->state() != BattlePair::PairState::NotBeaten)
            return false;

        Card* firstCard = pair->firstCard();
        if(firstCard == nullptr)
            return false;

        if((firstCard->suit() != m_trump) && (card->suit() == m_trump)) {
            return true;
        } else if(firstCard->suit() != card->suit()) {
            return false;
        } else if(firstCard->dignity() < card->dignity()) {
            return true;
        } else {
            return false;
        }
        */

    }
    void BattleArea::beatCard(BattlePair* pair, Card* card)
    {
        pair->beatCard(card);
    }
    //!
    //![перевод карт]
    bool BattleArea::checkPossibilityTrancferable(qint16 playerID, Card* card)
    {
        if(playerID != m_defenderID)
            return false;

        trancferableCard(card);
        return true;


        /**************************************************************
        *Релизная версия проверок
        *
        if(playerID != m_defenderID)
            return false;

        if(m_pairsList.isEmpty())
            return false;
        if(m_pairsList.size() > 3)
            return false;

        Card::Dignity dignity;
        if(m_pairsList.first()->firstCard())
            dignity = m_pairsList.first()->firstCard()->dignity();

        if(card->dignity() != dignity)
            return false;

        for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
        {
            if((*it)->state() == BattlePair::PairState::Beaten)
                return false;

            if((*it)->firstCard())
                if((*it)->firstCard()->dignity() != dignity)
                    return false;
        }

        return true;
        */

    }
    void BattleArea::trancferableCard(Card* card)
    {
        BattlePair* newPair = new BattlePair(this);
        newPair->tossCard(card);
        m_pairsList.push_back(newPair);
    }
    //!
    //![подбрасывание карт]
    bool BattleArea::checkPossibilityToss(qint16 playerID, Card* card)
    {
        if(m_pairsList.size() == 6)
            return false;

        if(playerID == m_attackerID) {
            tossCard(card);
            return true;
        } else if(!m_pairsList.isEmpty()) {
            tossCard(card);
            return true;
        }

        return false;

        /**************************************************************
        *Релизная версия проверок
        *
        if(playerID == m_defenderID)
        {
            return false;
        }
        else if(playerID == m_attackerID)
        {
            if(m_pairsList.isEmpty())
            {
                return true;
            }

            for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
            {
                if((*it)->firstCard())
                    if((*it)->firstCard()->dignity() == card->dignity())
                    {
                        return true;
                    }
                if((*it)->secondCard())
                    if((*it)->secondCard()->dignity() == card->dignity())
                    {
                        return true;
                    }
            }

            return false;
        }
        else
        {
            if(m_pairsList.isEmpty())
                return false;

            for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
            {
                if((*it)->firstCard())
                    if((*it)->firstCard()->dignity() == card->dignity())
                    {
                        return true;
                    }
                if((*it)->secondCard())
                    if((*it)->secondCard()->dignity() == card->dignity())
                    {
                        return true;
                    }
            }

            return false;
        }
        */

    }
    void BattleArea::tossCard(Card* card)
{
    BattlePair* newPair = new BattlePair(this);
    newPair->tossCard(card);
    m_pairsList.push_back(newPair);
}
    //!
//!




QList<Card*> BattleArea::endOfStep()
{
    QList<Card*> m_cardsList;
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
    {
        if((*it)->firstCard())
            m_cardsList.push_back((*it)->firstCard());

        if((*it)->secondCard())
            m_cardsList.push_back((*it)->secondCard());
    }
    return m_cardsList;
}

void BattleArea::setNewMainPair(qint16 attackerID, qint16 defenderID)
{
    m_attackerID = attackerID;
    m_defenderID = defenderID;
}
BattlePair* BattleArea::findPairOfID(quint16 pairID)
{
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
        if((*it)->id() == pairID)
            return (*it);

    return nullptr;
}
bool BattleArea::allPairsIsBeaten() const
{
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
    {
        if((*it)->state() != BattlePair::PairState::Beaten)
            return false;
    }
    return true;
}
quint8 BattleArea::amountOfPairs() const
{
    return m_pairsList.size();
}
