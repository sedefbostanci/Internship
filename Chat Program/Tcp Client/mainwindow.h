#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtNetwork>
#include <chat.h>
#include <privatechatpage.h>


struct m_struct{

    QString username;
    QString password;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    QString getText();
    ~MainWindow();


private slots:
    void on_pushButton_clicked();
    void onReadyRead();
    void read();
    void writeNameToServer(QString othername,QString myName);
    void newConnection();
     void private_read();
     void getPrivateMessage(QString messag,QString name);
    //void readActiveUsers();


public slots:
    bool connectToHost(QString host);
    bool writeData(QByteArray data);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;

    chat c;
    privateChatPage h;

    QTcpServer *privateChat;
    QTcpSocket *privateChatRead;

    QMap<QTcpServer*, QTcpSocket*> serverClient;
    QMap<QString,QString> serverNameClientName;
    QMap<QTcpSocket*,QString> clientSocketName;

    QStringList privateIpPort;
    QStringList IpPort;
    QSet<int> portNumbers;
    QStringList names;
    int serverClientFlag;

    void sendMessageFromServer(QString message,int index);
    void sendMessageFromClient(QString message,QTcpSocket *client);

signals:
    void send_name(QString a);
    void privateMessageCome(QString name);

};

#endif // MAINWINDOW_H
