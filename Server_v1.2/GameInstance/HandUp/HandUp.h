#ifndef HANDUP_H
#define HANDUP_H

#include <QObject>
#include "../Card/Card.h"
#include <QList>

class HandUp : public QObject
{
    Q_OBJECT
private:
    QList<Card*> m_handUp;
public:
    explicit HandUp(QObject *parent = nullptr);

    quint8 handUpSize() const;
    void putCard(Card*);
};

#endif // HANDUP_H
