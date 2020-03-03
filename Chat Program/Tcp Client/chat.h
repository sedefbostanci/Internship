#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include <QUdpSocket>
#include <QListWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <privatechatpage.h>

namespace Ui {
class chat;
}

class chat : public QDialog
{
    Q_OBJECT

public:
    explicit chat(QWidget *parent = nullptr);
    ~chat();
    void setText(const QString &text_);
    void cleanText();
    //QString getText();

private slots:
       void readPendingDatagrams();
       void on_send_button_clicked();
       void on_listWidget_itemDoubleClicked(QListWidgetItem *item);      
       void getPrivateMess(QString message);

       //void onReadyRead();

public slots:
      void get_Name(QString a);
      void openTabWidget(QString name);


signals:
      void sendNameToServer(QString otherName,QString myName);
      void sendToMain(QString message,QString name);

private:
    Ui::chat *ui;
    QUdpSocket *globalChatSocket;
    QUdpSocket *globalChatSocketc;
    QString name;
    privateChatPage h;
    QString clientName;
    //QTcpServer *privateChat;
    //QTcpSocket *privateChat;
    //QTcpSocket *privateChatRead;
    //MainWindow *main;

};

#endif // CHAT_H
