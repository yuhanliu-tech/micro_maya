#include "drawable.h"
#include <la.h>

Drawable::Drawable(OpenGLContext* context)
    : count(-1), bufIdx(), bufPos(), bufNor(), bufCol(),
      idxBound(false), posBound(false), norBound(false), colBound(false),
      mp_context(context)
{}

Drawable::~Drawable() {
    destroy();
}


void Drawable::destroy()
{
    mp_context->glDeleteBuffers(1, &bufIdx);
    mp_context->glDeleteBuffers(1, &bufPos);
    mp_context->glDeleteBuffers(1, &bufNor);
    mp_context->glDeleteBuffers(1, &bufCol);
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return count; // used by program to determine VBOS to send to GPU
    // we have mesh on CPU
    // need to figure out how to send individual info to GPU
    // vertex shader vars come from VBO
    // set up VBOs but now with arbitrary position from JSON
    // have to figure out how to triangulate mesh and send info to GPU
    // set vertex shader variables so GPU can draw mesh
    // four hald edges in face of cube
    // vertex stores one half edge that it points to
    // start at face, choose half edge and go around
    // count is size of index array (number of triangles times 3)
    // set count to number of indices in index VBO
    // count is used to send information to the GPU
    // look through shader program
}

void Drawable::generateIdx()
{
    idxBound = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &bufIdx);
}

void Drawable::generatePos()
{
    posBound = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mp_context->glGenBuffers(1, &bufPos);
}

void Drawable::generateNor()
{
    norBound = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mp_context->glGenBuffers(1, &bufNor);
}

void Drawable::generateCol()
{
    colBound = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &bufCol);
}

void Drawable::generateJointIDs()
{
    jointIDsBound = true;
    mp_context->glGenBuffers(1, &bufJointIDs);
}

void Drawable::generateWeights()
{
    weightsBound = true;
    mp_context->glGenBuffers(1, &bufWeights);
}

bool Drawable::bindIdx()
{
    if(idxBound) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    }
    return idxBound;
}

bool Drawable::bindPos()
{
    if(posBound){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    }
    return posBound;
}

bool Drawable::bindNor()
{
    if(norBound){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    }
    return norBound;
}

bool Drawable::bindCol()
{
    if(colBound){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    }
    return colBound;
}

bool Drawable::bindWeights() {
    if (weightsBound) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufWeights);
    }
    return weightsBound;
}

bool Drawable::bindJointIDs() {
    if (jointIDsBound) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJointIDs);
    }
    return jointIDsBound;
}

