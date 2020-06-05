#ifndef CARD_H
#define CARD_H
#include <QObject>
//Очередь игрока.
enum QueueMove
{
    Attacker = 0,
    Defender = 1,
    Other = 2
};
//тип действия игрока
enum PlayerActionType
{
    Toss = 0,
    Beat,
    Trancferable
};


class Card: public QObject
{
    Q_OBJECT
public:
    enum Suit
    {
        Club = 0,//трефа(крести)
        Heart = 1,//черва
        Tambourine = 2,//бубна
        Peak = 3,//пика
        MAX_SUIT = 4
    };
    enum Dignity
    {
        six = 0,
        seven = 1,
        eight = 2,
        nine = 3,
        ten = 4,
        jack = 5,
        lady = 6,
        king = 7,
        ace = 8,
        MAX_DIGNITY = 9
    };
    enum LocationState
    {
        InStartDeck = 0,
        FirstCardInStartDeck = 1,
        InPlayerDeck = 2,
        InBattlePlace = 3,
        InHangUp = 4
    };

private:
    Dignity m_dignity;
    Suit m_suit;

    LocationState m_location;
public:
    //![конструкторы]
    Card(Suit s, Dignity dig, QObject* parent);
    Card(const Card&) = delete;
    Card(Card&&) = delete;
    //!
    Dignity dignity() const;
    Suit suit() const;

    LocationState location() const;
    void setNewLocation(LocationState);
};

#endif // CARD_H
