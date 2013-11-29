#include "qextcombobox.h"
#include <qevent.h>


QExtComboBox::QExtComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void QExtComboBox::keyPressEvent(QKeyEvent *e)
{
    QComboBox::keyPressEvent(e);

    if ((e->key() == Qt::Key_Return)||(e->key() == Qt::Key_Enter))
    {
        emit push(currentText());
    }
}
