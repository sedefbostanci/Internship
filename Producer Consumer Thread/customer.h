#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <QtCore>
#include <QThread>
#include <QQueue>

class customer : public QThread
{
public:
    customer(QMutex *mu,QQueue<int> *que,QSemaphore *use,QSemaphore *fr);
    void run();
    QString name;
    QMutex *mut;
    QQueue<int> *this_q;
    QSemaphore *used;
    QSemaphore *free;
};

#endif // CUSTOMER_H
