#ifndef PLAYERDECK_H
#define PLAYERDECK_H

#include <QObject>
#include <QList>
#include "../Card/Card.h"

class PlayerDeck : public QObject
{
    Q_OBJECT
private:
    QList<Card*> m_playerDeck;
    qint16 m_ID;

    QueueMove m_playerState;
    bool m_actionButtonState;

public:
    explicit PlayerDeck(qint16, QObject* = nullptr);


    void putCard(Card*);
    Card* takeCard(Card::Suit, Card::Dignity);

    QueueMove playerState() const;
    void setPlayerState(QueueMove);

    quint8 playerDeckSize() const;
    quint8 requiredCards() const;
    qint16 id() const;

    bool actionButtonState() const;
    void setActionButtonState(bool);
};

#endif // PLAYERDECK_H
