#ifndef MESH_H
#define MESH_H

#pragma once

#include "halfedge.h"
#include "drawable.h"
#include <la.h>
#include <vector>
#include "smartpointerhelp.h"
#include <unordered_set>

class Mesh: public Drawable
{
public:

    Mesh(OpenGLContext* context);

    // set up triangular indices for any half-edge polygon face's index buffer.
    // write create() so that it organizes VBO data on a per-face basis

    // in other words, if two faces share a vertex,
    // there should be duplicate positions in your position VBO for that vertex.
    // The same goes for surface normals and vertex colors.

    // helpful to know that you can pass a std::vector as an argument
    // for glBufferData by calling its data() function.

    void create();
    void loadObj(QString filename);
    void subdivide();

    GLenum drawMode();

    std::vector<uPtr<Vertex>> v_ptrs;
    std::vector<uPtr<Face>> f_ptrs;
    std::vector<uPtr<HalfEdge>> he_ptrs;

    void bind(Joint* root);

private:

    bool binded;
    std::map<Face*, glm::vec3> calculateCentroids();
    void quadrangulate(Face* f, std::map<Face*, glm::vec3> centroids);
    uPtr<Vertex> computeSmoothedMid(HalfEdge* curr, glm::vec3 f1,  glm::vec3 f2);
    void findJoint(glm::vec3 pos, Joint* curr, Joint*& j1, Joint*& j2, float& d1, float& d2);
};

#endif // MESH_H
