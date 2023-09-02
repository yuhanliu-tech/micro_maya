#include "halfedge.h"

int Vertex::idLast = 1;

Vertex::Vertex(glm::vec3 p)
    :pos(p), id(idLast++), he(), inf1(nullptr), inf2(nullptr), weight1(1000.f), weight2(1000.f)
{

    QListWidgetItem::setText("Vertex " + QString::number(id));
}

void Vertex::findJoints(Joint* j) {

    j -> computeBind();
    float dist = glm::distance(glm::vec4(pos, 1), j->computeOverallT() * glm::vec4(0,0,0,1));

    if (dist < weight1 || dist < weight2) {
        if (weight1 < weight2) {
            weight2 = dist ;
            inf2 = j;
        } else {
            weight1 = dist;
            inf1 = j;
        }
    }

    for (int i = 0; i < j->children.size(); i ++) {
        j -> children.at(i).get()->computeBind();
        findJoints(j->children.at(i).get());
    }
}
