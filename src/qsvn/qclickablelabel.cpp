#include "qclickablelabel.h"

QClickableLabel::QClickableLabel(QWidget *parent) :
    QLabel(parent)
{
}

void QClickableLabel::mouseReleaseEvent(QMouseEvent * event)
{
    Q_UNUSED(event);

    emit clicked();
}
