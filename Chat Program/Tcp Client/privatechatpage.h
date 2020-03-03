#ifndef PRIVATECHATPAGE_H
#define PRIVATECHATPAGE_H

#include <QWidget>
#include <QtCore>

namespace Ui {
class privateChatPage;
}

class privateChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit privateChatPage(QWidget *parent = nullptr);
     QString getText();
    ~privateChatPage();

signals:
    void sendPrivateMessage(QString message);

private slots:
    void on_send_button_clicked();
public slots:
    void sendPrivateMess(QString mess);

private:
    Ui::privateChatPage *ui;
};


#endif // PRIVATECHATPAGE_H
