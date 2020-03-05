#include "producer.h"
#include <QtCore>
#include <QDebug>


producer::producer(QMutex *pro_mut, QQueue<int> *produce_que, QSemaphore *use, QSemaphore *fr)
{
    mut=pro_mut;
    this_q=produce_que;
    used=use;
    free=fr;
}

void producer::run(){

    while(true){

        mut->lock();

        if(free->available()<10){

            this_q->enqueue(1);

            qDebug()<<"Producer "+this->name +"produced "+" available data: "+ QString::number(this_q->size());

            sleep(1);
            free->release();



        }
        else{

            qDebug()<<"Box is full";
            mut->unlock();
            sleep(3);

            continue;
        }


        mut->unlock();
    }

}
