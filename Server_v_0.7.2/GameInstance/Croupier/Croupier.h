#ifndef CROUPIER_H
#define CROUPIER_H
#include "../Card/Card.h"
#include "../../Player/SettingsStruct/SettingsStruct.h"
class Croupier : public QObject
{
public:

private:
    DeckSize m_deckSize;
    bool m_trancferableAbility;

public:
    explicit Croupier(DeckType, bool, QObject *parent = nullptr);

    DeckSize deckSize() const;
    bool trancferableAbility() const;

};

#endif // CROUPIER_H
