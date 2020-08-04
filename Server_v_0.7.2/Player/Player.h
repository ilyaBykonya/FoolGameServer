#pragma once
#include <QTimer>
#include <QWebSocket>
#include <QDataStream>
#include "TypesAliases.h"
#include "UserInformation/UserInformation.h"
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
                PlayerExitFromWaitingRoom,

                PlayerDepositMoney,
                PlayerWithdrawMoney,

                MessageSignal,
                UpdateUserAvatar
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
                TryRegistration,
                LogOut
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
                YouAreAddedToTheQueue = 0,
                YouAreRemovedFromTheQueue,

                GetAllPlayInstanceOptions,


                AlertMessage,

                UserMessageSignal,
                ServerMessageSignal,


                UpdateUserInformation
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
                SuccesfullySignIn = 0,
                UnsuccesfullySignIn,
                LogOut
            };
        }
    }
}

class Player : public QObject
{
    Q_OBJECT
public:
    enum UserState
    {
        UserNoSignIn = 0,
        UserInLobbi,
        UserWaitGame,
        UserInGame
    };

private:
    QWebSocket* m_webSocket;

    UserInformation* m_userInfo;
    SettingsStruct m_settings;
    QPair<quint32, quint32> m_cashRange;

    UserState m_userState;

public:
    explicit Player(QWebSocket*, QObject *parent = nullptr);
    ~Player();
    UserID id() const { return m_userInfo->id(); }

    const SettingsStruct& playerSettings() const { return m_settings; }

    UserState userState() const { return m_userState; }
    UserInformation* userInfo() { return m_userInfo; }

    quint32 minCash() const { return m_cashRange.first; }
    quint32 maxCash() const { return m_cashRange.second; }

protected:
    friend QDataStream& operator<<(QDataStream&, const Player&);

public slots:
    //![чтение]
        //![первая стадия]
        void slotReadClient(const QByteArray&);
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
                //![игрок берёт карты]
                void slotInstanceTakeCardFromDeck(UserID, Card::Suit, Card::Dignity);
                //!
                //![отбой]
                void slotInstanceMakeTurn(UserID, UserID);
                //!
            //!
            //![конец игры]
            void slotInstanceEndOfMatch(QList<Player*>);
            //!
        //!
        //![запись [сервер -> приложение]]
            //![начало игры]
            void serverSlotGetAllPlayInstanceOptions(Card::Suit, Card::Dignity, QList<Player*>, UserID, UserID, quint8);
            //!
            //![alert message]
            void serverSlotAlertMessage(const QString&, const QString&);
            //!
        //!
        //![запись [отправка игрока в бой]]
            void queueSlotYouAreAddedToTheQueue();
            void queueSlotYouAreRemovedFromTheQueue();
        //!
        //![запись [регистрация игрока]]
            void registrationSlotSuccesfullySignIn(UserInformation*);
            void registrationSlotUnsuccesfullySignIn(const QString&);
            void registrationSlotLogOut();
        //!
        //![отправка сообщений]
            void messageSlotReceiveUserMessage(Player*, Canal, const QString&);
            void messageSlotReceiveServerMessage(const QString&);
        //!

        //![обновление информации об игроке]
            void slotUpdateUserInfo();
        //!
    //!
    //![слоты пополнения/снятия средств]
        void cashSlotDepositDollars(Cash);
        void cashSlotWithdrawDollars(Cash);
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
    void registrationSignalPlayerLogOut(Player*);


    void applicationSignalUserWantPlay(Player*);
    void applicationSignalUserExitFromWaitingRoom(Player*);

    void applicationSignalPlayerDepositMoney(Cash);
    void applicationSignalPlayerWithdrawMoney(Cash);



    void messageSignalUserSendMessage(Player*, Canal, const QString&);
    //!
    void saveUserInfoInDataBase(Player*);






    void signalUserDisconnected();
};


