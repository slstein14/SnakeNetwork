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
    QTcpSocket *socket;
    QTcpServer *server;

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

    bool p2connect;
    bool gameStarted;


private slots:
    void on_pushButton_clicked();
    void newConnection();
    void Disconnected();
    void readyRead();
    void updateField();


private:
    Ui::HostGame *ui;
    void moveSnake();
    void moveApple();

};

#endif // HOSTGAME_H
