#ifndef PLAYERDECK_H
#define PLAYERDECK_H

#include <QObject>

class PlayerDeck : public QObject
{
    Q_OBJECT
public:
    explicit PlayerDeck(QObject *parent = nullptr);

signals:

};

#endif // PLAYERDECK_H
