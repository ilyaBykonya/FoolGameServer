#include "Croupier.h"

Croupier::Croupier(Croupier::DeckSize deckSize, bool transferableAbility, QObject *parent)
    :QObject{ parent },
     m_deckSize{ deckSize },
     m_trancferableAbility{ transferableAbility }
    {
    }

Croupier::DeckSize Croupier::deckSizeType() const
{
    return m_deckSize;
}
quint8 Croupier::deckSizeAmount() const
{
    switch (m_deckSize)
    {
        case DeckSize::DeckSize_24: return 24;
        case DeckSize::DeckSize_36: return 36;
    default: return 52;
    }
}

bool Croupier::trancferableAbility() const
{
    return m_trancferableAbility;
}
