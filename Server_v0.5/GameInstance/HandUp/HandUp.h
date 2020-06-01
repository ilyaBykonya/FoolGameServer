#ifndef HANDUP_H
#define HANDUP_H

#include <QObject>

class HandUp : public QObject
{
    Q_OBJECT
public:
    explicit HandUp(QObject *parent = nullptr);

signals:

};

#endif // HANDUP_H
