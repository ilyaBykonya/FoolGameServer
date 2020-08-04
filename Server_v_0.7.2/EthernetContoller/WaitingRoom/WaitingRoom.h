#ifndef WAITINGROOM_H
#define WAITINGROOM_H
#include <QTimer>
#include <QObject>
#include <QList>
#include "Player/Player.h"
#include "Player/SettingsStruct/SettingsStruct.h"

#include <QTimer>
#include <QObject>
#include <QList>
#include "Player/Player.h"

class WaitingRoom : public QObject
{
    Q_OBJECT
private:
    SettingsStruct m_settings;
    QList<Player*> m_pendingPlayersList;

    QTimer* m_roundStartTimer;
public:
    explicit WaitingRoom(QObject *parent = nullptr);
    bool possibleNewPlayer(Player*);

    const SettingsStruct& waitingRoomSettings() const;
    QList<Player*> pendingPlayersList();

protected slots:
    void checkFullRoom();
    void forceExitRoom();

    void userExitFromRoom(Player*);
    void userDisconnected();

signals:
    void signalRoomIsFull(WaitingRoom*);
};

#endif // WAITINGROOM_H
