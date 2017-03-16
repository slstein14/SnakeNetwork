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
    RenderObject *player3;
    RenderObject *player4;
    RenderObject *player5;
    RenderObject *player6;
    RenderObject *player7;
    RenderObject *player8;
    RenderObject* wall;
    RenderObject* apple;
    QTimer *timer;
    HighScores* scoreSet;
    vector<RenderObject*>walls;
    vector<RenderObject*>segments1;
    vector<RenderObject*>segments2;
    vector<RenderObject*>segments3;
    vector<RenderObject*>segments4;
    vector<RenderObject*>segments5;
    vector<RenderObject*>segments6;
    vector<RenderObject*>segments7;
    vector<RenderObject*>segments8;
    int matrix[48][64];
    int direction;
    QMessageBox *msg;
    QPixmap *snakeImage1;
    QPixmap *snakeImage2;
    QPixmap *snakeImage3;
    QPixmap *snakeImage4;
    QPixmap *snakeImage5;
    QPixmap *snakeImage6;
    QPixmap *snakeImage7;
    QPixmap *snakeImage8;
    QPixmap *wallImage;
    QPixmap *appleImage;

public:
    Network2Player(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *evt);
    void gameOver(int winner);
    int getDirection();
    void setPart(int i, int x, int y);
    void resetObjects();
    void setPlayer(int player);

public slots:
    void updateField();
};

#endif // NETWORK2PLAYER_H
