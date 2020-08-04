#include <QApplication>
#include "EthernetContoller/Server.h"


int main(int argc, char *argv[])
{
    srand(time(0));
    QApplication a(argc, argv);
//    quint16 portNumber;
//    QFile readConnectToHostInfoFile("../ApplicationData/host_address.txt");
//    if(!readConnectToHostInfoFile.exists())
//    {
//        QMessageBox::critical(nullptr, "Error", "Файл host_address.txt не найден.");
//        return -1;
//    }
//    if(!readConnectToHostInfoFile.open(QFile::OpenModeFlag::ReadOnly))
//    {
//        QMessageBox::critical(nullptr, "Error", "Файл host_address.txt не может быть открыт.");
//        return -1;
//    }
//    QTextStream readConnectToHostInfoStream(&readConnectToHostInfoFile);
//    readConnectToHostInfoStream >> portNumber;

    Server serv(/*portNumber*/2323);
    serv.show();
    return a.exec();
}
