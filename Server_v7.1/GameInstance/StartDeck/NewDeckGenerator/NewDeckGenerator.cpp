#include "NewDeckGenerator.h"

QList<Card*> NewDeckGenerator::generateDeck(quint8 deckSize, QObject* cardParent)
{
    QList<Card*> newDeck;
    quint8 firstDignity = 13 - (deckSize / 4);

    for(quint8 suit = 0; suit < Card::Suit::MAX_SUIT; ++suit)
    {
        for(quint8 dignity = firstDignity; dignity < Card::Dignity::MAX_DIGNITY; ++dignity)
        {
            newDeck.push_back(new Card(Card::Suit(suit), Card::Dignity(dignity), cardParent));
        }
    }

    std::random_shuffle(newDeck.begin(), newDeck.end());
    return newDeck;
}
