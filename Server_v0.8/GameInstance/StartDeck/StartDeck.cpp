#include "StartDeck.h"

StartDeck::StartDeck(QObject *parent)
    :QObject{ parent },
     m_startDeck(NewDeckGenerator::generateDeck(parent))
    {
    }

Card* StartDeck::takeCard()
{
    Card* buf = m_startDeck.last();
    m_startDeck.pop_back();
    return buf;
}
Card* StartDeck::firstCard() { return m_startDeck.first(); }
quint8 StartDeck::deckSize() const { return m_startDeck.size(); }
