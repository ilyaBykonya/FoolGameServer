#include "Croupier.h"

Croupier::Croupier(DeckType deckType, bool transferableAbility, QObject *parent)
    :QObject{ parent },
     m_trancferableAbility{ transferableAbility }
    {
        switch (deckType)
        {
            case DeckType::Deck_24:
            {
                m_deckSize = 24;
                break;
            }
            case DeckType::Deck_36:
            {
                m_deckSize = 36;
                break;
            }
            default:
            {
                m_deckSize = 52;
                break;
            }
        }
    }

DeckSize Croupier::deckSize() const
{
    return m_deckSize;
}
bool Croupier::trancferableAbility() const
{
    return m_trancferableAbility;
}
