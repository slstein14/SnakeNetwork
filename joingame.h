#ifndef JOINGAME_H
#define JOINGAME_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include "renderobject.h"
#include <QPixmap>
#include <QTimer>
#include <QKeyEvent>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <vector>
#include <QGenericMatrix>
#include <vector>
#include "highscores.h"
#include "network2player.h"
using namespace std;

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

    int matrix[48][64];
    RenderObject *player1;
    RenderObject *player2;
    RenderObject* wall;
    RenderObject* apple;
    vector<RenderObject*>walls;
    vector<RenderObject*>segments1;
    vector<RenderObject*>segments2;
    int direction1;
    bool newDirection1;
    int direction2;
    bool newDirection2;
    QMessageBox *msg;
    QMessageBox *pbox;
    QPixmap *snakeImage1;
    QPixmap *snakeImage2;
    QPixmap *wallImage;
    QPixmap *appleImage;

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
    void updateField();
};

#endif // JOINGAME_H
