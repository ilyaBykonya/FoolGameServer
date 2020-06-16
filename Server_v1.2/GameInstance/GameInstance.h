#ifndef TESTINSTANCE_H
#define TESTINSTANCE_H
#include <QMap>
#include "Player/Player.h"
#include "Croupier/Croupier.h"
#include "StartDeck/StartDeck.h"
#include "BattleArea/BattleArea.h"
#include "PlayerDeck/PlayerDeck.h"
#include "HandUp/HandUp.h"


class GameInstance : public QObject
{
    Q_OBJECT
private:
    QList<PlayerDeck*> m_playerDecks;
    qint16 m_attackerID;
    qint16 m_defenderID;

    StartDeck* m_startDeck;
    BattleArea* m_battleArea;
    HandUp* m_handUp;

    Croupier* m_croupier;

public:
    explicit GameInstance(Croupier::DeckSize, bool, QList<Player*>, QObject *parent = nullptr);

protected:
    PlayerDeck* findPlayerDeckOfID(qint16);

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
        void playerTakeCardsFromTable(qint16);
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
    void instanceSlotPlayerTryBeat(Player*, quint16, Card::Suit, Card::Dignity);
    void instanceSlotPlayerTryToss(Player*, Card::Suit, Card::Dignity);
    void instanceSlotPlayerTryTransferable(Player*, Card::Suit, Card::Dignity);

    void instanceSlotPlayerWasClickedActionButton(Player*);
    //!

signals:
    //![сигналы [инстанс -> стол]]
        //![сигналы рантайма игры]
            //![сигналы ходов]
            void instanceSignalPlayerTossedCard(qint16, Card::Suit, Card::Dignity);
            void instanceSignalPlayerBeatCard(qint16, qint16, Card::Suit, Card::Dignity);
            void instanceSignalPlayerTrancferableCard(qint16, Card::Suit, Card::Dignity, qint16, qint16);
            //!
            //![сигнал взятия карт]
            void instanceSignalTakeCardFromDeck(qint16, Card::Suit, Card::Dignity);
            //!
            //![сигналы конца шага]
            void instanceSignalPlayerTakeAllCards(qint16, qint16, qint16);
            void instanceSignalMakeTurn(qint16, qint16);
            //!
        //!
        //! =====================
        //![сигналы начала/конца]
            //![сигнал начала раунда]
            void signalGetAllPlayInstanceOptions(Card::Suit, Card::Dignity, QList<Player*>, qint16, qint16, qint16);
            //!
            //![сигнал конца раунда]
            void instanceSignalEndOfMatch(qint16);
            //!
        //!
        //![синал сообщений]
            void messageSignalSendUserMessage(quint16, const QString&);
        //!
    //!
};

#endif // TESTINSTANCE_H
