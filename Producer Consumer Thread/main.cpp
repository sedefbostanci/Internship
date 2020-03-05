#include <QCoreApplication>
#include <customer.h>
#include <producer.h>

#include <QList>

int main(int argc, char *argv[])
{
    QQueue<int> q;
    QMutex mut;
    QSemaphore free;
    QSemaphore used;
    QCoreApplication a(argc, argv);

    customer custom1(&mut,&q,&used,&free);
    custom1.name="1";
    customer custom2(&mut,&q,&used,&free);
    custom2.name="2";
    customer custom3(&mut,&q,&used,&free);
    custom3.name="3";
    customer custom4(&mut,&q,&used,&free);
    custom4.name="4";
    customer custom5(&mut,&q,&used,&free);
    custom5.name="5";
    customer custom6(&mut,&q,&used,&free);
    custom6.name="6";

    producer producer1(&mut,&q,&used,&free);
    producer1.name="1";
    producer producer2(&mut,&q,&used,&free);
    producer2.name="2";
    producer producer3(&mut,&q,&used,&free);
    producer3.name="3";

    producer producer4(&mut,&q,&used,&free);
    producer4.name="4";
    producer producer5(&mut,&q,&used,&free);
    producer5.name="5";
    producer producer6(&mut,&q,&used,&free);
    producer6.name="6";

    producer1.start();
    producer2.start();
    producer3.start();
    producer4.start();
    producer5.start();
    producer6.start();
    custom1.start();
    custom2.start();
    custom3.start();
    custom4.start();
    custom5.start();
    custom6.start();

    return a.exec();
}
