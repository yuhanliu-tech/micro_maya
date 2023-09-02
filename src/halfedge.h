#pragma once
#include <la.h>
#include <glm/gtc/random.hpp>
#include <QListWidgetItem>
#include "smartpointerhelp.h"
#include "joint.h"

class HalfEdge;

class Face: public QListWidgetItem{
public:
    Face();
    HalfEdge* he; // pointer to half-edge that lies on this Face
    glm::vec4 color;
    int id; // unique int to identify face in menus while debugging
    static int idLast; // tracks the last ID assigned to a component, and increments that value by one each time a new vertex/face/half-edge is created
};

class Vertex: public QListWidgetItem{
public:
    Vertex(glm::vec3 p);

    Joint* inf1; // joint influencer 1
    Joint* inf2; // joint influencer 2

    float weight1;
    float weight2;

    void findJoints(Joint* j);

    glm::vec3 pos;
    HalfEdge* he; // pointer to half-edge that points to this vertex
    int id; // unique int to identify vertex in menus while debugging
    static int idLast; // tracks the last ID assigned to a component, and increments that value by one each time a new vertex/face/half-edge is created
};

class HalfEdge: public QListWidgetItem {
public:
    HalfEdge();
    void symWith(HalfEdge *e);
    void setVert(Vertex *v);
    Vertex* findPrev();
    int findVertsInFace();

    HalfEdge* sym;
    HalfEdge* next; //pointer to the next HalfEdge in the loop of HalfEdges that lie on this HalfEdge's Face
    Face* face; //A pointer to the Face on which this HalfEdge lies
    Vertex* between; //pointer to the Vertex between this HalfEdge and its next HalfEdge
    int id; // unique integer to identify the HalfEdge in menus and while debugging
    static int idLast; // tracks the last ID assigned to a component, and increments that value by one each time a new vertex/face/half-edge is created


};
