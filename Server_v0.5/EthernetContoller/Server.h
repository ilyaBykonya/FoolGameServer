#ifndef MYTESTSERVER_H
#define MYTESTSERVER_H

#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>
#include <algorithm>
#include "TestInstance.h"
#include "MyPlayer.h"
#include "Card.h"

class MyTestServer : public QWidget
{
    Q_OBJECT
private:
    QTcpServer* m_tcpServer;
    QTextEdit* m_textPlace;

    qint16 m_userID;

    QList<MyPlayer*> m_waitingPlayers;

public:
    explicit MyTestServer(QWidget *parent = nullptr);

public slots:
    virtual void slotNewConnection();
    void slotPlayerWantPlay(MyPlayer*, quint8, quint8, bool);

    void tryDoNewGameInstance();
    void printText(const QString&);
};

#endif // MYTESTSERVER_H
