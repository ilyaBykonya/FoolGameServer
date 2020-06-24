    #ifndef MYPLAYER_H
#define MYPLAYER_H
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include "TypesAliases.h"
#include "SettingsStruct/SettingsStruct.h"
#include "../../FoolGameServer/GameInstance/Card/Card.h"
enum Canal
{
    GameCanal = 0,
    SharedCanal,
    MAX_CANAL
};

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

        namespace Signals
        {
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
    }

    namespace ServerSignal
    {
        enum TypeSignal
        {
            FromServToApp = 0,
            FromServInstToAppInst,
            SignInSignals
        };
        namespace Signals
        {
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
}


class Player : public QObject
{
    Q_OBJECT
public:
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
    quint16 m_nextDataBlockSize;
    QTcpSocket* m_socket;

    UserID m_ID;
    QString m_userName;

    SettingsStruct m_settings;

    bool m_inFindGame;
public:
    explicit Player(UserID, QTcpSocket*, QObject *parent = nullptr);
    UserID id() const { return m_ID; }

    quint8 amountOfPlayers() const { return m_settings.m_countOfPlayers; }
    quint8 deckSize() const { return m_settings.m_deckType; }
    bool trancferableAbility() const { return m_settings.m_trancferableAbility; }

    bool inFindGame() const { return m_inFindGame; }

protected:
    friend QDataStream& operator<<(QDataStream&, const Player&);

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
            void slotInstancePlayerTossedCard(UserID, Card::Suit, Card::Dignity);
            void slotInstancePlayerBeatCard(UserID, PairID, Card::Suit, Card::Dignity);
            void slotInstancePlayerTransferable(UserID, Card::Suit, Card::Dignity, UserID, UserID);
            //!
            //![провалившиеся действия игроков]
            void slotInstanceUnsuccesfullyAction();
            //!
            //![конец шага]
                //![игрок взял карты]
                void slotInstancePlayerTakeAllCards(UserID, UserID, UserID);
                //!
                //![отбой]
                void slotInstanceMakeTurn(UserID, UserID);
                //!
            //!
            //![игрок берёт карты]
            void slotInstanceTakeCardFromDeck(UserID, Card::Suit, Card::Dignity);
            //!
            //![конец игры]
            void slotInstanceEndOfMatch(UserID);
            //!
        //!
        //![запись [сервер -> приложение]]
            //![начало игры]
            void serverSlotGetAllPlayInstanceOptions(Card::Suit, Card::Dignity, QList<Player*>, UserID, UserID, quint8);
            //!
            //![получить id (юзается 1 раз при подключении)]
            void serverSlotSendID();
            //!
            //![alert message]
            void serverSlotAlertMessage(const QString&, const QString&);
            //!
        //!
        //![запись [регистрация игрока]]
            void registrationSlotSuccesfullyLogIn(const QString&);
            void registrationSlotUnsuccesfullyLogIn(const QString&);

            void registrationSlotSuccesfullyRegistration(const QString&);
            void registrationSlotUnsuccesfullyRegistration(const QString&);
        //!
        //![отправка сообщений]
            void messageSlotReceiveUserMessage(Player*, Canal, const QString&);
            void messageSlotReceiveServerMessage(const QString&);
        //!
    //!


signals:
    //![сигналы [стол -> инстанс]]
    void instanceSignalPlayerTryTransferable(Card::Suit, Card::Dignity);
    void instanceSignalPlayerTryBeat(PairID, Card::Suit, Card::Dignity);
    void instanceSignalPlayerTryToss(Card::Suit, Card::Dignity);

    void instanceSignalPlayerActionButtonWasClicked();
    //!
    //![сигналы [клиент -> сервер]]
    void registrationSignalPlayerLogIn(const QString&, const QString&);
    void registrationSignalPlayerRegistration(const QString&, const QString&);

    void applicationSignalUserWantPlay(Player*, SettingsStruct);
    void applicationSignalPlayerDepositMoney(Cash);
    void applicationSignalPlayerWithdrawMoney(Cash);



    void messageSignalUserSendMessage(Player*, Canal, const QString&);
    //!
};

#endif // MYPLAYER_H
