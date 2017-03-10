#ifndef JOINGAME_H
#define JOINGAME_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include "Network2Player.h"

namespace Ui {
class JoinGame;
}

class JoinGame : public QWidget
{
    Q_OBJECT

public:
    explicit JoinGame(QWidget *parent = 0);
    ~JoinGame();
    QString nickname;
    QString hostIP;
    void setHostIP(QString address);
    void closeEvent(QCloseEvent *e);

private slots:
    void on_pushButton_clicked();
    void newConnection();
    void Disconnected();
    void readyRead();

private:
    Ui::JoinGame *ui;
    QTcpSocket *socket;
    QTcpServer *server;
    Network2Player* game2;
};

#endif // JOINGAME_H
