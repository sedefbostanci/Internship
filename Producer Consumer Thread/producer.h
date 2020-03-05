#ifndef PRODUCER_H
#define PRODUCER_H
#include <QtCore>
#include <QThread>

class producer : public QThread
{
public:
    producer(QMutex *pro_mut,QQueue<int> *produce_que,QSemaphore *use,QSemaphore *fr);
    void run();
    QString name;
    QMutex *mut;
    QQueue<int> *this_q;
    QSemaphore *free;
    QSemaphore *used;
};

#endif // PRODUCER_H
