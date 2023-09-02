#include "halfedge.h"

int Face::idLast = 1;

Face::Face()
    :color(glm::vec4(glm::linearRand(0.f, 1.f),
           glm::linearRand(0.f, 1.f),
           glm::linearRand(0.f, 1.f),
           1.f)), id(idLast++), he()
{
    QListWidgetItem::setText("Face " + QString::number(id));
}
