#ifndef BATTLEAREA_H
#define BATTLEAREA_H

#include <QObject>

class BattleArea : public QObject
{
    Q_OBJECT
public:
    explicit BattleArea(QObject *parent = nullptr);

signals:

};

#endif // BATTLEAREA_H
