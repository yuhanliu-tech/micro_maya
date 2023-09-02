#include "halfedge.h"
#include "smartpointerhelp.h"

int HalfEdge::idLast = 1;

HalfEdge::HalfEdge()
    :id(idLast++), next(), sym(), face(), between()
{
    QListWidgetItem::setText("Edge " + QString::number(id));
}

void HalfEdge::symWith(HalfEdge *e) {
    this -> sym = e;
    e -> sym = this;
}

void HalfEdge::setVert(Vertex *v) {
    this -> between = v;
    v -> he = this;
}

Vertex* HalfEdge::findPrev() {

    HalfEdge* last = this->next;

    do {

        if (last->next == this) {
            return last->between;
        } else {
            last = last -> next;
        }

    } while (last != this);
}

int HalfEdge::findVertsInFace() {

    int verts = 1;
    HalfEdge* last = this->next;

    do {

        if (last->next == this) {
            return verts + 1;
        } else {
            verts +=1;
            last = last -> next;
        }

    } while (last != this);

}
