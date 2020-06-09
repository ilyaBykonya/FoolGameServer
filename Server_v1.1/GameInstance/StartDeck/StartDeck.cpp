#include "StartDeck.h"

StartDeck::StartDeck(quint8 deckSize, QObject *parent)
    :QObject{ parent },
     m_startDeck(NewDeckGenerator::generateDeck(deckSize, parent))
    {
    }

Card* StartDeck::takeCard()
{
    if(m_startDeck.isEmpty())
        return nullptr;

    Card* buf = m_startDeck.last();
    m_startDeck.pop_back();
    qDebug() << "Take card " << m_startDeck.size();
    return buf;
}
Card* StartDeck::firstCard() { return m_startDeck.first(); }
quint8 StartDeck::deckSize() const { return m_startDeck.size(); }
