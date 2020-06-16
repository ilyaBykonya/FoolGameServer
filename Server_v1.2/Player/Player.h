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
            FromAppInstToServInst,
            SignInSignals
        };


        enum ApplicationSignal
        {
            PlayerWantPlay = 0,
            PlayerDepositMoney,
            PlayerWithdrawMoney,

            MessageSignal
        };
        enum TableSignal
        {

            ActionButtonWasClicked = 0,
            TryBeat,
            TryToss,
            TryTransferable
        };

        enum SignIn
        {
            TryLogIn = 0,
            TryRegistration
        };
    }

    namespace ServerSignal
    {
        enum TypeSignal
        {
            FromServToApp = 0,
            FromServInstToAppInst,
            SignInSignals
        };

        enum ServerSignal
        {
            SetPlayerID = 0,
            GetAllPlayInstanceOptions,
            AlertMessage,

            UserMessageSignal,
            ServerMessageSignal
        };
        enum InstanceSignal
        {
            PlayerTakeAllCards = 0,
            PlayerTossedCard,
            PlayerBeatCard,
            PlayerTrancferable,
            ThisPlayerTakeCardFromDeck,
            OtherPlayerTakeCardFromDeck,
            MakeTurn,
            EndOfMatch,
            UnsuccesfullyAction
        };

        enum SignIn
        {
            SuccesfullyLogIn = 0,
            SuccesfullyRegistration,
            UnsuccesfullyLogIn,
            UnsuccesfullyRegistration
        };
    }
}


class Player : public QObject
{
    Q_OBJECT
public:
    enum Canal
    {
        GameCanal = 0,
        SharedCanal,
        MAX_CANAL
    };

    enum DeckType
    {
        Deck_24 = 0x01,
        Deck_36 = 0x02,
        Deck_52 = 0x04
    };
    enum CountPlayers
    {
        Players_2 = 0x01,
        Players_3 = 0x02,
        Players_4 = 0x04,
        Players_5 = 0x08,
        Players_6 = 0x10
    };
private:
    qint16 m_nextDataBlockSize;
    qint16 m_ID;
    QTcpSocket* m_socket;

    quint8 m_amountOfPlayers;
    quint8 m_deckSize;
    bool m_trancferableAbility;

    bool m_inFindGame;
public:
    explicit Player(qint16, QTcpSocket*, QObject *parent = nullptr);
    qint16 id() const { return m_ID; }

    quint8 amountOfPlayers() const { return m_amountOfPlayers; }
    quint8 deckSize() const { return m_deckSize; }
    bool trancferableAbility() const { return m_trancferableAbility; }

    bool inFindGame() const { return m_inFindGame; }
public slots:
    //![чтение]
        //![первая стадия]
        void slotReadClient();
        //!
        //![вторая стадия]
            //![чтение [регистрация]]
            void slotReadSignIn(QDataStream&);
            //!
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
            void slotInstanceUnsuccesfullyAction();
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
        //![запись [регистрация игрока]]
            void registrationSlotSuccesfullyLogIn();
            void registrationSlotUnsuccesfullyLogIn(const QString&);

            void registrationSlotSuccesfullyRegistration();
            void registrationSlotUnsuccesfullyRegistration(const QString&);
        //!
        //![отправка сообщений]
            //![от инстанса]
                void messageSlotReceiveMessageFromInstance(quint16, const QString&);
            //!
            //![от инстанса]
                void messageSlotReceiveMessageFromUser(quint16, Canal, const QString&);
            //!
            //![от сервера]
                void messageSlotReceiveMessageFromServer(const QString&);
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
    void registrationSignalPlayerLogIn(const QString&, const QString&);
    void registrationSignalPlayerRegistration(const QString&, const QString&);

    void applicationSignalUserWantPlay(Player*, quint8, quint8, bool);
    void applicationSignalPlayerDepositMoney(qint16);
    void applicationSignalPlayerWithdrawMoney(qint16);


    void messageSignalUserSendMessageOnCanal(quint16, Canal, const QString&);
    void messageSignalUserSendMessagePlayers(quint16, const QString&);

    //!
};

#endif // MYPLAYER_H
