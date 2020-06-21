#ifndef TESTINSTANCE_H
#define TESTINSTANCE_H
#include "ChatRoom/ChatRoom.h"
#include "Croupier/Croupier.h"
#include "StartDeck/StartDeck.h"
#include "BattleArea/BattleArea.h"
#include "PlayerDeck/PlayerDeck.h"
#include "HandUp/HandUp.h"
#include <QTimer>


class GameInstance : public QObject
{
    Q_OBJECT
private:
    QList<PlayerDeck*> m_playerDecks;
    UserID m_attackerID;
    UserID m_defenderID;

    StartDeck* m_startDeck;
    BattleArea* m_battleArea;
    HandUp* m_handUp;

    Croupier* m_croupier;
    ChatRoom* m_chatRoom;

public:
    explicit GameInstance(Croupier::DeckSize, bool, QList<Player*>, QObject *parent = nullptr);

protected:
    PlayerDeck* findPlayerDeckOfID(UserID);

    //![обнулить нажатия action button]
    void resetPlayersActionButton();
    //!

    //[переопределить офера/дефера]
    void setNewMainPair();
    //!

    //![сброс карт со стола]
        //![отбой]
        void makeTurn();
        //!
        //![игрок берёт карты]
        void playerTakeCardsFromTable(UserID);
        //!
    //!
    //![конец матча]
        //![проверить конец матча]
        bool checkEndOfMatch();
        //!
        //![проверить выбывших игроков]
        void checkOutEliminatedPlayers();
        //!
    //!

    //![раздать недостающие карты]
    void drawCards();

    //![найти игрока, идущего за отбивающимся]
    PlayerDeck* findFollowingBeatingPlayer();

public slots:
    //![слоты [стол -> инстанс]]
    void instanceSlotPlayerTryBeat(PlayerDeck*, PairID, Card::Suit, Card::Dignity);
    void instanceSlotPlayerTryToss(PlayerDeck*, Card::Suit, Card::Dignity);
    void instanceSlotPlayerTryTransferable(PlayerDeck*, Card::Suit, Card::Dignity);

    void instanceSlotPlayerWasClickedActionButton(PlayerDeck*);
    //!

signals:
    //![сигналы [инстанс -> стол]]
        //![сигналы рантайма игры]
            //![сигналы ходов]
            void instanceSignalPlayerTossedCard(UserID, Card::Suit, Card::Dignity);
            void instanceSignalPlayerBeatCard(UserID, PairID, Card::Suit, Card::Dignity);
            void instanceSignalPlayerTrancferableCard(UserID, Card::Suit, Card::Dignity, UserID, UserID);
            //!
            //![сигнал взятия карт]
            void instanceSignalTakeCardFromDeck(UserID, Card::Suit, Card::Dignity);
            //!
            //![сигналы конца шага]
            void instanceSignalPlayerTakeAllCards(UserID, UserID, UserID);
            void instanceSignalMakeTurn(UserID, UserID);
            //!
        //!
        //! =====================
        //![сигналы начала/конца]
            //![сигнал начала раунда]
            void signalGetAllPlayInstanceOptions(Card::Suit, Card::Dignity, QList<Player*>, UserID, UserID, quint8);
            //!
            //![сигнал конца раунда]
            void instanceSignalEndOfMatch(UserID);
            //!
        //!
    //!
};

#endif // TESTINSTANCE_H
