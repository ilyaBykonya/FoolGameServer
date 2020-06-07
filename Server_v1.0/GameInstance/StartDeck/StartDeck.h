#ifndef STARTDECK_H
#define STARTDECK_H
#include <QDebug>
#include <QObject>
#include "NewDeckGenerator/NewDeckGenerator.h"

class StartDeck : public QObject
{
    Q_OBJECT
private:
    QList<Card*> m_startDeck;

public:
    explicit StartDeck(QObject *parent = nullptr);

    Card* takeCard();
    Card* firstCard();
    quint8 deckSize() const;
};

#endif // STARTDECK_H
