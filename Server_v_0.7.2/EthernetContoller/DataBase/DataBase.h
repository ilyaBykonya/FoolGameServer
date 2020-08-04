#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>
#include <QTextStream>
#include "Player/Player.h"


class DataBaseController : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase m_dataBase;
    QString m_errorString;
public:
    explicit DataBaseController(QObject *parent = nullptr);

    UserInformation* tryLogIn(const QString&, const QString&);
    UserInformation* tryRegistrationPlayer(const QString&, const QString&);

    bool saveUserInDataBase(Player*);

    const QString& errorText() const;

signals:
    void noConnectToDataBase(const QString&);
};



#endif // DATABASE_H
