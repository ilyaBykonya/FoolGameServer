#include "BattleArea.h"

BattleArea::BattleArea(Card::Suit trump, UserID attackerID, UserID defenderID, QObject *parent)
    :QObject{ parent },
     m_attackerID{ attackerID },
     m_defenderID{ defenderID },
     m_trump{ trump }
    {
    }

//![функции ходов игроков]
    //![отбивание карт]
    bool BattleArea::checkPossibilityBeat(UserID playerID, Card* card, quint16 pairID)
    {
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
            this->beatCard(pair, card);
            return true;
        } else if(firstCard->suit() != card->suit()) {
            return false;
        } else if(firstCard->dignity() < card->dignity()) {
            this->beatCard(pair, card);
            return true;
        } else {
            return false;
        }
    }
    void BattleArea::beatCard(BattlePair* pair, Card* card)
    {
        pair->beatCard(card);
    }
    //!
    //![перевод карт]
    bool BattleArea::checkPossibilityTrancferable(UserID playerID, Card* card)
    {
        if(playerID != m_defenderID)
            return false;

        if(m_pairsList.isEmpty())
            return false;
        if(m_pairsList.size() > 3)
            return false;


        for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
        {
            if((*it)->state() == BattlePair::PairState::Beaten)
                return false;

            if((*it)->firstCard())
                if((*it)->firstCard()->dignity() != card->dignity())
                    return false;
        }

        this->trancferableCard(card);
        return true;
    }
    void BattleArea::trancferableCard(Card* card)
    {
        BattlePair* newPair = new BattlePair(this);
        newPair->tossCard(card);
        m_pairsList.push_back(newPair);
    }
    //!
    //![подбрасывание карт]
    bool BattleArea::checkPossibilityToss(UserID playerID, Card* card)
    {
        if(m_pairsList.size() == 6)
            return false;

        if(playerID == m_defenderID)
        {
            return false;
        }
        else if(playerID == m_attackerID)
        {
            if(m_pairsList.isEmpty())
            {
                this->tossCard(card);
                return true;
            }

            for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
            {
                if((*it)->firstCard())
                    if((*it)->firstCard()->dignity() == card->dignity())
                    {
                        this->tossCard(card);
                        return true;
                    }
                if((*it)->secondCard())
                    if((*it)->secondCard()->dignity() == card->dignity())
                    {
                        this->tossCard(card);
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
                        this->tossCard(card);
                        return true;
                    }
                if((*it)->secondCard())
                    if((*it)->secondCard()->dignity() == card->dignity())
                    {
                        this->tossCard(card);
                        return true;
                    }
            }

            return false;
        }
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

        delete (*it);
        m_pairsList.erase(it);
    }
    return m_cardsList;
}

void BattleArea::setNewMainPair(UserID attackerID, UserID defenderID)
{
    m_attackerID = attackerID;
    m_defenderID = defenderID;
}
BattlePair* BattleArea::findPairOfID(PairID pairID)
{
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
        if((*it)->id() == pairID)
            return (*it);

    return nullptr;
}



quint8 BattleArea::amountOfPairs() const
{
    return m_pairsList.size();
}
quint8 BattleArea::amountOfBeatenPairs() const
{
    quint8 amountOfBeatenPairs = 0;
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
        if((*it)->state() == BattlePair::PairState::Beaten)
            ++amountOfBeatenPairs;

    return amountOfBeatenPairs;
}
quint8 BattleArea::amountOfNotBeatenPairs() const
{
    quint8 amountOfBeatenPairs = 0;
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
        if((*it)->state() == BattlePair::PairState::NotBeaten)
            ++amountOfBeatenPairs;

    return amountOfBeatenPairs;
}



bool BattleArea::checkAllPairsIsBeaten() const
{
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
    {
        if((*it)->state() != BattlePair::PairState::Beaten)
            return false;
    }
    return true;
}
bool BattleArea::checkAllPairsIsNotBeaten() const
{
    for(auto it = m_pairsList.begin(); it != m_pairsList.end(); ++it)
    {
        if((*it)->state() == BattlePair::PairState::Beaten)
            return false;
    }
    return true;
}

