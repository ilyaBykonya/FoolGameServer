#ifndef CROUPIER_H
#define CROUPIER_H

#include <QObject>
#include "../Card/Card.h"
class Croupier : public QObject
{
public:
    enum DeckSize
    {
        DeckSize_24,
        DeckSize_36,
        DeckSize_52
    };
private:
    DeckSize m_deckSize;
    bool m_trancferableAbility;

public:
    explicit Croupier(DeckSize, bool, QObject *parent = nullptr);

    DeckSize deckSizeType() const;
    quint8 deckSizeAmount() const;
    bool trancferableAbility() const;

};

#endif // CROUPIER_H
