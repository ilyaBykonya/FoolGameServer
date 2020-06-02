#include "Server.h"

Server::Server(QWidget *parent)
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
        QObject::connect(m_tcpServer, &QTcpServer::newConnection, this, &Server::slotNewConnection);

        m_textPlace = new QTextEdit();
        m_textPlace->setReadOnly(true);



        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(new QLabel("<center><sup><i>SERVER</i></sup></center>"));
        layout->addWidget(m_textPlace);
        this->setLayout(layout);
    }



void Server::slotNewConnection()
{
    Player* player = new Player(m_userID,  m_tcpServer->nextPendingConnection(), this);
    QObject::connect(player, &Player::applicationSignalUserWantPlay, this, &Server::slotPlayerWantPlay);

    this->m_textPlace->append(QString("New connection [%1]").arg(m_userID));
    ++m_userID;
}

void Server::slotPlayerWantPlay(Player* player, quint8 deckTypeFlags, quint8 playersCountFlags, bool realCash)
{
    auto it = std::find(m_waitingPlayers.begin(), m_waitingPlayers.end(), player);
    if(it != m_waitingPlayers.end())
        return;

    m_waitingPlayers.push_back(player);
    tryDoNewGameInstance();
}
void Server::tryDoNewGameInstance()
{
    if(m_waitingPlayers.size() > 1)
    {
        new GameInstance(m_waitingPlayers, this);
        m_waitingPlayers.clear();
    }
}

void Server::slotPlayerRegistration(const QString& name, const QString& password)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        player->serverSlotAlertMessage(QString("You registration with atributes:"),
                                       QString(QString("Name: %1").arg(name) +
                                               QString("\nPassword: %1").arg(password) +
                                               QString("\nID: %1").arg(player->id())));
}
void Server::slotPlayerDepositMoney(qint16 money)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        player->serverSlotAlertMessage(QString("You deposit money:"),
                                       QString(QString("Cash: %1").arg(money)));
}
void Server::slotPlayerWithdrawMoney(qint16 money)
{
    Player* player = qobject_cast<Player*>(this->sender());
    if(player)
        player->serverSlotAlertMessage(QString("You withdraw money:"),
                                       QString(QString("Cash: %1").arg(money)));
}

void Server::printText(const QString& str)
{
    this->m_textPlace->append(str);
}

