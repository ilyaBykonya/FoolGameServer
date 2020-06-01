#include "TestInstance.h"

TestInstance::TestInstance(QList<MyPlayer*> players, QObject *parent)
    :QObject{ parent },
     m_players(players),
     amountOfCards{ 36 }
    {
        m_attackerID = m_players.at(0)->id();
        m_defenderID = m_players.at(1)->id();

        for(MyPlayer* player: m_players)
        {
            //сигналы от игрока инстансу
            QObject::connect(player, &MyPlayer::instanceSignalPlayerTryToss, this, &TestInstance::instanceSlotPlayerTryToss);
            QObject::connect(player, &MyPlayer::instanceSignalPlayerTryBeat, this, &TestInstance::instanceSlotPlayerTryBeat);
            QObject::connect(player, &MyPlayer::instanceSignalTryTransferable, this, &TestInstance::instanceSlotPlayerTryTransferable);
            QObject::connect(player, &MyPlayer::instanceSignalPlayerActionButtonWasClicked, this, &TestInstance::instanceSlotPlayerWasClickedActionButton);

            //сигналы от инстанса игроку
            QObject::connect(this, &TestInstance::instanceSignalMakeTurn, player, &MyPlayer::slotInstanceMakeTurn);
            QObject::connect(this, &TestInstance::instanceSignalEndOfMatch, player, &MyPlayer::slotInstanceEndOfMatch);
            QObject::connect(this, &TestInstance::instanceSignalPlayerBeatCard, player, &MyPlayer::slotInstancePlayerBeatCard);
            QObject::connect(this, &TestInstance::instanceSignalPlayerTossedCard, player, &MyPlayer::slotInstancePlayerTossedCard);
            QObject::connect(this, &TestInstance::instanceSignalPlayerTrancferableCard, player, &MyPlayer::slotInstancePlayerTransferable);
            QObject::connect(this, &TestInstance::instanceSignalTakeCardFromDeck, player, &MyPlayer::slotInstanceTakeCardFromDeck);
            QObject::connect(this, &TestInstance::instanceSignalPlayerTakeAllCards, player, &MyPlayer::slotInstancePlayerTakeAllCards);
            QObject::connect(this, &TestInstance::instanceSignalSetNewMainPlayersPair, player, &MyPlayer::slotInstanceSetNewMainPlayersPair);


            //сигнал начала матча (почучение настроек)
            QObject::connect(this, &TestInstance::signalGetAllPlayInstanceOptions, player, &MyPlayer::serverSlotGetAllPlayInstanceOptions);
            //в конце матча инстанс удаляется
            QObject::connect(this, &TestInstance::instanceSignalEndOfMatch, this, &QObject::deleteLater);
        }
        //отправляем игрокам информацию о матче
        emit signalGetAllPlayInstanceOptions(Card::Suit::Tambourine, Card::Dignity::king, m_players, m_attackerID, m_defenderID, amountOfCards);

        for(int i = 0; i < m_players.size(); ++i)
            m_finishedMove.push_back(false);


        //*****************************************************************************
        //TEST
        emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
        emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
        emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
        emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));

        emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
        emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
        emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
        emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
        amountOfCards -= 8;
        //*****************************************************************************
    }

void TestInstance::instanceSlotPlayerTryBeat(MyPlayer* player, qint16 pairID, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    qDebug() << QString("Try beat in instance. Card[%1:%2]. Pair[%3]").arg(qint16(cardSuit)).arg(qint16(cardDignity)).arg(pairID);
    if(rand() % 2)
    {
        emit instanceSignalPlayerBeatCard(player->id(), pairID, cardSuit, cardDignity);
    }
    else
    {
        player->slotInstanceUnsuccesfullyBeat(cardSuit, cardDignity, pairID);
    }
}
void TestInstance::instanceSlotPlayerTryToss(MyPlayer* player, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    qDebug() << QString("Try toss in instance. Card[%1:%2]").arg(qint16(cardSuit)).arg(qint16(cardDignity));
    if(rand() % 2)
    {

        emit instanceSignalPlayerTossedCard(player->id(), cardSuit, cardDignity);
    }
    else
    {
        player->slotInstanceUnsuccesfullyToss(cardSuit, cardDignity, ((cardSuit + 1) * 100 + cardDignity));
    }
}
void TestInstance::instanceSlotPlayerTryTransferable(MyPlayer* player, Card::Suit cardSuit, Card::Dignity cardDignity)
{
    qDebug() << QString("Player trancferable in instance. Card[%1:%2]").arg(qint16(cardSuit)).arg(qint16(cardDignity));
    if(rand() % 2)
    {

        emit instanceSignalPlayerTrancferableCard(player->id(), cardSuit, cardDignity);
        emit instanceSignalSetNewMainPlayersPair(m_defenderID, m_attackerID);
        qint16 temp = m_defenderID;
        m_defenderID = m_attackerID;
        m_attackerID = temp;
    }
    else
    {
        player->slotInstanceUnsuccesfullyTrancferable(cardSuit, cardDignity, ((cardSuit + 1) * 100 + cardDignity));
    }
}
void TestInstance::instanceSlotPlayerWasClickedActionButton(MyPlayer* player)
{
    qDebug() << QString("Player action button was clicked[%1]").arg(player->id());
    if(player->id() == m_attackerID)
    {
        qDebug() << QString("Player id == attacker id");
        qDebug() << QString("   Attacker id[%1]: Defender id[%2]").arg(m_attackerID).arg(m_defenderID);
        emit instanceSignalMakeTurn();
        qint16 temp = m_attackerID;
        m_attackerID = m_defenderID;
        m_defenderID = temp;
        if(amountOfCards > 0)
        {
        qDebug() << QString("   Attacker id[%1]: Defender id[%2]").arg(m_attackerID).arg(m_defenderID);
            emit instanceSignalSetNewMainPlayersPair(m_attackerID, m_defenderID);
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));

            emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_defenderID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            amountOfCards -= 8;
        }
        else
        {
            qDebug() << "Signal end of match_0";
            emit instanceSignalEndOfMatch(m_defenderID);
        }
    }
    else if(player->id() == m_defenderID)
    {
        qDebug() << QString("Player id == defender id");
        if(amountOfCards > 0)
        {
            emit instanceSignalPlayerTakeAllCards(m_defenderID);
            emit instanceSignalSetNewMainPlayersPair(m_attackerID, m_defenderID);
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            emit instanceSignalTakeCardFromDeck(m_attackerID, Card::Suit(rand() % Card::Suit::MAX_SUIT), Card::Dignity(rand() % Card::Dignity::MAX_DIGNITY));
            amountOfCards -= 4;
        }
        else
        {
            qDebug() << "Signal end of match_1";
            emit instanceSignalEndOfMatch(m_attackerID);
        }
    }
}
