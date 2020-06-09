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
        Heart,//черва
        Peak,//пика
        Tambourine,//бубна
        Club,//трефа(крести)
        MAX_SUIT
    };
    enum Dignity
    {
        two = 0,
        three,
        four,
        five,
        six,
        seven,
        eight,
        nine,
        ten,
        jack,
        lady,
        king,
        ace,
        MAX_DIGNITY
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
