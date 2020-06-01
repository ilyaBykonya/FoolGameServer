#include "MyTestServer.h"

MyTestServer::MyTestServer(QWidget *parent)
    :QWidget{ parent },
     m_tcpServer{ new QTcpServer(this) },
     m_userID{ 0 }
    {
        if(!m_tcpServer->listen(QHostAddress::SpecialAddress::Any, 2323))
        {
            QMessageBox::critical(this, "Port error", QString("Port: ") + QString::number(2323) + "\n" + m_tcpServer->errorString());
            m_tcpServer->close();

            return;
        }
        QObject::connect(m_tcpServer, &QTcpServer::newConnection, this, &MyTestServer::slotNewConnection);

        m_textPlace = new QTextEdit();
        m_textPlace->setReadOnly(true);



        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(new QLabel("<center><sup><i>SERVER</i></sup></center>"));
        layout->addWidget(m_textPlace);
        this->setLayout(layout);
    }



void MyTestServer::slotNewConnection()
{
    MyPlayer* player = new MyPlayer(m_userID,  m_tcpServer->nextPendingConnection(), this);
    QObject::connect(player, &MyPlayer::applicationSignalUserWantPlay, this, &MyTestServer::slotPlayerWantPlay);

    this->m_textPlace->append(QString("New connection [%1]").arg(m_userID));
    ++m_userID;
}

void MyTestServer::slotPlayerWantPlay(MyPlayer* player, quint8 deckTypeFlags, quint8 playersCountFlags, bool realCash)
{
    auto it = std::find(m_waitingPlayers.begin(), m_waitingPlayers.end(), player);
    if(it != m_waitingPlayers.end())
        return;

    m_waitingPlayers.push_back(player);
    tryDoNewGameInstance();
}

void MyTestServer::tryDoNewGameInstance()
{
    if(m_waitingPlayers.size() > 1)
    {
        new TestInstance(m_waitingPlayers, this);
        m_waitingPlayers.clear();
    }
}


void MyTestServer::printText(const QString& str)
{
    this->m_textPlace->append(str);
}

