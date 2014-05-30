#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include"snake.h"
#include "1_message.pb.h"
#define MAXLENGTH 2000
bool startFlag = false;
int speed = 10;
int counter=0;
int counterThre=0;
int route[mapSize];
std::vector<int> shortRoute;
struct addrinfo lookup_addr;
struct addrinfo *send_addr;
int sock;
unsigned int sequenceNum = 0;
SnakeOL::frame image;
void keyboard(int key, int x, int y)
{
    SnakeOL::control keyPressed;
    keyPressed.set_seqnum(sequenceNum);
    keyPressed.set_keyvalue(key);

    uint32_t size = keyPressed.ByteSize();
    char *data = new char[size+4];
    memcpy(data, &size, 4);
    keyPressed.SerializeToArray(data+4, size);
    ssize_t bytes_sent = sendto(sock, data, size+4, 0,
            send_addr->ai_addr, send_addr->ai_addrlen);
    delete data;
    if (bytes_sent != size+4)
    {
        perror("sendto failed");
        close(sock);
        return;
    }
    if(startFlag == false)
        startFlag = true;
}
void *recvFrame(void *)
{
    char *recvBuff = new char[200];
    socklen_t addr_len = send_addr->ai_addrlen;
    while(1)
    {
        std::cout<<"receiving..."<<std::endl;
        ssize_t bytes = recvfrom(sock, recvBuff, MAXLENGTH, 0,
                send_addr->ai_addr, &addr_len);
        uint32_t size;
        memcpy(&size, recvBuff, sizeof(uint32_t));
        if (!image.ParseFromArray(recvBuff+sizeof(uint32_t),size))
        {
            delete recvBuff;
            perror("recvfrom failed");
            close(sock);
            return NULL;
        }
        std::cout<<image.time()<<"~~~";
        std::cout<<image.position(1)<<std::endl;
    }
    delete recvBuff;
}
Axis::Axis(int a, int b)
{
    x = a;
    y = b;
}
Axis::Axis()
{
    x = 1 * speed;
    y = 0 * speed;
}
void Axis::setPosition(int a, int b)
{
    x = a;
    y = b;
}
bool Axis::equal(Axis p)
{
    if(x==p.getX() && y==p.getY())
        return true;
    else
        return false;
}
void Axis::increase(int a, int b)
{
    x += a;
    y += b;
}
void Axis::decrease(int a, int b)
{
    x -= a;
    y -= b;
}
Axis::Axis(int a)
{
    x = (a%((BORDERRIGHT-BORDERLEFT)/THICKNESS+1))*THICKNESS+BORDERLEFT;
    y = (a/((BORDERRIGHT-BORDERLEFT)/THICKNESS+1))*THICKNESS+BORDERBOTTOM;
}
int Axis::toSerial()
{
    return((y-BORDERBOTTOM)/THICKNESS*((BORDERRIGHT-BORDERLEFT)/THICKNESS+1)
            +(x-BORDERLEFT)/THICKNESS);
}

void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    for(int i = 0; i < image.position_size(); ++i)
    {
        if(i == 0)
        {

            Axis apple_all(image.position(i));
            glRectf(apple_all.getX()+2, apple_all.getY()-2, apple_all.getX()-2+THICKNESS, apple_all.getY()+2-THICKNESS);
        }
        else
        {
            Axis snake(image.position(i));
            glRectf(snake.getX(), snake.getY(), snake.getX()+THICKNESS, snake.getY()-THICKNESS);
        }
    }
    glFlush();
    glPopMatrix();
    glutSwapBuffers();
}

void spinDisplay(int)
{
    if(startFlag == true)
    {
        char *recvBuff = new char[200];
        socklen_t addr_len = send_addr->ai_addrlen;
        std::cout<<"receiving..."<<std::endl;
        ssize_t bytes = recvfrom(sock, recvBuff, MAXLENGTH, 0,
                send_addr->ai_addr, &addr_len);
        uint32_t size;
        memcpy(&size, recvBuff, sizeof(uint32_t));
        if (!image.ParseFromArray(recvBuff+sizeof(uint32_t),size))
        {
            delete recvBuff;
            perror("recvfrom failed");
            close(sock);
            return;
        }
        std::cout<<image.time()<<"~~~";
        std::cout<<image.position(1)<<std::endl;
        delete recvBuff;
    }
    glutPostRedisplay();
    //    snake.autoDirect();
    glutTimerFunc(20, spinDisplay, 0);
}

void reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(BORDERLEFT, BORDERRIGHT+THICKNESS, BORDERBOTTOM-THICKNESS, BORDERTOP, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
                spinDisplay(0);
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN)
                glutIdleFunc(NULL);
            break;
        default:
            break;
    }
}

/*
 *  Request double buffer display mode.
 *  Register mouse input callback functions
 */
int main(int argc, char** argv)
{
    memset(&lookup_addr, 0, sizeof(struct addrinfo));
    lookup_addr.ai_family = AF_UNSPEC;
    lookup_addr.ai_socktype = SOCK_DGRAM;
    lookup_addr.ai_protocol = IPPROTO_UDP;


    if (getaddrinfo(argv[1], argv[2], &lookup_addr, &send_addr) != 0)
    {
        perror("getaddrinfo failed");
        return 1;
    }

    sock = socket(send_addr->ai_family, send_addr->ai_socktype,
            send_addr->ai_protocol);
    if (sock < 0)
    {
        perror("socket failed");
        return 1;
    }
    //pthread_t t = pthread_create(&t, NULL, recvFrame, NULL);
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (400, 400);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(keyboard);
    //    glutMouseFunc(mouse);
    spinDisplay(0);
    glutMainLoop();
    //pthread_join(t, NULL);
    google::protobuf::ShutdownProtobufLibrary();
    freeaddrinfo(send_addr);
    close(sock);
    return 0;
}
