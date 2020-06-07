#include "NewDeckGenerator.h"

QList<Card*> NewDeckGenerator::generateDeck(QObject* cardParent)
{
    QList<Card*> newDeck;
    for(quint8 suit = 0; suit < Card::Suit::MAX_SUIT; ++suit)
    {
        for(quint8 dignity = 0; dignity < Card::Dignity::MAX_DIGNITY; ++dignity)
        {
            newDeck.push_back(new Card(Card::Suit(suit), Card::Dignity(dignity), cardParent));
        }
    }

    std::random_shuffle(newDeck.begin(), newDeck.end());
    return newDeck;
}
