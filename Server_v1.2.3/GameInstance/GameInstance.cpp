#include "GameInstance.h"

GameInstance::GameInstance(Croupier::DeckSize deckSize, bool trancferableAbility, QList<Player*> playersList, QObject *parent)
    :QObject{ parent }
    {

        m_chatRoom = new ChatRoom(Canal::GameCanal, this);
        m_croupier = new Croupier(deckSize, trancferableAbility);
        m_startDeck = new StartDeck(m_croupier->deckSizeAmount(), this);

        //=========================================================

        quint8 index = 0;
        for(auto it = playersList.begin(); it != playersList.end(); ++it, ++index)
        {
            m_chatRoom->pushUser(*it);


            PlayerDeck* deck = new PlayerDeck(*it);
            if(index == 0) {
                m_attackerID = deck->id();
                deck->setPlayerState(QueueMove::Attacker);
            } else if (index == 1) {
                m_defenderID = deck->id();
                deck->setPlayerState(QueueMove::Defender);
            } else {
                deck->setPlayerState(QueueMove::Other);
            }

            m_playerDecks.push_back(deck);

            //=========================================================
            //Сигналы [стол] --> [инстанс]
            QObject::connect(deck, &PlayerDeck::signalThisPlayerTryToss, this, &GameInstance::instanceSlotPlayerTryToss, Qt::ConnectionType::UniqueConnection);
            QObject::connect(deck, &PlayerDeck::signalThisPlayerTryBeat, this, &GameInstance::instanceSlotPlayerTryBeat, Qt::ConnectionType::UniqueConnection);
            QObject::connect(deck, &PlayerDeck::signalThisPlayerTryTransferable, this, &GameInstance::instanceSlotPlayerTryTransferable, Qt::ConnectionType::UniqueConnection);
            QObject::connect(deck, &PlayerDeck::signalThisPlayerClickedActionButton, this, &GameInstance::instanceSlotPlayerWasClickedActionButton, Qt::ConnectionType::UniqueConnection);

            //=========================================================
            //Сигналы [инстанс] --> [стол]
                //![сигналы рантайма игры]
                    //![сигналы ходов]
                    QObject::connect(this, &GameInstance::instanceSignalPlayerBeatCard, deck, &PlayerDeck::instanceSignalPlayerBeatCard, Qt::ConnectionType::UniqueConnection);
                    QObject::connect(this, &GameInstance::instanceSignalPlayerTossedCard, deck, &PlayerDeck::instanceSignalPlayerTossedCard, Qt::ConnectionType::UniqueConnection);
                    QObject::connect(this, &GameInstance::instanceSignalPlayerTrancferableCard, deck, &PlayerDeck::instanceSignalPlayerTrancferableCard, Qt::ConnectionType::UniqueConnection);
                    //!
                    //![сигнал конца шага]
                    QObject::connect(this, &GameInstance::instanceSignalMakeTurn, deck, &PlayerDeck::instanceSignalMakeTurn, Qt::ConnectionType::UniqueConnection);
                    //!
                    //![сигналы взятия карт]
                    QObject::connect(this, &GameInstance::instanceSignalTakeCardFromDeck, deck, &PlayerDeck::instanceSignalTakeCardFromDeck, Qt::ConnectionType::UniqueConnection);
                    QObject::connect(this, &GameInstance::instanceSignalPlayerTakeAllCards, deck, &PlayerDeck::instanceSignalPlayerTakeAllCards, Qt::ConnectionType::UniqueConnection);
                    //!
                //!
                //!======================
                //![сигналы начала/конца]
                    //![сигнал начала игры]
                    QObject::connect(this, &GameInstance::signalGetAllPlayInstanceOptions, deck, &PlayerDeck::signalGetAllPlayInstanceOptions, Qt::ConnectionType::UniqueConnection);
                    //!
                    //![сигнал конца игры]
                    QObject::connect(this, &GameInstance::instanceSignalEndOfMatch, deck, &PlayerDeck::instanceSignalEndOfMatch, Qt::ConnectionType::UniqueConnection);
                    //!
                //!
            //!
            //=========================================================
        }

        //=========================================================

        m_battleArea = new BattleArea(m_startDeck->firstCard()->suit(), m_attackerID, m_defenderID, this);

        //=========================================================

        m_handUp = new HandUp(this);

        //=========================================================


        //!в конце матча инстанс удаляется
        QObject::connect(this, &GameInstance::instanceSignalEndOfMatch, this, &QObject::deleteLater);

        //============================
        //!Начинаем матч
        emit this->signalGetAllPlayInstanceOptions(m_startDeck->firstCard()->suit(),
                                                   m_startDeck->firstCard()->dignity(),
                                                   playersList,
                                                   m_attackerID,
                                                   m_defenderID,
                                                   m_croupier->deckSizeAmount());

        QTimer::singleShot(150, this, &GameInstance::drawCards);
    }

PlayerDeck* GameInstance::findPlayerDeckOfID(UserID playerID)
{
    for(auto it = m_playerDecks.begin(); it != m_playerDecks.end(); ++it)
    {
        if((*it)->id() == playerID)
            return (*it);
    }

    return nullptr;
}
void GameInstance::setNewMainPair()
{
    m_attackerID = m_defenderID;
    PlayerDeck *defenderDeck = findPlayerDeckOfID(m_defenderID);
    if(defenderDeck == m_playerDecks.last())
    {
        m_defenderID = m_playerDecks.first()->id();
    }
    else
    {
        m_defenderID = m_playerDecks.at(m_playerDecks.indexOf(defenderDeck) + 1)->id();
    }
    m_battleArea->setNewMainPair(m_attackerID, m_defenderID);
}



void GameInstance::resetPlayersActionButton()
{
    for(auto it = m_playerDecks.begin(); it != m_playerDecks.end(); ++it)
        (*it)->setActionButtonState(false);
}
PlayerDeck* GameInstance::findFollowingBeatingPlayer()
{
    quint16 defenderIndex = 0;
    for(auto it = m_playerDecks.begin(); it != m_playerDecks.end(); ++it, ++defenderIndex)
        if((*it)->id() == m_defenderID)
            break;

    if(defenderIndex == (m_playerDecks.size() - 1))
        return m_playerDecks.first();
    else
        return m_playerDecks.at(defenderIndex + 1);
}



void GameInstance::makeTurn()
{
    resetPlayersActionButton();

    QList<Card*> list = m_battleArea->endOfStep();
    for(auto it = list.begin(); it != list.end(); ++it)
    {
        m_handUp->putCard(*it);
    }


    checkOutEliminatedPlayers();
    setNewMainPair();
    emit this->instanceSignalMakeTurn(m_attackerID, m_defenderID);


    if(this->checkEndOfMatch())
        emit this->instanceSignalEndOfMatch(m_attackerID);

    drawCards();
}
void GameInstance::playerTakeCardsFromTable(UserID playerID)
{
    resetPlayersActionButton();


    PlayerDeck* deck = findPlayerDeckOfID(playerID);
    if(deck)
    {
        QList<Card*> cardsList = m_battleArea->endOfStep();
        for(auto it = cardsList.begin(); it != cardsList.end(); ++it)
        {
            deck->putCard((*it));
        }
    }


    checkOutEliminatedPlayers();

    setNewMainPair();
    setNewMainPair();
    emit this->instanceSignalPlayerTakeAllCards(playerID, m_attackerID, m_defenderID);


    if(this->checkEndOfMatch())
        emit this->instanceSignalEndOfMatch(m_attackerID);

    drawCards();
}



void GameInstance::drawCards()
{
    for(quint8 i = 0; i < 6; ++i)
    {
        for(auto it = m_playerDecks.begin(); it != m_playerDecks.end(); ++it)
        {
            if((*it)->playerDeckSize() > 5)
                continue;

            Card* card = m_startDeck->takeCard();
            if(card == nullptr)
                return;



            (*it)->putCard(card);
            emit instanceSignalTakeCardFromDeck((*it)->id(), card->suit(), card->dignity());
        }
    }
}
bool GameInstance::checkEndOfMatch()
{
    if(m_startDeck->deckSize() > 0)
        return false;


    if(m_playerDecks.size() == 1)
        return true;

    return false;
}
void GameInstance::checkOutEliminatedPlayers()
{
    if(m_startDeck->deckSize() != 0)
        return;

    for(auto it = m_playerDecks.begin(); it != m_playerDecks.end(); ++it)
    {
        if((*it)->playerDeckSize() == 0)
        {
            m_playerDecks.erase(it);
            it = m_playerDecks.begin();
        }
    }
}


//=======================================================================
//![слоты [стол -> инстанс]]
void GameInstance::instanceSlotPlayerTryBeat(PlayerDeck* deck, PairID pairID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    Card* card = deck->takeCard(cardSuit, cardDignity);
    if(card == nullptr)
        deck->signalUnsuccesfullyAction();

    if(m_battleArea->checkPossibilityBeat(deck->id(), card, pairID))
    {
        emit this->instanceSignalPlayerBeatCard(deck->id(), pairID, cardSuit, cardDignity);
        resetPlayersActionButton();
    }
    else
    {
        deck->putCard(card);
        deck->signalUnsuccesfullyAction();
    }
}
void GameInstance::instanceSlotPlayerTryToss(PlayerDeck* deck, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(findPlayerDeckOfID(m_defenderID)->playerDeckSize() <= m_battleArea->amountOfNotBeatenPairs())
    {
        deck->signalUnsuccesfullyAction();
        return;
    }

    Card* card = deck->takeCard(cardSuit, cardDignity);
    if(card == nullptr)
    {
        deck->signalUnsuccesfullyAction();
        return;
    }

    if(m_battleArea->checkPossibilityToss(deck->id(), card))
    {
        emit this->instanceSignalPlayerTossedCard(deck->id(), cardSuit, cardDignity);
        resetPlayersActionButton();
    }
    else
    {
        deck->putCard(card);
        deck->signalUnsuccesfullyAction();
    }
}
void GameInstance::instanceSlotPlayerTryTransferable(PlayerDeck* deck, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(!m_croupier->trancferableAbility())
    {
        deck->signalUnsuccesfullyAction();
        return;
    }
    if(!m_battleArea->checkAllPairsIsNotBeaten())
    {
        deck->signalUnsuccesfullyAction();
        return;
    }
    if(findFollowingBeatingPlayer()->playerDeckSize() <= m_battleArea->amountOfPairs())
    {
        deck->signalUnsuccesfullyAction();
        return;
    }


    Card* card = deck->takeCard(cardSuit, cardDignity);
    if(card == nullptr)
    {
        deck->signalUnsuccesfullyAction();
        return;
    }

    if(m_battleArea->checkPossibilityTrancferable(deck->id(), card))
    {
        setNewMainPair();
        emit this->instanceSignalPlayerTrancferableCard(deck->id(), cardSuit, cardDignity, m_attackerID, m_defenderID);
        resetPlayersActionButton();
    }
    else
    {
        deck->putCard(card);
        deck->signalUnsuccesfullyAction();
    }
}

void GameInstance::instanceSlotPlayerWasClickedActionButton(PlayerDeck* deck)
{

    if(deck->id() == m_defenderID)
    {
        this->playerTakeCardsFromTable(deck->id());
    }
    else if(deck->id() == m_attackerID)
    {
        if(m_battleArea->checkAllPairsIsBeaten())
            this->makeTurn();
    }
}
//!
