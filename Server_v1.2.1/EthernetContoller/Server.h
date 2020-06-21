#ifndef MYTESTSERVER_H
#define MYTESTSERVER_H
#include <QLabel>
#include <algorithm>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QMessageBox>
#include "DataBase/DataBase.h"
#include "WaitingRoom/WaitingRoom.h"
#include "../GameInstance/GameInstance.h"

class Server : public QWidget
{
    Q_OBJECT
private:
    DataBase* m_registrationDataBase;
    QTcpServer* m_tcpServer;

    QTextEdit* m_textPlace;
    QLineEdit* m_inputPlace;

    UserID m_userID;

    QList<WaitingRoom*> m_waitingPlayers;
    ChatRoom* m_sharedChat;

public:
    explicit Server(QWidget *parent = nullptr);

public slots:
    virtual void slotNewConnection();

    void slotPlayerWantPlay(Player*, quint8, quint8, bool);
    void doNewGameInstance(WaitingRoom*);


    void slotPlayerLogIn(const QString&, const QString&);
    void slotPlayerRegistration(const QString&, const QString&);

    void slotPlayerDepositMoney(Cash);
    void slotPlayerWithdrawMoney(Cash);

signals:
    void sendServerMessage(const QString&);
};

#endif // MYTESTSERVER_H
