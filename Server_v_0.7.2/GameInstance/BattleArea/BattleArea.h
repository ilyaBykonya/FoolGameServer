#ifndef BATTLEAREA_H
#define BATTLEAREA_H
#include <QList>
#include <QObject>
#include "BattlePair/BattlePair.h"
#include "TypesAliases.h"
class BattleArea : public QObject
{
    Q_OBJECT
    QList<BattlePair*> m_pairsList;

    UserID m_attackerID;
    UserID m_defenderID;

    Card::Suit m_trump;
public:
    explicit BattleArea(Card::Suit, UserID, UserID, QObject *parent = nullptr);

    //![функции ходов игроков]
    public:
        //![отбивание карт]
            //![проверить возможность отбивания карты]
            bool checkPossibilityBeat(UserID, Card*, quint16);
            //!
        //!
        //![перевод карт]
            //![проверить возможность перевода карт]
            bool checkPossibilityTrancferable(UserID, Card*);
            //!
        //!
        //![подбрасывание карт]
            //![проверить возможность подбрасывания карт]
            bool checkPossibilityToss(UserID, Card*);
            //!
        //!
    protected:
        //![отбивание карт]
            //![побить карту]
            void beatCard(BattlePair*, Card*);
            //!
        //!
        //![перевод карт]
            //![перевести карты]
            void trancferableCard(Card*);
            //!
        //!
        //![подбрасывание карт]
            //![подбросить карту]
            void tossCard(Card*);
            //!
        //!
    //!

public:
    //![функции конца хода]
        //![собирает и возвращает все карты. Они идут либо в отбой, либо взявшему их игроку]
        QList<Card*> endOfStep();
        //!
        //![проверяет, все ли карты]
            //![побиты]
            bool checkAllPairsIsBeaten() const;
            //!
            //![не побиты]
            bool checkAllPairsIsNotBeaten() const;
            //!
        //!
        //![число пар]
            //![всего]
            quint8 amountOfPairs() const;
            //!
            //![число отбитых пар]
            quint8 amountOfBeatenPairs() const;
            //!
            //![число не отбитых пар]
            quint8 amountOfNotBeatenPairs() const;
            //!
        //!
    //!
    //![переопределяет офера/дефера]
    void setNewMainPair(UserID, UserID);
    //!

protected:
    BattlePair* findPairOfID(PairID);
};

#endif // BATTLEAREA_H
