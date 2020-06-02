#include "NewDeckGenerator.h"

QList<Card*> NewDeckGenerator::generateDeck(QObject* cardParent)
{
    QList<Card*> newDeck;
    for(quint8 suit = Card::Suit::Club; suit < Card::Suit::MAX_SUIT; suit = ++suit)
    {
        for(quint8 dignity = Card::Dignity::six; dignity < Card::Dignity::MAX_DIGNITY; ++dignity)
        {
            newDeck.push_back(new Card(Card::Suit(suit), Card::Dignity(dignity), cardParent));
        }
    }

    std::random_shuffle(newDeck.begin(), newDeck.end());
    return newDeck;
}
