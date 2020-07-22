#ifndef BATTLEPAIR_H
#define BATTLEPAIR_H
#include <QObject>
#include "../../Card/Card.h"
#include "TypesAliases.h"

class BattlePair : public QObject
{
    Q_OBJECT
public:
    enum PairState
    {
        Empty = 0,
        NotBeaten,
        Beaten
    };
    
private:
    Card* m_firstCard;
    Card* m_secondCard;
    
    PairState m_state;

    PairID m_pairID;
public:
    explicit BattlePair(QObject *parent = nullptr);
    
    void tossCard(Card*);
    void beatCard(Card*);

    Card* firstCard();
    Card* secondCard();
    
    PairState state() const;
    PairID id() const;
};

#endif // BATTLEPAIR_H
