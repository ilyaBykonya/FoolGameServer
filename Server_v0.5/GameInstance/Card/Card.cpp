#include "Card.h"

Card::Card(Suit s, Dignity dig, QObject* parent)
    :QObject{ parent },
     m_dignity{ dig },
     m_suit{ s }
    {
    }

bool Card::operator<(const Card* other)
{
    if(this->suit() < other->suit())
        return true;
    else if(this->suit() > other->suit())
        return false;
    else
        return (this->dignity() < other->dignity());
}
