#include "GameInstance.h"

GameInstance::GameInstance(Croupier::DeckSize deckSize, bool trancferableAbility, QList<Player*> playersList, QObject *parent)
    :QObject{ parent }
    {
        m_croupier = new Croupier(deckSize, trancferableAbility);
        m_startDeck = new StartDeck(m_croupier->deckSizeAmount(), this);

        //=========================================================

        quint8 index = 0;
        for(auto it = playersList.begin(); it != playersList.end(); ++it, ++index)
        {
            PlayerDeck* deck = new PlayerDeck((*it)->id());
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
            QObject::connect((*it), &Player::instanceSignalPlayerTryToss, this, &GameInstance::instanceSlotPlayerTryToss, Qt::ConnectionType::UniqueConnection);
            QObject::connect((*it), &Player::instanceSignalPlayerTryBeat, this, &GameInstance::instanceSlotPlayerTryBeat, Qt::ConnectionType::UniqueConnection);
            QObject::connect((*it), &Player::instanceSignalTryTransferable, this, &GameInstance::instanceSlotPlayerTryTransferable, Qt::ConnectionType::UniqueConnection);
            QObject::connect((*it), &Player::instanceSignalPlayerActionButtonWasClicked, this, &GameInstance::instanceSlotPlayerWasClickedActionButton, Qt::ConnectionType::UniqueConnection);

            //=========================================================
            //Сигналы [инстанс] --> [стол]
                //![сигналы рантайма игры]
                    //![сигналы ходов]
                    QObject::connect(this, &GameInstance::instanceSignalPlayerBeatCard, (*it), &Player::slotInstancePlayerBeatCard, Qt::ConnectionType::UniqueConnection);
                    QObject::connect(this, &GameInstance::instanceSignalPlayerTossedCard, (*it), &Player::slotInstancePlayerTossedCard, Qt::ConnectionType::UniqueConnection);
                    QObject::connect(this, &GameInstance::instanceSignalPlayerTrancferableCard, (*it), &Player::slotInstancePlayerTransferable, Qt::ConnectionType::UniqueConnection);
                    //!
                    //![сигнал конца шага]
                    QObject::connect(this, &GameInstance::instanceSignalMakeTurn, (*it), &Player::slotInstanceMakeTurn, Qt::ConnectionType::UniqueConnection);
                    //!
                    //![сигналы взятия карт]
                    QObject::connect(this, &GameInstance::instanceSignalTakeCardFromDeck, (*it), &Player::slotInstanceTakeCardFromDeck, Qt::ConnectionType::UniqueConnection);
                    QObject::connect(this, &GameInstance::instanceSignalPlayerTakeAllCards, (*it), &Player::slotInstancePlayerTakeAllCards, Qt::ConnectionType::UniqueConnection);
                    //!
                //!
                //!======================
                //![сигналы начала/конца]
                    //![сигнал начала игры]
                    QObject::connect(this, &GameInstance::signalGetAllPlayInstanceOptions, (*it), &Player::serverSlotGetAllPlayInstanceOptions, Qt::ConnectionType::UniqueConnection);
                    //!
                    //![сигнал конца игры]
                    QObject::connect(this, &GameInstance::instanceSignalEndOfMatch, (*it), &Player::slotInstanceEndOfMatch, Qt::ConnectionType::UniqueConnection);
                    //!
                //!
                //![сигнал сообщений]
                QObject::connect((*it), &Player::messageSignalUserSendMessagePlayers, this, &GameInstance::messageSignalSendUserMessage, Qt::ConnectionType::UniqueConnection);
                QObject::connect(this, &GameInstance::messageSignalSendUserMessage, (*it), &Player::messageSlotReceiveMessageFromInstance, Qt::ConnectionType::UniqueConnection);
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
        drawCards();
    }

PlayerDeck* GameInstance::findPlayerDeckOfID(qint16 id)
{
    for(auto it = m_playerDecks.begin(); it != m_playerDecks.end(); ++it)
    {
        if((*it)->id() == id)
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
void GameInstance::playerTakeCardsFromTable(qint16 playerID)
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
void GameInstance::instanceSlotPlayerTryBeat(Player* player, quint16 pairID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    PlayerDeck* deck = findPlayerDeckOfID(player->id());
    if(deck == nullptr)
        player->slotInstanceUnsuccesfullyBeat(cardSuit, cardDignity, pairID);

    Card* card = deck->takeCard(cardSuit, cardDignity);
    if(card == nullptr)
        player->slotInstanceUnsuccesfullyBeat(cardSuit, cardDignity, pairID);

    if(m_battleArea->checkPossibilityBeat(player->id(), card, pairID))
    {
        emit this->instanceSignalPlayerBeatCard(player->id(), pairID, cardSuit, cardDignity);
        resetPlayersActionButton();
    }
    else
    {
        deck->putCard(card);
        player->slotInstanceUnsuccesfullyBeat(cardSuit, cardDignity, pairID);
    }
}
void GameInstance::instanceSlotPlayerTryToss(Player* player, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(findPlayerDeckOfID(m_defenderID)->playerDeckSize() <= m_battleArea->amountOfNotBeatenPairs())
    {
        player->slotInstanceUnsuccesfullyToss(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }

    PlayerDeck* deck = findPlayerDeckOfID(player->id());
    if(deck == nullptr)
    {
        player->slotInstanceUnsuccesfullyToss(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }
    Card* card = deck->takeCard(cardSuit, cardDignity);
    if(card == nullptr)
    {
        player->slotInstanceUnsuccesfullyToss(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }

    if(m_battleArea->checkPossibilityToss(player->id(), card))
    {
        emit this->instanceSignalPlayerTossedCard(player->id(), cardSuit, cardDignity);
        resetPlayersActionButton();
    }
    else
    {
        deck->putCard(card);
        player->slotInstanceUnsuccesfullyToss(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
    }
}
void GameInstance::instanceSlotPlayerTryTransferable(Player* player, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    if(!m_croupier->trancferableAbility())
    {
        player->slotInstanceUnsuccesfullyTrancferable(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }
    if(!m_battleArea->checkAllPairsIsNotBeaten())
    {
        player->slotInstanceUnsuccesfullyTrancferable(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }
    if(findFollowingBeatingPlayer()->playerDeckSize() <= m_battleArea->amountOfPairs())
    {
        player->slotInstanceUnsuccesfullyTrancferable(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }
    PlayerDeck* deck = findPlayerDeckOfID(player->id());
    if(deck == nullptr)
    {
        player->slotInstanceUnsuccesfullyTrancferable(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }
    Card* card = deck->takeCard(cardSuit, cardDignity);
    if(card == nullptr)
    {
        player->slotInstanceUnsuccesfullyTrancferable(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
        return;
    }

    if(m_battleArea->checkPossibilityTrancferable(player->id(), card))
    {
        setNewMainPair();
        emit this->instanceSignalPlayerTrancferableCard(player->id(), cardSuit, cardDignity, m_attackerID, m_defenderID);
        resetPlayersActionButton();
    }
    else
    {
        deck->putCard(card);
        player->slotInstanceUnsuccesfullyTrancferable(cardSuit, cardDignity, ((cardSuit + 1) * 100) + cardDignity);
    }
}

void GameInstance::instanceSlotPlayerWasClickedActionButton(Player* player)
{

    if(player->id() == m_defenderID)
    {
        this->playerTakeCardsFromTable(player->id());
    }
    else if(player->id() == m_attackerID)
    {
        if(m_battleArea->checkAllPairsIsBeaten())
            this->makeTurn();
    }
}
//!
