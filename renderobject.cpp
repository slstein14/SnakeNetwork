#include "renderobject.h"
#include <QDir>
#include <QCoreApplication>
#include <qdebug.h>

RenderObject::RenderObject(QWidget *parent)
{
#ifdef __APPLE__
     QDir bin(QCoreApplication::applicationDirPath());
     bin.cdUp();
     bin.cdUp();
     bin.cdUp();
     QDir::setCurrent(bin.absolutePath());
 #endif
        posX = 3; //starts at 30,30 in matrix (immediately moved by the setup)
        posY = 3;
        sizeX = 10;//size is 10x10px (1 matrix unit)
        sizeY = 10;
        image = new QPixmap("Images/default.png");//Default Image file
}

void RenderObject::drawObject(QPainter &painter)
{//Draws the object at the appropriate location
    painter.drawPixmap(10*posX, 10*posY, sizeX, sizeY, *image);//Converts matrix location to pixel location
}

void RenderObject::setXCoord(int x)
{
    posX=x;
}

int RenderObject::getXCoord()
{
    return posX;
}

void RenderObject::setYCoord(int y)
{
    posY=y;
}

int RenderObject::getYCoord()
{
    return posY;
}

void RenderObject::setImage(QPixmap *image)
{
    this->image=image;
}
