#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtNetwork>
#include <QUdpSocket>

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
    ~MainWindow();

signals:
    void dataReceived(QByteArray);

private slots:
    void newConnection();
    void disconnected();
    void readyRead();
    int checkLogin(m_struct m, QTcpSocket *socket);
    int updateIp(QString ip, QTcpSocket *socket, QString name);
    int findIpofName(QTcpSocket *mySocket);
    void findSockets(QTcpSocket *mysocket);

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QHash<QTcpSocket*, QByteArray*> buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes;
    QMap<QTcpSocket*, QByteArray*> activeUsers;
    QSet<QString> Username;
    QStringList privateChat;
    QStringList privateChatIp;
    QUdpSocket *active;
    QString name;
    //QHash<QTcpSocket*, m_struct*> users;
    //We need to store the size to verify if a block has received completely
};


#endif // MAINWINDOW_H
