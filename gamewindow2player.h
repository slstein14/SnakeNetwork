#ifndef GAMEWINDOW2PLAYER_H
#define GAMEWINDOW2PLAYER_H
#include "renderobject.h"
#include <QWidget>
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
using namespace std;


class GameWindow2Player : public QWidget
{
    Q_OBJECT
private:
    QTimer *timer;
    RenderObject *player1;
    RenderObject *player2;
    RenderObject* wall;
    RenderObject* apple;
    HighScores* scoreSet;
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
    bool newDirection1;
    int direction2;
    bool newDirection2;
    bool player1lost;
    bool player2lost;
    QMessageBox *msg;
    QMessageBox *pbox;
    QPixmap *snakeImage1;
    QPixmap *snakeImage2;
    QPixmap *wallImage;
    QPixmap *appleImage;
public:
    GameWindow2Player(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *evt);
//    void keyReleaseEvent(QKeyEvent *evt);
    void moveSnake();
    void moveApple();
    void setDifficulty(int difficulty);
    void sendHighScoreObject(HighScores* scoreSet);
    void pauseMenu();


public slots:
    void updateField();
};

#endif // GAMEWINDOW2PLAYER_H
