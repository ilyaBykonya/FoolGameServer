#include <QApplication>
#include "EthernetContoller/Server.h"


int main(int argc, char *argv[])
{
    srand(time(0));
    QApplication a(argc, argv);
    Server serv;
    serv.show();
    return a.exec();
}
