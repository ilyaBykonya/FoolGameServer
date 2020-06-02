#include <QApplication>
#include "EthernetContoller/Server.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server serv;
    serv.show();
    return a.exec();
}
