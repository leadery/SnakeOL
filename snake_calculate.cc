/************************************
 * This is the server part
 ************************************/
#include "snake.h"
#include "graph.cc"
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <csignal>
#include <iostream>
#include "1_message.pb.h"

Snake snake;
Apple apples;
int stop = 0;
int sock = 0;
struct sockaddr_storage client_addr;
socklen_t addr_len;
int speed = 10;
int counter=0;
int counterThre=10;
const int MAXLENGTH = 2000;
Graph graph(mapSize);
int route[mapSize];
std::vector<int> shortRoute;
SnakeOL::frame image;
void handler(int signal)
{
    stop = 1;
    if (sock != 0)
    {
        close(sock);
        sock = 0;
        std::cout << "Closed socket" << std::endl;
    }
}
int setTicker(int n_msecs)
{
    struct itimerval new_timeset;
    long    n_sec, n_usecs;
    n_sec = n_msecs / 1000 ;
    n_usecs = ( n_msecs % 1000 ) * 1000L ;
    new_timeset.it_interval.tv_sec  = n_sec;
    new_timeset.it_interval.tv_usec = n_usecs;
    n_msecs = 1;
    n_sec = n_msecs / 1000 ;
    n_usecs = ( n_msecs % 1000 ) * 1000L ;
    new_timeset.it_value.tv_sec     = n_sec  ;
    new_timeset.it_value.tv_usec    = n_usecs ;
    return setitimer(ITIMER_REAL, &new_timeset, NULL);
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
Apple::Apple()
{
    apple_all.setPosition(2*THICKNESS,2*THICKNESS);
}
void Apple::generate()
{
    int x,y;
    do
    {
        x = (rand()%((int)((BORDERRIGHT-BORDERLEFT+THICKNESS)/THICKNESS)))*THICKNESS+BORDERLEFT;
        y = (rand()%((int)((BORDERTOP-BORDERBOTTOM+THICKNESS)/THICKNESS)))*THICKNESS+BORDERBOTTOM;
    }
    while(snake.isBody(Axis(x,y))||snake.isHead(Axis(x,y)));
    apple_all.setPosition(x,y);
}
Axis Apple::getPosition()
{
    //TODO return the position of the apple;
    return apple_all;
}

void Apple::display()
{
    image.add_position(apple_all.toSerial());
    std::cout<<"-----------------------"<<apple_all.toSerial()<<"-----------------------"<<std::endl;
}

bool Snake::isBody(Axis a)
{
    std::list<Axis>::iterator i = body.begin();
    ++i;
    for(;i!=body.end();++i)
    {
        if(a.equal(*i))
            return true;
    }
    return false;
}
bool Snake::isHead(Axis a)
{
    if(a.equal(body.front()))
        return true;
    else
        return false;
}

Snake::Snake()
{
    body.push_back(Axis( 0, 0));
    body.push_back(Axis( -THICKNESS, 0));
    body.push_back(Axis(-2*THICKNESS, 0));
    length = 3;
}

void Snake::display()
{
    for(auto i=body.begin(); i!=body.end(); ++i)
    {
        image.add_position(i->toSerial());
        std::cout<<i->toSerial()<<' ';
    }
    std::cout<<std::endl;
}

void Snake::move()
{
    if(counter == counterThre)
    {
        direction.setPosition(nextDirection.getX(),nextDirection.getY());
        nextStep.setPosition(body.begin()->getX()+direction.getX(), body.begin()->getY()+direction.getY());
        /*    if((int)nextStep.getX()%(int)THICKNESS == 0 && (int)nextStep.getY()%(int)THICKNESS == 0)
              {*/
        std::cout<<"MOVING TO ["<<nextStep.getX()<<","<<nextStep.getY()<<"] ("<<nextStep.toSerial() << ')' << std::endl;
        for(auto i = body.begin(); i!=body.end(); ++i)
        {
            //    i->decrease(counter*direction.getX(),counter*direction.getY());
            std::cout<<i->getX()<<","<<i->getY()<<std::endl;
        }
        std::cout<<"Length: "<<snake.getLen()<<std::endl;
        // counter = 0;
        // Eat apple, size increases by one

        if(nextStep.equal(apples.getPosition()))
        {
            body.push_front(nextStep);
            ++length;
            apples.generate();
        }
        // Didn't eat the apple
        else
        {
            for(auto i = body.begin(); !(i->equal(body.back())); ++i)
            {
                // Hit the border or eat itself, GAME OVER
                if(i->equal(nextStep) || nextStep.getX()>BORDERRIGHT
                        || nextStep.getX()<BORDERLEFT
                        || nextStep.getY()<BORDERBOTTOM
                        || nextStep.getY()>BORDERTOP)
                {
                    isDead();
                }
                // Move forward
            }

            body.push_front(nextStep);
            body.pop_back();
        }
        counter = 0;
        std::cout<<"MOVED"<<std::endl;
    }
    else
        ++counter;
    /*    }
          else
          {
          for(auto i = body.begin(); i != body.end(); ++i)
          {
          i->increase(direction.getX(),direction.getY());
          }
          ++counter;
          }*/
}
void Snake::autoDirect()
{
    if(counter == counterThre)
    {
        if(direction.getY() > 0 && body.begin()->getY() == BORDERTOP)
            changeDirection(1, 0);
        else if(direction.getX() > 0 && body.begin()->getY() == BORDERTOP)
            changeDirection(0, -1);
        else if(direction.getY() < 0 && body.begin()->getY() == (BORDERBOTTOM + THICKNESS)
                && body.begin()->getX() != BORDERRIGHT)
            changeDirection(1, 0);
        else if(direction.getX() > 0 && body.begin()->getY() == (BORDERBOTTOM + THICKNESS))
            changeDirection(0, 1);
        else if(direction.getX() > 0 && body.begin()->getX() == BORDERRIGHT)
            changeDirection(0, -1);
        else if(direction.getY() < 0 && body.begin()->getY() == BORDERBOTTOM)
            changeDirection(-1, 0);
        else if(direction.getX() < 0 && body.begin()->getX() == BORDERLEFT)
            changeDirection(0, 1);
    }
}
void Snake::autoDirectV2()
{
    if(counter == counterThre)
    {
        if(shortRoute.empty())
        {
            std::cout<<"Calculating the route..."<<std::endl;
            for(int i = 0; i < mapSize; ++i)
            {
                route[i] = 0;
            }
            graph.renew();
            setGraph(graph);
            int snakeHead = body.begin()->toSerial();
            int dest = apples.getPosition().toSerial();
            std::cout<<"Head,Apple:"<<snakeHead<<' '<<dest<<std::endl;
            if(BFS(graph,route,snakeHead,dest))
            {

                shortRoute.push_back(dest);
                while(route[dest] != dest)
                {
                    shortRoute.push_back(route[dest]);
                    dest = route[dest];
                }
                shortRoute.pop_back();

                for(std::vector<int>::reverse_iterator i = shortRoute.rbegin(); i != shortRoute.rend(); ++i)
                    std::cout << *i << std::endl;
            }
        }
        if(!shortRoute.empty())
        {
            Axis nextStep(shortRoute.back());
            shortRoute.pop_back();
            changeDirection((nextStep.getX()-body.begin()->getX())/speed, (nextStep.getY()-body.begin()->getY())/speed);
        }
        else
        {
            isDead();
        }
    }
}



int Snake::getLen()
{
    return length;
}

void Snake::isDead()
{
    //TODO GAME OVER
    std::cout<<"GAME OVER"<<std::endl;
    exit(0);
}

void Snake::changeDirection(int a, int b)
{
    //TODO change the movement direction
    if((direction.getX() + a*speed == 0) || (direction.getY() + b*speed == 0))
        return;
    else
        nextDirection.setPosition(a*speed,b*speed);
}


void show(int)
{
    signal(SIGALRM, show);
    std::cout<<"Time out!"<<std::endl;

    //snake.autoDirect();
    //snake.autoDirectV2();
    snake.move();
    apples.display();
    snake.display();
    image.set_time(time(NULL));
    uint32_t size;
    size = image.ByteSize();
    char *data = new char[size+4];
    memcpy(data, &size, 4);
    image.SerializeToArray(data+4,size);
    ssize_t bytes_sent = sendto(sock, data, size + sizeof(int), 0,
            (struct sockaddr *) &client_addr, addr_len);
    if (bytes_sent < 0)
    {
        perror("sendto failed");
        close(sock);
        printf("Closed socket\n");
        return;
    }
    image.Clear();
}

void keyboard(int key)
{
    switch(key)
    {
        case 101:
            snake.changeDirection(0,1);break;
        case 103:
            snake.changeDirection(0,-1);break;
        case 100:
            snake.changeDirection(-1,0);break;
        case 102:
            snake.changeDirection(1,0);break;
    }
}
void setGraph(Graph &G)
{
    for(int i = 0; i < mapSize; ++i)
    {
        if(snake.isBody(Axis(i)))
            G.visit(i);
    }
    for(int i = 0; i < xLength; ++i)
        for(int j = 0; j < yLength; ++j)
        {
            if(G.isVisited(j*xLength+i) == false)
            {
                if(j > 0 && G.isVisited(xLength*(j-1)+i) == false)
                    G.setEdge(xLength*j+i,xLength*(j-1)+i,1);
                if(j < yLength-1 && G.isVisited(xLength*(j+1)+i) == false)
                    G.setEdge(xLength*j+i,xLength*(j+1)+i,1);
                if(i > 0 && G.isVisited(xLength*j+i-1) == false)
                    G.setEdge(xLength*j+i,xLength*j+i-1,1);
                if(i < xLength-1 && G.isVisited(xLength*j+i+1) == false)
                    G.setEdge(xLength*j+i,xLength*j+i+1,1);
            }
        }
}


/*
 *  Request double buffer display mode.
 *  Register mouse input callback functions
 */
int main(int argc, char** argv)
{
    signal(SIGINT, handler);
    signal(SIGHUP, handler);
    signal(SIGTERM, handler);
    srand(time(NULL));
    struct addrinfo lookup_addr;
    memset(&lookup_addr, 0, sizeof(struct addrinfo));
    lookup_addr.ai_family = AF_INET6; //or AF_INET
    lookup_addr.ai_flags = AI_PASSIVE;
    lookup_addr.ai_socktype = SOCK_DGRAM;
    lookup_addr.ai_protocol = IPPROTO_UDP;

    struct addrinfo *listen_addr = NULL;
    if (getaddrinfo(NULL, argv[1], &lookup_addr, &listen_addr) != 0)
    {
        perror("getaddrinfo failed");
        return 1;
    }

    sock = socket(listen_addr->ai_family, listen_addr->ai_socktype,
            listen_addr->ai_protocol);
    if (sock < 0)
    {
        perror("socket failed");
        return 1;
    }
    if (bind(sock, listen_addr->ai_addr,
                listen_addr->ai_addrlen) < 0)
    {
        perror("bind failed");
        close(sock);
        printf("Closed socket\n");
        return 1;
    }
    char *buffer = NULL;

    while (!stop)
    {

        //char input[MAXLENGTH];
        //char output[MAXLENGTH];
        int input;
        addr_len = sizeof(struct sockaddr_storage);
        buffer = new char[200];
        uint32_t size;
        ssize_t bytes = recvfrom(sock, buffer, MAXLENGTH, 0,
                (struct sockaddr *) &client_addr,
                &addr_len);
        memcpy(&size,buffer,4);
        //parse
        SnakeOL::control keyPressed;
        if (!keyPressed.ParseFromArray(buffer+4, size))
        {
            delete buffer;
            perror("invalid data");
        }

        if (stop)
        {
            break;
        }

        if (bytes < 0)
        {
            perror("recvfrom failed");
            close(sock);
            printf("Closed socket\n");
            return 1;
        }
        printf("RECEIVED: \"%d\"\n", keyPressed.keyvalue());
        keyboard(keyPressed.keyvalue());
        signal(SIGALRM, show);
        setTicker(20);
    }
    std::cout << "EXIT" << std::endl;
    freeaddrinfo(listen_addr);
    if (sock != 0)
    {
        close(sock);
        printf("Closed socket\n");
    }
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
