#include "chat.h"
#include "ui_chat.h"
#include "privatechatpage.h"

chat::chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chat)
{
    ui->setupUi(this);


    globalChatSocket=new QUdpSocket(this);
    globalChatSocketc=new QUdpSocket(this);

    globalChatSocketc->bind(8001);
    globalChatSocket->connectToHost(QHostAddress("255.255.255.255"),8001);

    globalChatSocket->waitForConnected();
    bool a=globalChatSocket->isOpen();
    qDebug()<<QString::number(a);

    //h = new privateChatPage(this);
    connect(globalChatSocketc,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));
    /*if(connect(&h,SIGNAL(sendPrivateMessage(QString)),this,SLOT(getPrivateMess(QString)))){
            qDebug()<<"connect worked";
    }*/

    //emit h.sendPrivateMessage("aaaa");
    //connect(privateChat, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

chat::~chat()
{

    delete ui;
}

void chat::setText(const QString &text_){

    ui->listWidget->addItem(text_);

}

void chat::readPendingDatagrams()
{

    while (globalChatSocketc->hasPendingDatagrams()) {

        QByteArray buf;

        buf.resize(globalChatSocketc->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        globalChatSocketc->readDatagram(buf.data(), buf.size());

        ui->message_box_widget->append(buf.data());
    }

}

void chat::on_send_button_clicked()
{
     //MainWindow *main=new MainWindow(this);

    //QString name=main->getText();

    QString word=name+": "+ui->input_text->toPlainText();

    QByteArray buffer;
    buffer=word.toUtf8();

    QHostAddress sender;
    quint16 senderPort;

    globalChatSocket->writeDatagram(buffer.data(),QHostAddress("255.255.255.255"),8001);

    //ui->message_box_widget->append(word.toUtf8());
}

void chat::cleanText(){

    ui->listWidget->clear();
}

void chat::get_Name(QString a){

    name=a;
}

void chat::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    openTabWidget(item->text());
    //ui->tabWidget->addTab(new privateChatPage(),item->text());


    emit sendNameToServer(item->text(),name);

}

/*void chat::onReadyRead(){

    QByteArray buffer= privateChat->readAll();
}*/

void chat::openTabWidget(QString name){

    int flag=1;

    privateChatPage *d=new privateChatPage(this);
    //d=&h;

    connect(d,SIGNAL(sendPrivateMessage(QString)),this,SLOT(getPrivateMess(QString)));

    if(ui->tabWidget->count()!=0){

        for (int k = 0; k < ui->tabWidget->count(); ++k) {
          if (ui->tabWidget->tabText(k)==name) {

            flag=0;
            break;
          }
        }
    }

    if(flag==1){
         ui->tabWidget->addTab(d,name);
    }

    clientName=name;
}

void chat::getPrivateMess(QString message){

    qDebug()<<"getPrivate";

    emit sendToMain(message,clientName);

}
