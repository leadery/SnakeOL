#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <list>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include"1_message.pb.h"
//using namespace std;
const int BORDERLEFT = -50;
const int BORDERRIGHT = 40;
const int BORDERBOTTOM = -40;
const int BORDERTOP = 50;
const int THICKNESS = 10;
const int yLength = (BORDERTOP-BORDERBOTTOM)/THICKNESS+1;
const int xLength = (BORDERRIGHT-BORDERLEFT)/THICKNESS+1;
const int mapSize = yLength * xLength;
class Axis
{
    public:
        int getX() const {return x;}
        int getY() const {return y;}
        void setPosition(int a, int b);
        void increase(int a, int b);
        void decrease(int a, int b);
        Axis(int a, int b);
        Axis();
        Axis(int a);
        int toSerial();
        bool equal(Axis p);
    private:
        int x,y;
};
class Apple
{
    public:
        void generate();
        void display();
        Apple();
        Axis getPosition();
    private:
        Axis apple_all;
};
class Snake
{
    public:
        void move();
        void changeDirection(int,int);
        void isDead();
        void display();
        bool isBody(Axis);
        bool isHead(Axis);
        void autoDirect();
        void autoDirectV2();
        int getLen();
        Snake();
    private:
        int length;
        Axis direction;
        Axis nextDirection;
        Axis nextStep;
        std::list<Axis> body;
};
