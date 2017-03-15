#ifndef HOSTGAME_H
#define HOSTGAME_H
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
#include <time.h>
using namespace std;

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
    QTcpSocket *socketp1;
    QTcpSocket *socketp2;
    QTcpServer *server;
    QList<QTcpSocket*>socket;

    QTimer *timer;
    RenderObject *player1;
    RenderObject *player2;
    RenderObject* wall;
    RenderObject* apple;
    vector<RenderObject*>walls;
    vector<RenderObject*>segments1;
    vector<RenderObject*>segments2;
    int matrix[48][64];
    bool appleEatenBy1;
    bool appleEatenBy2;
    int difficulty;
    int score1;
    int score2;
    bool paused;
    int direction1;
    int direction2;
    bool player1lost;
    bool player2lost;
    QMessageBox *msg;
    QMessageBox *pbox;
    QPixmap *snakeImage1;
    QPixmap *snakeImage2;
    QPixmap *wallImage;
    QPixmap *appleImage;

    bool p1connect;
    bool p2connect;
    bool p1ready;
    bool p2ready;
    bool gameStarted;
    bool newDirection1;
    bool newDirection2;


private slots:
    void on_pushButton_clicked();
    void newConnection();
    void p1Disconnected();
    void p2Disconnected();
    void p1readyRead();
    void p2readyRead();
    void updateField();
    void Disconnected();
    void readyRead();


private:
    Ui::HostGame *ui;
    void moveSnake();
    void moveApple();
    void resetVars();
    void startGame();

};

#endif // HOSTGAME_H
