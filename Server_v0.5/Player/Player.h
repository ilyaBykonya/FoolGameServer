#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include "Card.h"

namespace EthernetSignals
{
    namespace ClientSignal
    {
        enum TypeSignal
        {
            FromAppToServ = 0,
            FromAppInstToServInst
        };

        enum ApplicationSignal
        {
            PlayerWantPlay = 0,
            PlayerDepositMoney,
            PlayerWithdrawMoney,
            PlayerRegistration
        };
        enum TableSignal
        {

            ActionButtonWasClicked = 0,
            TryBeat,
            TryToss,
            TryTransferable
        };
    }

    namespace ServerSignal
    {
        enum TypeSignal
        {
            FromServToApp = 0,
            FromServInstToAppInst
        };

        enum ServerSignal
        {
            SetPlayerID = 0,
            GetAllPlayInstanceOptions,
            AlertMessage
        };
        enum InstanceSignal
        {
            PlayerTakeAllCards = 0,
            PlayerTossedCard,
            PlayerBeatCard,
            PlayerTrancferable,
            ThisPlayerTakeCardFromDeck,
            OtherPlayerTakeCardFromDeck,
            SetNewMainPlayersPair,
            MakeTurn,
            EndOfMatch,
            UnsuccesfullyAction
        };
    }
}

class MyPlayer : public QObject
{
    Q_OBJECT
public:

private:
    qint16 m_nextDataBlockSize;
    qint16 m_ID;
    QTcpSocket* m_socket;

public:
    explicit MyPlayer(qint16, QTcpSocket*, QObject *parent = nullptr);
    qint16 id() const { return m_ID; }
public slots:
    //чтение
    void slotReadClient();
    void slotReadFromAppToServ(QDataStream&);
    void slotReadFromAppInstToServInst(QDataStream&);


    //запись:
    //сообщения инстансу
    void slotInstancePlayerTakeAllCards(qint16);
    void slotInstancePlayerTossedCard(qint16, Card::Suit, Card::Dignity);
    void slotInstancePlayerBeatCard(qint16, qint16, Card::Suit, Card::Dignity);
    void slotInstancePlayerTransferable(qint16, Card::Suit, Card::Dignity);
    void slotInstanceTakeCardFromDeck(qint16, Card::Suit, Card::Dignity);
    void slotInstanceSetNewMainPlayersPair(qint16, qint16);
    void slotInstanceMakeTurn();
    void slotInstanceUnsuccesfullyToss(Card::Suit, Card::Dignity, qint16);
    void slotInstanceUnsuccesfullyBeat(Card::Suit, Card::Dignity, qint16);
    void slotInstanceUnsuccesfullyTrancferable(Card::Suit, Card::Dignity, qint16);
    void slotInstanceEndOfMatch(qint16);

    //сообщения приложению
    void serverSlotSendID();
    void serverSlotAlertMessage(const QString&, const QString&);
    void serverSlotGetAllPlayInstanceOptions(Card::Suit, Card::Dignity, QList<MyPlayer*>, qint16, qint16, qint16);

signals:
    void instanceSignalPlayerActionButtonWasClicked(MyPlayer*);
    void instanceSignalTryTransferable(MyPlayer*, Card::Suit, Card::Dignity);
    void instanceSignalPlayerTryBeat(MyPlayer*, qint16, Card::Suit, Card::Dignity);
    void instanceSignalPlayerTryToss(MyPlayer*, Card::Suit, Card::Dignity);

    void applicationSignalUserWantPlay(MyPlayer*, quint8, quint8, bool);
    void applicationSignalPlayerRegistration(const QString&, const QString&);
    void applicationSignalPlayerDepositMoney(qint16);
    void applicationSignalPlayerWithdrawMoney(qint16);
};

#endif // MYPLAYER_H
