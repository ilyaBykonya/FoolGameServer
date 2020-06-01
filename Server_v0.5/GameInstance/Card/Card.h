#ifndef CARD_H
#define CARD_H
#include <QObject>
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
    Card(Suit s, Dignity dig, QObject* parent);

    Dignity dignity() const { return m_dignity; }
    Suit suit() const { return m_suit; }

    LocationState location() const { return m_location; }

    bool operator<(const Card*);
public slots:
    void setNewLocation(LocationState newLocation) { m_location = newLocation; }
};

#endif // CARD_H
