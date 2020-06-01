#ifndef TESTINSTANCE_H
#define TESTINSTANCE_H

#include <QObject>
#include <QTcpSocket>
#include "MyPlayer.h"

class TestInstance : public QObject
{
    Q_OBJECT
private:
    QList<bool> m_finishedMove;
    QList<MyPlayer*> m_players;
    qint16 m_attackerID;
    qint16 m_defenderID;

    //**************
    //TEST
    qint16 amountOfCards;
public:
    explicit TestInstance(QList<MyPlayer*>, QObject *parent = nullptr);

public slots:
    void instanceSlotPlayerWasClickedActionButton(MyPlayer*);
    void instanceSlotPlayerTryBeat(MyPlayer*, qint16, Card::Suit, Card::Dignity);
    void instanceSlotPlayerTryToss(MyPlayer*, Card::Suit, Card::Dignity);
    void instanceSlotPlayerTryTransferable(MyPlayer*, Card::Suit, Card::Dignity);

signals:
    void instanceSignalPlayerTakeAllCards(qint16);
    void instanceSignalPlayerTossedCard(qint16, Card::Suit, Card::Dignity);
    void instanceSignalPlayerBeatCard(qint16, qint16, Card::Suit, Card::Dignity);
    void instanceSignalPlayerTrancferableCard(qint16, Card::Suit, Card::Dignity);
    void instanceSignalTakeCardFromDeck(qint16, Card::Suit, Card::Dignity);
    void instanceSignalSetNewMainPlayersPair(qint16, qint16);
    void instanceSignalMakeTurn();
    void instanceSignalEndOfMatch(qint16);


    void signalGetAllPlayInstanceOptions(Card::Suit, Card::Dignity, QList<MyPlayer*>, qint16, qint16, qint16);
};

#endif // TESTINSTANCE_H
