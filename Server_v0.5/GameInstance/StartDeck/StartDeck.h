#ifndef STARTDECK_H
#define STARTDECK_H

#include <QObject>

class StartDeck : public QObject
{
    Q_OBJECT
public:
    explicit StartDeck(QObject *parent = nullptr);

signals:

};

#endif // STARTDECK_H
