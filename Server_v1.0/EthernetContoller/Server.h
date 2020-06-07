#ifndef MYTESTSERVER_H
#define MYTESTSERVER_H

#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>
#include <algorithm>
#include "DataBase/DataBase.h"
#include "../GameInstance/GameInstance.h"

class Server : public QWidget
{
    Q_OBJECT
private:
    DataBase* m_registrationDataBase;

    QTcpServer* m_tcpServer;
    QTextEdit* m_textPlace;

    qint16 m_userID;

    QList<Player*> m_waitingPlayers;

public:
    explicit Server(QWidget *parent = nullptr);

public slots:
    virtual void slotNewConnection();

    void slotPlayerWantPlay(Player*, quint8, quint8, bool);
    void tryDoNewGameInstance();


    void slotPlayerLogIn(const QString&, const QString&);
    void slotPlayerRegistration(const QString&, const QString&);

    void slotPlayerDepositMoney(qint16);
    void slotPlayerWithdrawMoney(qint16);


    void printText(const QString&);
};

#endif // MYTESTSERVER_H
