#include "HandUp.h"

HandUp::HandUp(QObject *parent)
    :QObject{ parent }
    {
    }


quint8 HandUp::handUpSize() const { return m_handUp.size(); }
void HandUp::putCard(Card* card)
{
    m_handUp.push_back(card);
}
