#include "mainwindow.h"
#include "ui_mainwindow.h"

//static inline QByteArray IntToArray(qint32 source);
static inline qint32 ArrayToInt(QByteArray source);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    socket = new QTcpSocket(this);

    int x=connectToHost("192.168.137.1");
    qDebug()<<"aa "<<x;
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(this,SIGNAL(send_name(QString)),&c,SLOT(get_Name(QString)));
    connect(&c,SIGNAL(sendNameToServer(QString,QString)),this,SLOT(writeNameToServer(QString,QString)));
    connect(this,SIGNAL(privateMessageCome(QString)),&c,SLOT(openTabWidget(QString)));
    connect(&c,SIGNAL(sendToMain(QString,QString)),this,SLOT(getPrivateMessage(QString,QString)));


}

MainWindow::~MainWindow()
{

    delete ui;
}


void MainWindow::on_pushButton_clicked()
{


    QString userName=ui->lineEdit->text();
    QString password=ui->lineEdit_2->text();

    m_struct msg;
    msg.username=userName;
    msg.password=password;

    QByteArray buf;
    QDataStream s(&buf, QIODevice::WriteOnly);

    qint32 identifier;
    identifier=2;

    s<<identifier<<msg.username.toUtf8()<<msg.password.toUtf8();

    writeData(buf);
    emit send_name(userName);

}


void MainWindow::onReadyRead()
{

    qint32 size = 0;

    QByteArray buf;
    QDataStream s(&buf, QIODevice::WriteOnly);


    QString ip;
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);

    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {

        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)

            ip=address.toString();
    }

    QByteArray buffer = socket->readAll();
    qint32 identifier;

    identifier=ArrayToInt(buffer.mid(0, 4));

    buffer.remove(0, 4);

    if(identifier==2){

         buffer.remove(0,4);
        ui->label_3->setText(buffer.data());
         //ui->lineEdit_2->clear();
    }

    else if(identifier==3){

         buffer.remove(0,4);
         ui->label_3->setText(buffer.data());
         //ui->lineEdit->clear();
         //ui->lineEdit_2->clear();
    }


    else if(identifier==4){

        qint32 sendingidentifier;
        sendingidentifier=3;

        s<<sendingidentifier<<ip.toUtf8();
        writeData(buf);


    }
    else if (identifier==5){

        c.cleanText();

        while ((size == 0 && buffer.size() >= 4) || (size > 0 && buffer.size() >= size)) //While can process data, process it
        {

            if (size == 0 && buffer.size() >= 4) //if size of data has received completely, then store it on our global variable
            {
                size = ArrayToInt(buffer.mid(0, 4));
                buffer.remove(0, 4);
            }


            if (size > 0 && buffer.size() >= size) // If data has received completely, then emit our SIGNAL with the data
            {
                QByteArray data = buffer.mid(0, size);
                c.setText(data);
                buffer.remove(0, size);
                size = 0;

             }
        }

        c.show();
    }

    else if(identifier==7){

        int count=0;

        while ((size == 0 && buffer.size() >= 4) || (size > 0 && buffer.size() >= size)){

            if(count<2){

                if (size == 0 && buffer.size() >= 4)
                {
                    size = ArrayToInt(buffer.mid(0, 4));

                    buffer.remove(0, 4);

                }


                if (size > 0 && buffer.size() >= size)
                {

                    QByteArray data = buffer.mid(0, size);
                    privateIpPort.insert(count,data);
                    buffer.remove(0, size);
                    size = 0;


                }

                 count++;
            }

            else{
                buffer.remove(0,buffer.size());
            }

        }


    }
    else if(identifier==8){


        int count=0;

        while ((size == 0 && buffer.size() >= 4) || (size > 0 && buffer.size() >= size)){

            if(count<=2){

                if (size == 0 && buffer.size() >= 4)
                {
                    size = ArrayToInt(buffer.mid(0, 4));

                    buffer.remove(0, 4);

                }

                if (size > 0 && buffer.size() >= size)
                {

                    QByteArray data = buffer.mid(0, size);

                    IpPort.insert(count,data);
                    buffer.remove(0, size);
                    size = 0;


                }

                 count++;
            }

            else{
                buffer.remove(0,buffer.size());
            }

        }

        privateChatRead=new QTcpSocket(this);

        privateChatRead->connectToHost(QHostAddress(IpPort.at(1)),IpPort.at(2).toUShort());

        clientSocketName.insert(privateChatRead,ui->lineEdit->text());

        connect(privateChatRead, SIGNAL(readyRead()), this, SLOT(private_read()));

        serverClientFlag=2;

    }
}


bool MainWindow::connectToHost(QString host)
{

    socket->connectToHost(host, 1024);
    return socket->waitForConnected();

}

bool MainWindow::writeData(QByteArray data)
{

    if(socket->state() == QAbstractSocket::ConnectedState)
    {

        socket->write(data); //write the data itself
        return socket->waitForBytesWritten();

    }
    else
        return false;
}



qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}



void MainWindow::writeNameToServer(QString otherName,QString myName){


    QByteArray nameArray;
    QByteArray port;
    QDataStream d(&nameArray, QIODevice::WriteOnly);

    serverNameClientName.insert(myName,otherName);

    serverClientFlag=1;

    qint32 identifier;
    identifier=5;

    quint16 portNumber=5000;


    privateChat=new QTcpServer(this);


    connect(privateChat, SIGNAL(newConnection()),this, SLOT(newConnection()));


    if(portNumbers.contains(portNumber)){

        portNumber=portNumber+1;
    }

    portNumbers.insert(portNumber);

    port.setNum(portNumber);

    bool a=privateChat->listen(QHostAddress::Any,portNumber);

    qDebug()<<"listening: "<<a<<portNumber;

    d<<identifier<<otherName.toUtf8()<<myName.toUtf8()<<port;


    writeData(nameArray);
}

void MainWindow::read(){

    QTcpSocket *privateChatSocket = static_cast<QTcpSocket*>(sender());

    QByteArray buf;

    buf=privateChatSocket->readAll();

    qDebug()<<"clientdan gelen mesaj: "<<buf.data();

}

void MainWindow::newConnection()
{

    while (privateChat->hasPendingConnections())
    {
        QTcpSocket *privateChatSocket = privateChat->nextPendingConnection();

        serverClient.insert(privateChat,privateChatSocket);

        connect(privateChatSocket, SIGNAL(readyRead()), this,SLOT(read()));


    }


}

void MainWindow::private_read(){

    emit privateMessageCome(IpPort.at(0));

    QByteArray buf=privateChatRead->readAll();
    qDebug()<<"serverdan gelen: "<<buf.data();
}

void MainWindow::getPrivateMessage(QString message,QString name){

    QMap<QString,QString>::iterator z;
    QMap<QTcpSocket*,QString>::iterator x;
    QTcpSocket *client;
    int index;
    int count=0;

    if(serverClientFlag==1){

        for (z = serverNameClientName.begin(); z != serverNameClientName.end(); ++z){

            if(z.key()==ui->lineEdit->text() && z.value()==name){

                index=count;
            }

            count++;

        }

        sendMessageFromServer(message,index);
    }
    else if(serverClientFlag==2){

        for (x = clientSocketName.begin(); x != clientSocketName.end(); ++x){

            if(x.value()==ui->lineEdit->text()){

                client=x.key();
                sendMessageFromClient(message,client);
            }


        }


    }

}

void MainWindow::sendMessageFromServer(QString message,int index){

     QMap<QTcpServer*, QTcpSocket*>:: iterator z;
     int count=0;
     QTcpSocket *client;

     for (z = serverClient.begin(); z != serverClient.end(); ++z){

         if(count==index){

             client=z.value();
             privateChatRead=z.value();
             client->write(message.toUtf8());
         }
         count++;
     }



}

void MainWindow::sendMessageFromClient(QString message, QTcpSocket* client){

    client->write(message.toUtf8());
}
