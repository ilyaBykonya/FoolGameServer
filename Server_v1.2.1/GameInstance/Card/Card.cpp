#include "Card.h"

Card::Card(Suit s, Dignity dig, QObject* parent)
    :QObject{ parent },
     m_dignity{ dig },
     m_suit{ s },
     m_location{ LocationState::InStartDeck }
    {
    }



Card::Dignity Card::dignity() const
{
    return m_dignity;
}
Card::Suit Card::suit() const
{
    return m_suit;
}



Card::LocationState Card::location() const
{
    return m_location;
}
void Card::setNewLocation(Card::LocationState newLocation)
{
    m_location = newLocation;
}
