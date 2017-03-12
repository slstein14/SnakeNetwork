#ifndef NETWORK2PLAYER_H
#define NETWORK2PLAYER_H
#include "renderobject.h"
#include <QWidget>
#include <QPixmap>
#include <QKeyEvent>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <vector>
#include <QTimer>
#include "highscores.h"
using namespace std;


class Network2Player: public QWidget
{
    Q_OBJECT
private:
    RenderObject *player1;
    RenderObject *player2;
    RenderObject* wall;
    RenderObject* apple;
    QTimer *timer;
    HighScores* scoreSet;
    vector<RenderObject*>walls;
    vector<RenderObject*>segments1;
    vector<RenderObject*>segments2;
    int matrix[48][64];
    bool paused;
    int direction;
    QMessageBox *msg;
    QMessageBox *pbox;
    QPixmap *snakeImage1;
    QPixmap *snakeImage2;
    QPixmap *wallImage;
    QPixmap *appleImage;

public:
    Network2Player(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *evt);
    void pauseMenu();
    void gameOver(int winner);
    int getDirection();
    bool isPaused();
    void setPart(int i, int x, int y);
    void resetObjects();
    void setPlayer(int player);

public slots:
    void updateField();
};

#endif // NETWORK2PLAYER_H
