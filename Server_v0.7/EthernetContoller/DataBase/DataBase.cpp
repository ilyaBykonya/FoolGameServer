#include "DataBase.h"

DataBase::DataBase(QObject *parent)
    :QObject{ parent }
    {
    }

DataBase::LogInFlags DataBase::tryLogIn(const QString& playerName, const QString& playerPassword)
{
    QFile inFile(QString("D:/C++/partsFoolGame/PlayersDB/") + playerName + ".txt");

    if(!inFile.exists())
        return DataBase::LogInFlags::UserNotRegistered;

    if(!inFile.open(QFile::OpenModeFlag::ReadOnly))
        return DataBase::LogInFlags::OtherLogInError;

    QString password;
    QTextStream inStream(&inFile);
    inStream >> password;

    if(password != playerPassword)
    {
        inFile.close();
        return DataBase::LogInFlags::WrongPassword;
    }
    inFile.close();
    return DataBase::LogInFlags::SuccesfullyLogIn;
}
DataBase::RegistrationFlags DataBase::tryRegistrationPlayer(const QString& playerName, const QString& playerPassword)
{
    QFile inFile(QString("D:/C++/partsFoolGame/PlayersDB/") + playerName + ".txt");

    if(inFile.exists())
        return DataBase::RegistrationFlags::UsernameAlreadyTaken;

    if(!inFile.open(QFile::OpenModeFlag::WriteOnly))
        return DataBase::RegistrationFlags::OtherRegistrationError;

    QTextStream inStream(&inFile);
    inStream << playerPassword;

    inFile.close();
    return DataBase::RegistrationFlags::SuccesfullyRegistration;
}

void DataBase::userDepositCash(Cash sum){}
void DataBase::userWithdrawCash(Cash sum){}
