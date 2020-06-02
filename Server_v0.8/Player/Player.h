#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include "../../FoolGameServer/GameInstance/Card/Card.h"

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

class Player : public QObject
{
    Q_OBJECT
public:

private:
    qint16 m_nextDataBlockSize;
    qint16 m_ID;
    QTcpSocket* m_socket;

public:
    explicit Player(qint16, QTcpSocket*, QObject *parent = nullptr);
    qint16 id() const { return m_ID; }
public slots:
    //![чтение]
        //![первая стадия]
        void slotReadClient();
        //!
        //![вторая стадия]
            //![чтение [клиент -> сервер]]
            void slotReadFromAppToServ(QDataStream&);
            //!
            //![чтение [стол -> инстанс]]
            void slotReadFromAppInstToServInst(QDataStream&);
            //!
        //!
    //!


    //![запись]
        //![запись [инстанс -> стол]]
            //![удачные действия игроков]
            void slotInstancePlayerTossedCard(qint16, Card::Suit, Card::Dignity);
            void slotInstancePlayerBeatCard(qint16, qint16, Card::Suit, Card::Dignity);
            void slotInstancePlayerTransferable(qint16, Card::Suit, Card::Dignity, qint16, qint16);
            //!
            //![провалившиеся действия игроков]
            void slotInstanceUnsuccesfullyToss(Card::Suit, Card::Dignity, qint16);
            void slotInstanceUnsuccesfullyBeat(Card::Suit, Card::Dignity, qint16);
            void slotInstanceUnsuccesfullyTrancferable(Card::Suit, Card::Dignity, qint16);
            //!
            //![конец шага]
                //![игрок взял карты]
                void slotInstancePlayerTakeAllCards(qint16, qint16, qint16);
                //!
                //![отбой]
                void slotInstanceMakeTurn(qint16, qint16);
                //!
            //!
            //![игрок берёт карты]
            void slotInstanceTakeCardFromDeck(qint16, Card::Suit, Card::Dignity);
            //!
            //![конец игры]
            void slotInstanceEndOfMatch(qint16);
            //!
        //!
        //![запись [сервер -> приложение]]
            //![начало игры]
            void serverSlotGetAllPlayInstanceOptions(Card::Suit, Card::Dignity, QList<Player*>, qint16, qint16, qint16);
            //!
            //![получить id (юзается 1 раз при подключении)]
            void serverSlotSendID();
            //!
            //![alert message]
            void serverSlotAlertMessage(const QString&, const QString&);
            //!
        //!
    //!

signals:
    //![сигналы [стол -> инстанс]]
    void instanceSignalTryTransferable(Player*, Card::Suit, Card::Dignity);
    void instanceSignalPlayerTryBeat(Player*, qint16, Card::Suit, Card::Dignity);
    void instanceSignalPlayerTryToss(Player*, Card::Suit, Card::Dignity);

    void instanceSignalPlayerActionButtonWasClicked(Player*);
    //!
    //![сигналы [клиент -> сервер]]
    void applicationSignalUserWantPlay(Player*, quint8, quint8, bool);
    void applicationSignalPlayerRegistration(const QString&, const QString&);
    void applicationSignalPlayerDepositMoney(qint16);
    void applicationSignalPlayerWithdrawMoney(qint16);
    //!
};

#endif // MYPLAYER_H
