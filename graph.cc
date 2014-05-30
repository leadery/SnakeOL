/************************************
 * This file describes the map of the game
 *
 ************************************/
#include<iostream>
#include<queue>

const bool UNVISITED = false;
const bool VISITED = true;
//const int INFINITY = 2;

class Edge
{
    public:
        int from, to ,weight;
        Edge()
        {
            from = -1;
            to = -1;
            weight = -1;
        }
        Edge(int f, int t, int w)
        {
            from = f;
            to = t;
            weight = w;
        }
        bool operator > (Edge oneEdge)
        {
            return weight > oneEdge.weight;
        }
        bool operator < (Edge oneEdge)
        {
            return weight < oneEdge.weight;
        }
};
struct listUnit
{
    int vertex;
    int weight;
};
template < class T>
class Link
{
    public:
        Link(const T& tVal, Link * nextVal = NULL)
        {
            elem = tVal;
            next = nextVal;
        }
        Link(Link *nextVal = NULL)
        {
            next = nextVal;
        }
        ~Link()
        {
        }
        T elem;
        Link *next;
        Link *head;
};
class Graph
{
    public:
        int VerticesNum();
        int EdgesNum();
        Graph(int numV);
        ~Graph();
        Edge FirstEdge(int oneVertex);
        Edge NextEdge(Edge preEdge);
        void setEdge(int fromVertex, int toVertex, int w);
        void delEdge(int fromVertex, int toVertex);
        bool isEdge(Edge oneEdge);
        int FromVertex(Edge oneEdge);
        int ToVertex(Edge oneEdge);
        int Weight(Edge oneEdge);
        void visit(int oneVertex);
        bool isVisited(int oneVertex);
        void renew();
    private:
        int numVertex;
        int numEdge;
        bool *Mark;
        Link<struct listUnit> *graList;
};
Graph::Graph(int numV)
{
    numVertex = numV;
    numEdge = 0;
    Mark = new bool[numVertex];
    for(int i=0; i < numVertex; ++i)
    {
        Mark[i] = UNVISITED;
    }
    graList = new Link<struct listUnit> [numVertex];
}
Graph::~Graph()
{
    delete [] Mark;
    delete [] graList;
}
Edge Graph::FirstEdge(int oneVertex)
{
    Edge myEdge;
    myEdge.from = oneVertex;
    myEdge.to = -1;
    Link<struct listUnit> *temp = &graList[oneVertex];
    if(temp -> next != NULL)
    {
        myEdge.to = temp->next->elem.vertex;
        myEdge.weight = temp->next->elem.weight;
    }
    return myEdge;
}
Edge Graph::NextEdge(Edge preEdge)
{
    Edge myEdge;
    myEdge.from = preEdge.from;
    myEdge.to = -1;
    Link<struct listUnit> *temp = &graList[preEdge.from];
    while(temp -> next != NULL&&temp->next->elem.vertex <= preEdge.to)
        temp = temp->next;
    if(temp->next != NULL)
    {
        myEdge.to = temp->next->elem.vertex;
        myEdge.weight = temp->next->elem.weight;
    }
    return myEdge;
}
bool Graph::isEdge(Edge oneEdge)
{
    Link<listUnit> * temp = &graList[oneEdge.from];
    while(temp->next != NULL && temp->next->elem.vertex < oneEdge.to)
        temp = temp->next;
    if(temp->next == NULL)
        return false;
    if(temp->next->elem.weight == oneEdge.weight
            && temp->next->elem.vertex == oneEdge.to)
        return true;
    else
        return false;
}
void Graph::setEdge(int fromVertex, int toVertex, int w)
{
    Link<listUnit> * temp = &graList[fromVertex];
    while(temp->next != NULL && temp->next->elem.vertex < toVertex)
        temp = temp->next;
    if(temp->next == NULL)
    {
        temp->next = new Link<listUnit>;
        temp->next->elem.vertex = toVertex;
        temp->next->elem.weight = w;
        ++numEdge;
        return;
    }
    if(temp->next->elem.vertex == toVertex)
    {
        temp->next->elem.weight = w;
        return;
    }
    if(temp->next->elem.vertex > toVertex)
    {
        Link<listUnit> *temp2 = temp->next;
        temp->next = new Link<listUnit>;
        temp->next->elem.vertex = toVertex;
        temp->next->elem.weight = w;
        temp = temp->next;
        temp->next = temp2;
        ++numEdge;
        return;
    }
}
void Graph::delEdge(int fromVertex, int toVertex)
{
    Link<listUnit> *temp = &graList[fromVertex];
    while(temp->next != NULL && temp->next->elem.vertex < toVertex)
        temp = temp->next;
    if(temp->next == NULL)
        return;
    if(temp->next->elem.vertex > toVertex)
        return;
    if(temp->next->elem.vertex == toVertex)
    {
        Link<listUnit> *temp2 = temp->next->next;
        delete temp->next;
        temp->next = temp2;
        --numEdge;
        return;
    }
}
void Graph::visit(int oneVertex)
{
    if(oneVertex >= 0 && oneVertex < numVertex)
        Mark[oneVertex] = VISITED;
}
void Graph::renew()
{
    for(int i=0; i<numVertex; ++i)
        Mark[i] = UNVISITED;
}
bool Graph::isVisited(int oneVertex)
{
    if(oneVertex >= 0 && oneVertex < numVertex &&
            Mark[oneVertex] == VISITED)
        return true;
    else
        return false;
}
bool BFS(Graph &G, int *route, int v, int dest)
{
    std::queue<int> Q;
    G.visit(v);
    route[v] = v;
    Q.push(v);
    while(!Q.empty())
    {
        int u = Q.front();
        Q.pop();
        for(Edge e = G.FirstEdge(u); G.isEdge(e); e=G.NextEdge(e))
        {
            if(G.isVisited(e.to) == false)
            {
                G.visit(e.to);
                route[e.to] = e.from;
                if(e.to == dest)
                    return true;
                Q.push(e.to);
            }
        }
    }
    return false;
}
void setGraph(Graph &G);
