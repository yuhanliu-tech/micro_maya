#pragma once
#ifndef JOINT_H
#define JOINT_H
#include <la.h>
#include <vector>
#include "drawable.h"
#include <QTreeWidgetItem>
#include <QJsonObject>
#include <QJsonArray>
#include "smartpointerhelp.h"
#include "shaderprogram.h"


class Joint: public Drawable, public QTreeWidgetItem
{
public:

    Joint(OpenGLContext* context);

    glm::vec3 translate;
    glm::quat rotate;

    glm::mat4 bindMat;
    Joint* parent;
    std::vector<uPtr<Joint>> children;

    QString name;
    int id;
    static int idLast;

    glm::mat4 computeLocalT();
    glm::mat4 computeOverallT();
    void computeBind();

    void addChild(Joint*);

    void create() override;
    void createFirst();
    void createClicked(QTreeWidgetItem *item);

    GLenum drawMode() override;

    void getAllTransformations(glm::mat4 t[100]);
    void getAllBinds(glm::mat4 b[100]);

};

#endif // JOINT_H
