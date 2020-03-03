#include "privatechatpage.h"
#include "ui_privatechatpage.h"

privateChatPage::privateChatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::privateChatPage)
{
    ui->setupUi(this);

    connect(this,SIGNAL(sendPrivateMessage(QString)),this,SLOT(sendPrivateMess(QString)));
}

privateChatPage::~privateChatPage()
{

    delete ui;
}

void privateChatPage::on_send_button_clicked()
{
    qDebug()<<"sendbutton";

    QString message = ui->input_text->toPlainText();

    emit sendPrivateMessage(message);

}

void privateChatPage::sendPrivateMess(QString mess){

    ui->message_box_widget->append(mess);
}

QString privateChatPage::getText(){

    return ui->input_text->toPlainText();
}
