#ifndef BATTLEAREA_H
#define BATTLEAREA_H
#include <QList>
#include <QObject>
#include "BattlePair/BattlePair.h"

class BattleArea : public QObject
{
    Q_OBJECT
    QList<BattlePair*> m_pairsList;

    qint16 m_attackerID;
    qint16 m_defenderID;

    Card::Suit m_trump;
public:
    explicit BattleArea(Card::Suit, qint16, qint16, QObject *parent = nullptr);

    //![функции ходов игроков]
    public:
        //![отбивание карт]
            //![проверить возможность отбивания карты]
            bool checkPossibilityBeat(qint16, Card*, quint16);
            //!
        //!
        //![перевод карт]
            //![проверить возможность перевода карт]
            bool checkPossibilityTrancferable(qint16, Card*);
            //!
        //!
        //![подбрасывание карт]
            //![проверить возможность подбрасывания карт]
            bool checkPossibilityToss(qint16, Card*);
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
    void setNewMainPair(qint16, qint16);
    //!

protected:
    BattlePair* findPairOfID(quint16);
};

#endif // BATTLEAREA_H
