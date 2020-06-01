#ifndef BATTLEPAIR_H
#define BATTLEPAIR_H

#include <QObject>

class BattlePair : public QObject
{
    Q_OBJECT
public:
    explicit BattlePair(QObject *parent = nullptr);

signals:

};

#endif // BATTLEPAIR_H
