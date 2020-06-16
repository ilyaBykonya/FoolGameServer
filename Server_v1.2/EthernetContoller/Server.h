#ifndef MYTESTSERVER_H
#define MYTESTSERVER_H

#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>
#include <algorithm>
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

    qint16 m_userID;

    QList<WaitingRoom*> m_waitingPlayers;

public:
    explicit Server(QWidget *parent = nullptr);

public slots:
    virtual void slotNewConnection();

    void slotPlayerWantPlay(Player*, quint8, quint8, bool);
    void doNewGameInstance(WaitingRoom*);


    void slotPlayerLogIn(const QString&, const QString&);
    void slotPlayerRegistration(const QString&, const QString&);

    void slotPlayerDepositMoney(qint16);
    void slotPlayerWithdrawMoney(qint16);

signals:
    void messageSignalSendUserMessage(quint16, Player::Canal, const QString&);
};

#endif // MYTESTSERVER_H
