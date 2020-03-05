#include "customer.h"
#include <QtCore>
#include <QDebug>


customer::customer(QMutex *mu, QQueue<int> *que, QSemaphore *use, QSemaphore *fr)
{
    mut=mu;
    this_q=que;
    used=use;
    free=fr;
}

void customer::run(){

   while(true){

       mut->lock();

       if(free->available()>0){

           this_q->dequeue();

           qDebug()<<"Consumer "+this->name +"consumed "+" avaliable data: "+QString::number(this_q->size());

           sleep(1);
           free->acquire();


       }
       else{

           qDebug()<<"Box is empty";
           mut->unlock();
           sleep(3);
           continue;
       }



       mut->unlock();

       sleep(3);
   }
}
