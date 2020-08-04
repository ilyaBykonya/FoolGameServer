#ifndef MYTESTSERVER_H
#define MYTESTSERVER_H
#include <QLabel>
#include <algorithm>
#include <QTextEdit>
#include <QLineEdit>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QVBoxLayout>
#include <QMessageBox>
#include "ChatRoom/ChatRoom.h"
#include "DataBase/DataBase.h"
#include "WaitingRoom/WaitingRoom.h"
#include "../GameInstance/GameInstance.h"

class Server : public QWidget
{
    Q_OBJECT
private:
    DataBaseController* m_registrationDataBase;
    QWebSocketServer* m_tcpServer;

    QTextEdit* m_textPlace;
    QLineEdit* m_inputPlace;

    QList<WaitingRoom*> m_waitingPlayers;
    ChatRoom* m_sharedChat;

public:
    explicit Server(quint16, QWidget *parent = nullptr);

public slots:
    virtual void slotNewConnection();

    void slotPlayerWantPlay(Player*);
    void doNewGameInstance(WaitingRoom*);


    void slotPlayerLogIn(const QString&, const QString&);
    void slotPlayerRegistration(const QString&, const QString&);
    void slotLogOut(Player*);

    void slotPlayerDepositMoney(Cash);
    void slotPlayerWithdrawMoney(Cash);

signals:
    void sendServerMessage(const QString&);
};

#endif // MYTESTSERVER_H
