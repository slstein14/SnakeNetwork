#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include <QPainter>
#include <QPixmap>
#include <QWidget>

class RenderObject : QObject
{
private:
    int posX, posY;
    int sizeX, sizeY;
    QPixmap *image;
    enum direction {Up = 0, Left = 1, Right = 2, Down=3};

public:
    RenderObject(QWidget *parent);
    void drawObject(QPainter &painter);
    void setXCoord(int x);
    int getXCoord();
    void setYCoord(int y);
    int getYCoord();
    void setImage(QPixmap *image);
};

#endif // RENDEROBJECT_H
