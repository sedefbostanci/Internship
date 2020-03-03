#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtXml>
#include <QNetworkInterface>

static inline qint32 ArrayToInt(QByteArray source);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), SLOT(newConnection()));

    qDebug() << "Listening:" << server->listen(QHostAddress::Any, 1024);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newConnection()
{
    while (server->hasPendingConnections())
    {
        QTcpSocket *socket = server->nextPendingConnection();

        connect(socket, SIGNAL(readyRead()), SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));

        QByteArray *buffer = new QByteArray();
        qint32 *s = new qint32(0);
        QByteArray *buf=new QByteArray();

        buffers.insert(socket, buffer);

        sizes.insert(socket, s);

        activeUsers.insert(socket,buf);


    }
}

void MainWindow::disconnected()
{

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    QByteArray buf;
    QByteArray *userNames=activeUsers.value(socket);
    QStringList names;

    QMap<QTcpSocket*,QByteArray*>::iterator x;
    QMap<QTcpSocket*,QByteArray*>::iterator z;

    for (z = activeUsers.begin(); z != activeUsers.end(); ++z){

        names<<z.value()->data();

    }

    if(names.count(userNames->data())==1){
        Username.remove(userNames->data());
    }



    QDataStream data(&buf,QIODevice::WriteOnly);
    qint32 identifier=5;


    data<<identifier;

    QSet<QString>::iterator y;
    for (y = Username.begin(); y != Username.end(); ++y){
        data<<y->toUtf8();

    }


    socket->deleteLater();
    delete buffer;
    delete s;
    delete userNames;
    activeUsers.remove(socket);

    QMap<QTcpSocket*, QByteArray*>::iterator i;

    for (i = activeUsers.begin(); i != activeUsers.end(); ++i){

        if(i.value()->isEmpty()==false){

            i.key()->write(buf);
        }

    }
}

void MainWindow::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    QByteArray *userNameIp=activeUsers.value(socket);

    qint32 *s = sizes.value(socket);
    qint32 size = *s;

    int a=0;
    m_struct receive;
    QByteArray user;
    QByteArray password;

    QString otherName,myName;

    qint32 identifier;
    QString ip;

    QDataStream c(userNameIp,QIODevice::WriteOnly);

    while (socket->bytesAvailable() > 0)
    {
        buffer->append(socket->readAll());

        identifier=ArrayToInt(buffer->mid(0, 4));

        buffer->remove(0, 4);

        if(identifier==2){

            while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size))
            {
                if (size == 0 && buffer->size() >= 4)
                {
                    size = ArrayToInt(buffer->mid(0, 4));
                    *s = size;
                    buffer->remove(0, 4);
                }

                for(int i=0;i<buffer->size();i++){
                    if(buffer->at(i) != NULL){

                        user.append(buffer->at(i));
                    }
                    else{

                        receive.username=user;

                        a=i;

                        break;
                    }
                }


                for(int y=a+4;y<buffer->size();y++){
                    password.append(buffer->at(y));
                }


                receive.password=password;


                if (size > 0 && buffer->size() >= size)
                {

                    QByteArray data = buffer->mid(0, size);
                    buffer->remove(0, size);
                    size = 0;
                    *s = size;
                    emit dataReceived(data);
                 }
            }


            int flag=checkLogin(receive,socket);

            if(flag==2){

                 c<<receive.username.toUtf8();
            }

        }
        else if(identifier==3){

            if (size == 0 && buffer->size() >= 4)
            {
                size = ArrayToInt(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }


            ip=buffer->data();


            if (size > 0 && buffer->size() >= size)
            {

                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
                emit dataReceived(data);
            }

            QByteArray *hash=activeUsers.value(socket);
            hash->remove(0,4);
            Username<<hash->data();
            updateIp(ip,socket,hash->data());


        }
        else if(identifier==5){

            int count=0;

            while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)){

                if (size == 0 && buffer->size() >= 4)
                {
                    size = ArrayToInt(buffer->mid(0, 4));
                    *s = size;
                    buffer->remove(0, 4);

                }

                if (size > 0 && buffer->size() >= size)
                {

                    QByteArray data = buffer->mid(0, size);
                    privateChat.insert(count,data);
                    buffer->remove(0, size);
                    size = 0;
                    *s = size;
                    emit dataReceived(data);
                }

                count++;
            }



            findIpofName(socket);
        }

    }


}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

int MainWindow::checkLogin(m_struct m,QTcpSocket *socket){


    int counter=0;
    qint32 identifier=4;
    QByteArray buf;
    QByteArray message;
    QDataStream msg(&message,QIODevice::WriteOnly);
    QDataStream s(&buf,QIODevice::WriteOnly);



    s<<identifier;

    QDomDocument document;

        QFile file("C:/Users/Admin/Documents/tcpServerDeneme/kullanıcılar.xml");

        if(!file.open(QIODevice::ReadOnly)){

            qDebug()<<"failed to open file";
            return -1;
        }
        else{

            if(!document.setContent(&file)){

                qDebug()<<"failed to load ";
                return -1;
            }

            file.close();
        }


        QDomElement root=document.firstChildElement();

        QDomNodeList items=root.elementsByTagName("user");


        for(int i=0;i<items.count();i++){

            QDomNode itemnode =items.at(i);

            if(itemnode.isElement()){

                QDomElement itemele = itemnode.toElement();

                if(itemele.attribute("name")==m.username){

                    if(itemele.attribute("password")==m.password){

                        socket->write(buf);
                        return 2;

                    }
                    else{

                        qint32 identifier=2;
                        msg<<identifier;
                        msg<<"Incorrect password";
                        socket->write(message);
                    }
                }
                else{
                    counter++;
                }


            }
        }

        if(counter==items.count()){

            qint32 identifier=3;
            msg<<identifier;
            msg<<"Incorrect username or password";
            socket->write(message);

        }

        if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            qDebug("Basically, now we lost content of a file");
            return -1;
        }

        QByteArray xml = document.toByteArray();
        file.write(xml);
        file.close();
}

int MainWindow::updateIp(QString ip, QTcpSocket *socket, QString name){

    QByteArray buf;
    QDataStream s(&buf,QIODevice::WriteOnly);
    qint32 identifier=5;



    s<<identifier;

    QSet<QString>::iterator x;
    for (x = Username.begin(); x != Username.end(); ++x){
        s<<x->toUtf8();

    }


    QDomDocument document;

        QFile file("C:/Users/Admin/Documents/tcpServerDeneme/kullanıcılar.xml");

        if(!file.open(QIODevice::ReadOnly)){

            qDebug()<<"failed to open file";
            return -1;
        }
        else{

            if(!document.setContent(&file)){

                qDebug()<<"failed to load ";
                return -1;
            }

            file.close();
        }


        QDomElement root=document.firstChildElement();

        QDomNodeList items=root.elementsByTagName("user");

        for(int i=0;i<items.count();i++){

            QDomNode itemnode =items.at(i);

            if(itemnode.isElement()){

                QDomElement itemele = itemnode.toElement();

                if(itemele.attribute("name")==name){

                        itemele.setAttribute("ip",ip);

                        QMap<QTcpSocket*, QByteArray*>::iterator i;

                        for (i = activeUsers.begin(); i != activeUsers.end(); ++i){

                            if(i.value()->isEmpty()==false){
                                i.key()->write(buf);
                            }

                        }

                }

            }
        }

        if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            qDebug("Basically, now we lost content of a file");
            return -1;
        }

        QByteArray xml = document.toByteArray();
        file.write(xml);
        file.close();
}

int MainWindow::findIpofName(QTcpSocket *mySocket){

    QString other_ip;
    QString my_ip;


    QDomDocument document;

        QFile file("C:/Users/Admin/Documents/tcpServerDeneme/kullanıcılar.xml");

        if(!file.open(QIODevice::ReadOnly)){

            qDebug()<<"failed to open file";
            return -1;
        }
        else{

            if(!document.setContent(&file)){

                qDebug()<<"failed to load ";
                return -1;
            }

            file.close();
        }


        QDomElement root=document.firstChildElement();

        QDomNodeList items=root.elementsByTagName("user");

        for(int i=0;i<items.count();i++){

            QDomNode itemnode =items.at(i);

            if(itemnode.isElement()){

                QDomElement itemele = itemnode.toElement();

                if(itemele.attribute("name")==privateChat.at(0)){

                     other_ip=itemele.attribute("ip");

                }
                else if(itemele.attribute("name")==privateChat.at(1)){
                    qDebug()<<"kkk "<<itemele.attribute("ip");
                    my_ip=itemele.attribute("ip");

                }

            }
        }

        privateChatIp.insert(0,my_ip);
        privateChatIp.insert(1,other_ip);
        qDebug()<<my_ip<<" "<<other_ip;

        findSockets(mySocket);
}

void MainWindow::findSockets(QTcpSocket *mysocket){

    QByteArray my,other;
    QDataStream s(&my,QIODevice::WriteOnly);
    QDataStream m(&other,QIODevice::WriteOnly);
    qint32 identifier=7;
    qint32 identifier_other=8;
    QTcpSocket *otherSocket;


    s<<identifier;
    m<<identifier_other;


    QMap<QTcpSocket*, QByteArray*>::iterator i;

    for (i = activeUsers.begin(); i != activeUsers.end(); ++i){

        if(i.value()->data()==privateChat.at(0)){
            otherSocket=i.key();
        }
    }

    s<<privateChatIp.at(1).toUtf8()<<privateChat.at(2).toUtf8();
    mysocket->write(my);

    m<<privateChat.at(1).toUtf8()<<privateChatIp.at(0).toUtf8()<<privateChat.at(2).toUtf8();
    otherSocket->write(other);
}
