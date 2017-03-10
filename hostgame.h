#ifndef HOSTGAME_H
#define HOSTGAME_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include "Network2Player.h"

namespace Ui {
class HostGame;
}

class HostGame : public QWidget
{
    Q_OBJECT

public:
    explicit HostGame(QWidget *parent = 0);
    ~HostGame();

    QString hostIP;
    void setHostIP(QString address);
    QTcpSocket *socket;
    QTcpServer *server;


private slots:
    void on_pushButton_clicked();
    void newConnection();
    void Disconnected();
    void readyRead();

private:
    Ui::HostGame *ui;
    Network2Player* game2;

    bool p2connect;
};

#endif // HOSTGAME_H
