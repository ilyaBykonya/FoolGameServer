#include "ChatRoom.h"

ChatRoom::ChatRoom(Canal chatCanal, QObject* parent)
    :QObject{ parent },
     m_chatCanal{ chatCanal }
    {
    }


void ChatRoom::pushUser(Player* user)
{
    if(user != nullptr)
    {
        QObject::connect(user, &Player::messageSignalUserSendMessage, this, &ChatRoom::sendMessage, Qt::ConnectionType::UniqueConnection);
        QObject::connect(this, &ChatRoom::userSendMessage, user, &Player::messageSlotReceiveUserMessage, Qt::ConnectionType::UniqueConnection);
    }
}
void ChatRoom::popUser(Player* user)
{
    if(user != nullptr)
    {
        QObject::disconnect(user, &Player::messageSignalUserSendMessage, this, &ChatRoom::sendMessage);
        QObject::disconnect(this, &ChatRoom::userSendMessage, user, &Player::messageSlotReceiveUserMessage);
    }
}


void ChatRoom::sendMessage(Player* user, Canal canal, const QString& message)
{
    if(canal == m_chatCanal)
    {
        emit this->userSendMessage(user, m_chatCanal, message);
    }
}
