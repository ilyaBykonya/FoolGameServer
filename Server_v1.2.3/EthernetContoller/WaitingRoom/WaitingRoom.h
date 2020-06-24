#ifndef WAITINGROOM_H
#define WAITINGROOM_H
#include <QTimer>
#include <QObject>
#include <QList>
#include "Player/Player.h"

class WaitingRoom : public QObject
{
    Q_OBJECT
private:
    quint8 m_amountOfPlayers;
    quint8 m_deckSize;
    bool m_trancferableAbility;
    QList<Player*> m_pendingPlayersList;

    QTimer* m_roundStartTimer;
public:
    explicit WaitingRoom(QObject *parent = nullptr);
    bool possibleNewPlayer(Player*);

    quint8 amountOfPlayers() const;
    quint8 deckSize() const;
    bool trancferableAbility() const;
    QList<Player*> pendingPlayersList();
protected:
    void checkFullRoom();

protected slots:
    void forceRoundStart();

signals:
    void roomIsFull(WaitingRoom*);

};

#endif // WAITINGROOM_H