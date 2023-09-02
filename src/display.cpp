#include "display.h"

// Joint
/*
JointDisplay::JointDisplay(OpenGLContext *context)
    :Drawable(context), representedJoint(nullptr)
{
}

void JointDisplay::create() {

}*/

//Vertex

VertexDisplay::VertexDisplay(OpenGLContext* context)
    :Drawable(context), representedVertex(nullptr)
{
}

void VertexDisplay::create() {

    std::vector<glm::vec4> pos, color;
    std::vector<GLuint> idx = {0};

    pos.push_back(glm::vec4(representedVertex->pos, 1.f));
    color.push_back(glm::vec4(1.f, 1.f, 1.f, 1.f));

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

void VertexDisplay::updateVertex(Vertex* v) {
    representedVertex = v;
}

GLenum VertexDisplay::drawMode() {
    return GL_POINTS;
}

//Face

FaceDisplay::FaceDisplay(OpenGLContext* context)
    :Drawable(context), representedFace(nullptr)
{
}

void FaceDisplay::create() {

    std::vector<glm::vec4> pos, color;
    std::vector<GLuint> idx;

    HalfEdge *half = representedFace -> he;
    HalfEdge *curr = half;

    int verts = 0;

    do {
        glm::vec4 posHE = glm::vec4(curr -> between -> pos, 1.f);
        pos.push_back(posHE);
        curr = curr -> next;
        verts++;

    } while (curr != half);

    glm::vec4 faceCol = representedFace -> color;
    glm::vec4 newCol = glm::vec4(1.f - faceCol.r, 1.f - faceCol.g, 1.f - faceCol.b, 1.f);

    for (int i = 0; i < verts; i ++) {
        idx.push_back(i);
        idx.push_back(i + 1);
    }

    idx.push_back(verts - 1);
    idx.push_back(0);

    for (int i = 0; i < pos.size(); i ++) {
        color.push_back(newCol);
    }

    this->count = idx.size();
    // other stuff

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

void FaceDisplay::updateFace(Face* f) {
    representedFace = f;
}

GLenum FaceDisplay::drawMode() {
    return GL_LINES;
}

// Edges

EdgeDisplay::EdgeDisplay(OpenGLContext* context)
    :Drawable(context), representedEdge(nullptr)
{
}

void EdgeDisplay::create() {

    std::vector<glm::vec4> pos, color;

    std::vector<GLuint> idx = {0,1};

    glm::vec4 p1 = glm::vec4(representedEdge->between->pos, 1.f);
    glm::vec4 p2 = glm::vec4(representedEdge-> sym -> between->pos, 1.f);

    pos.push_back(p1);
    pos.push_back(p2);

    color.push_back(glm::vec4(1.f, 0.f, 0.f, 1.f));
    color.push_back(glm::vec4(1.f, 1.f, 0.f, 1.f));

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

void EdgeDisplay::updateEdge(HalfEdge* e) {
    representedEdge = e;
}

GLenum EdgeDisplay::drawMode() {
    return GL_LINES;
}
