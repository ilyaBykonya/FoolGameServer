#ifndef PLAYERDECK_H
#define PLAYERDECK_H
#include "Player/Player.h"
#include <QObject>
#include <QList>
#include "../Card/Card.h"

class PlayerDeck : public QObject
{
    Q_OBJECT
private:
    Player* m_controlThisDeckPlayer;
    QList<Card*> m_playerDeck;

    QueueMove m_playerState;
    bool m_actionButtonState;

public:
    explicit PlayerDeck(Player*, QObject* = nullptr);


    void putCard(Card*);
    Card* takeCard(Card::Suit, Card::Dignity);

    QueueMove playerState() const;
    void setPlayerState(QueueMove);

    quint8 playerDeckSize() const;
    UserID id() const;

    bool actionButtonState() const;
    void setActionButtonState(bool);

    Player* player();

protected slots:
    void slotThisPlayerTryBeat(PairID, Card::Suit, Card::Dignity);
    void slotThisPlayerTryToss(Card::Suit, Card::Dignity);
    void slotThisPlayerTryTransferable(Card::Suit, Card::Dignity);
    void slotThisPlayerClickedActionButton();

signals:
    void signalThisPlayerTryBeat(PlayerDeck*, PairID, Card::Suit, Card::Dignity);
    void signalThisPlayerTryToss(PlayerDeck*, Card::Suit, Card::Dignity);
    void signalThisPlayerTryTransferable(PlayerDeck*, Card::Suit, Card::Dignity);
    void signalThisPlayerClickedActionButton(PlayerDeck*);

    void signalUnsuccesfullyAction();


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
            void instanceSignalEndOfMatch(QList<Player*>);
            //!
        //!
    //!
    //!
    void signalUserDisconnected();
};

#endif // PLAYERDECK_H
