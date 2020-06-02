#ifndef NEWDECKGENERATOR_H
#define NEWDECKGENERATOR_H
#include <algorithm>
#include <QList>
#include "../../Card/Card.h"


class NewDeckGenerator
{
public:
    NewDeckGenerator() = delete;
    static QList<Card*> generateDeck(QObject*);
};

#endif // NEWDECKGENERATOR_H
