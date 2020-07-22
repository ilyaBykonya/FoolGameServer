#ifndef CHATROOM_H
#define CHATROOM_H
#include "Player/Player.h"

class ChatRoom : public QObject
{
    Q_OBJECT
private:
    Canal m_chatCanal;

public:
    explicit ChatRoom(Canal, QObject *parent = nullptr);

public slots:
    void pushUser(Player*);
    void popUser(Player*);

protected slots:
    void sendMessage(Player*, Canal, const QString&);

signals:
    void userSendMessage(Player*, Canal, const QString&);
};

#endif // CHATROOM_H
