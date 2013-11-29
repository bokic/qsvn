#ifndef QEXTCOMBOBOX_H
#define QEXTCOMBOBOX_H

#include <QComboBox>
#include <QEvent>


class QExtComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit QExtComboBox(QWidget *parent = 0);

signals:
    void push(QString text);

public slots:

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // QEXTCOMBOBOX_H
