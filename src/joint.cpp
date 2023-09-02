#include "joint.h"
#include <iostream>

int Joint::idLast = -1;

Joint::Joint(OpenGLContext* context)
    :Drawable(context), translate(glm::vec3(0,0,0)), rotate(glm::quat(1,0,0,0)), parent(nullptr), name(QString("Joint")), id(idLast++), bindMat(glm::mat4(1))
{}

void Joint::computeBind() {

    bindMat = glm::mat4(glm::inverse(computeOverallT()));

    for (uPtr<Joint>& child : children) {
        child -> computeBind();
    }

}

glm::mat4 Joint::computeLocalT() {

    glm::mat4 a = glm::translate(glm::mat4(1.0f), translate) * glm::mat4_cast(rotate);

    return a * glm::scale(glm::mat4(1.0f), glm::vec3(1,1,1));
}

glm::mat4 Joint::computeOverallT() {
    if (parent != nullptr) {
        return parent -> computeOverallT() * computeLocalT();
    } else {
        return computeLocalT();
    }
}

void Joint::getAllTransformations(glm::mat4 t[]) {
    t[id] = computeOverallT();
    for (uPtr<Joint>& child : children) {
        child->getAllTransformations(t);
    }
}

void Joint::getAllBinds(glm::mat4 b[]) {
    b[id] = bindMat;
    for (uPtr<Joint>& child : children) {
        child->getAllBinds(b);
    }
}

void Joint::addChild(Joint* child) {
    QTreeWidgetItem::addChild(child);
}

void Joint::create() {

    std::vector<glm::vec4> pos, color;
    std::vector<GLuint> idx;

    // circle positions

    for (int i= 0; i<12; i++) {
        glm::vec4 v = glm::rotate(glm::mat4(1.0f), glm::radians(i*30.0f), glm::vec3(0, 0, 1)) * glm::vec4(0,0.5f,0,1);
        pos.push_back(v);
    }
    for (int i= 12; i<24; i++) {
        glm::vec4 v = glm::rotate(glm::mat4(1.0f), glm::radians(i*30.0f), glm::vec3(1, 0, 0)) * glm::vec4(0,0.5f,0,1);
        pos.push_back(v);
    }
    for (int i= 24; i<36; i++) {
        glm::vec4 v = glm::rotate(glm::mat4(1.0f), glm::radians(i*30.0f), glm::vec3(0, 1, 0)) * glm::vec4(0.5f,0,0,1);
        pos.push_back(v);
    }

    // add to colors

    for (int i= 0; i<12; i++) {
        color.push_back(glm::vec4(0,0,1,1));
    }
    for (int i= 12; i<24; i++) {
        color.push_back(glm::vec4(1,0,0,1));
    }
    for (int i= 24; i<36; i++) {
        color.push_back(glm::vec4(0,1,0,1));
    }

    // add to idx

    for (int i= 0; i<22; i+=2) {
        idx.push_back(i/2);
        idx.push_back((i/2) + 1);
    }
    idx.push_back(11);
    idx.push_back(0);

    for (int i= 24; i<46; i+=2) {
        idx.push_back(i/2);
        idx.push_back((i/2) + 1);
    }
    idx.push_back(23);
    idx.push_back(12);

    for (int i= 48; i<70; i+=2) {
        idx.push_back(i/2);
        idx.push_back((i/2) + 1);
    }
    idx.push_back(35);
    idx.push_back(24);

    // draw lines between joints

    if (parent != nullptr) {

        pos.push_back(glm::vec4(0, 0, 0, 1));
        color.push_back(glm::vec4(1, 1, 0, 1));

        pos.push_back(glm::inverse(computeLocalT()) * glm::vec4(0, 0, 0, 1));
        color.push_back(glm::vec4(1, 0, 1, 1));

        idx.push_back(36);
        idx.push_back(37);

    }

    this->count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);

}

void Joint::createClicked(QTreeWidgetItem *item) {

    std::vector<glm::vec4> pos, color;
    std::vector<GLuint> idx;

    // circle positions

    for (int i= 0; i<12; i++) {
        glm::vec4 v = glm::rotate(glm::mat4(1.0f), glm::radians(i*30.0f), glm::vec3(0, 0, 1)) * glm::vec4(0,0.5f,0,1);
        pos.push_back(v);
    }
    for (int i= 12; i<24; i++) {
        glm::vec4 v = glm::rotate(glm::mat4(1.0f), glm::radians(i*30.0f), glm::vec3(1, 0, 0)) * glm::vec4(0,0.5f,0,1);
        pos.push_back(v);
    }
    for (int i= 24; i<36; i++) {
        glm::vec4 v = glm::rotate(glm::mat4(1.0f), glm::radians(i*30.0f), glm::vec3(0, 1, 0)) * glm::vec4(0.5f,0,0,1);
        pos.push_back(v);
    }

    // add to colors

    if (this == item) {

        for (int i = 0; i < 36; i++) {
            color.push_back(glm::vec4(1,1,1,1));
        }

        for (uPtr<Joint>& child : children) {
            child -> create();
        }

    } else {

        for (int i= 0; i<12; i++) {
            color.push_back(glm::vec4(0,0,1,1));
        }
        for (int i= 12; i<24; i++) {
            color.push_back(glm::vec4(1,0,0,1));
        }
        for (int i= 24; i<36; i++) {
            color.push_back(glm::vec4(0,1,0,1));
        }

        for (uPtr<Joint>& child : children) {
            child -> createClicked(item);
        }
    }

    // add to idx

    for (int i= 0; i<22; i+=2) {
        idx.push_back(i/2);
        idx.push_back((i/2) + 1);
    }
    idx.push_back(11);
    idx.push_back(0);

    for (int i= 24; i<46; i+=2) {
        idx.push_back(i/2);
        idx.push_back((i/2) + 1);
    }
    idx.push_back(23);
    idx.push_back(12);

    for (int i= 48; i<70; i+=2) {
        idx.push_back(i/2);
        idx.push_back((i/2) + 1);
    }
    idx.push_back(35);
    idx.push_back(24);

    // draw lines between joints

    if (parent != nullptr) {

        pos.push_back(glm::vec4(0, 0, 0, 1));
        color.push_back(glm::vec4(1, 1, 0, 1));

        pos.push_back(glm::inverse(computeLocalT()) * glm::vec4(0, 0, 0, 1));
        color.push_back(glm::vec4(1, 0, 1, 1));

        idx.push_back(36);
        idx.push_back(37);

    }

    this->count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);

}

void Joint::createFirst() {

    std::vector<glm::vec4> pos, color;
    std::vector<GLuint> idx;

    this->count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);

}

GLenum Joint::drawMode() {
    return GL_LINES;
}
