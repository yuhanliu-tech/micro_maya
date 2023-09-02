#include "mesh.h"
#include <QFile>
#include <unordered_set>

#include <iostream>

Mesh::Mesh(OpenGLContext* context)
    : Drawable(context), v_ptrs(), f_ptrs(), he_ptrs(), binded(false)
{}

void Mesh::subdivide() {

    int n = f_ptrs.size(); // store original number of faces
    int m = v_ptrs.size(); // store original vertices
    int h = he_ptrs.size(); // store original half-edges

    // call helper to find each face's centroid and store in map
    std::map<Face*, glm::vec3> centroids = calculateCentroids();

    // compute smoothed midpoint of each edge in mesh
    std::unordered_set<int> split;

    int test = 0;

    for (int i = 0; i < h; i ++) {
        HalfEdge* curr = he_ptrs[i].get();
        if (split.count(curr->id) == 0) {
            split.insert(curr -> sym -> id);
            glm::vec3 f1 = centroids.at(curr -> face);
            glm::vec3 f2 = centroids.at(curr->sym->face);

            // call helper function that creates midpoint and sets half edges that it divides
            uPtr<Vertex> mid = computeSmoothedMid(curr, f1, f2);

            v_ptrs.push_back(std::move(mid));
            test ++;
        }
    }

    // smooth original vertices

    std::cout << "new subdivision" <<std::endl;
    for (int i = 0; i < m; i++)
    {
        glm::vec3 e = glm::vec3(0,0,0);
        glm::vec3 f = glm::vec3(0,0,0);
        float n = 0.f;

        HalfEdge* currHE = v_ptrs[i]->he;
        HalfEdge* last = currHE;

        do {
            e += last->sym->between->pos;
            f += centroids[last->face];
            n += 1.f;
            last = last -> next -> sym;
        }while(last != currHE);

        v_ptrs[i]->pos = (v_ptrs[i]->pos * ((n - 2) / n)) +
                            (e * (1.f / (n * n))) + (f * (1.f / (n * n)));
    }

    // for each original face, split into N quadrangle faces
    for (int i = 0; i < n; i++) {
        quadrangulate(f_ptrs[0].get(), centroids);
        f_ptrs.erase(f_ptrs.begin());
    }

    centroids.clear();
}

void Mesh::quadrangulate(Face* f, std::map<Face*, glm::vec3> centroids) {

    // helper function to quadrangulate

    // Adam's first method: count number of subfaces
    HalfEdge *curr = f -> he;
    int subfaces = 0;
    do {
        subfaces++;
        curr = curr -> next -> next;
    } while (curr != f -> he);

    curr = f -> he -> next;
    uPtr<Vertex> centroid = mkU<Vertex>(centroids.at(f));

    HalfEdge* prevToCentroid = nullptr;
    HalfEdge* firstFromCentroid = nullptr;

    // begin setting new half edges
    for (int i = 0; i < subfaces; i ++) {

        uPtr<HalfEdge> toCentroid = mkU<HalfEdge>();
        uPtr<HalfEdge> fromCentroid = mkU<HalfEdge>();

        uPtr<Face> newFace = mkU<Face>();
        toCentroid.get() -> face = newFace.get();
        fromCentroid.get() -> face = newFace.get();
        curr -> face = newFace.get();
        curr -> next -> face = newFace.get();

        newFace->he = curr;

        if (firstFromCentroid == nullptr) {
            firstFromCentroid = fromCentroid.get();
        }

        toCentroid->next = fromCentroid.get();
        fromCentroid->next = curr;

        if (prevToCentroid != nullptr) {
            fromCentroid.get() -> symWith(prevToCentroid);
        }

        toCentroid.get()->setVert(centroid.get());
        fromCentroid.get()->setVert(curr -> sym -> between);

        HalfEdge* currNext = curr -> next;

        curr = curr -> next -> next;

        currNext -> next = toCentroid.get();

        prevToCentroid = toCentroid.get();

        f_ptrs.push_back(std::move(newFace));

        he_ptrs.push_back(std::move(toCentroid));
        he_ptrs.push_back(std::move(fromCentroid));

    }

    prevToCentroid->symWith(firstFromCentroid); // connect sym pointers
    v_ptrs.push_back(std::move(centroid));

}

uPtr<Vertex> Mesh::computeSmoothedMid(HalfEdge* curr, glm::vec3 f1,  glm::vec3 f2) {

    // f1 is curr -> face
    uPtr<HalfEdge> newHalf = mkU<HalfEdge>();

    HalfEdge* oldHalfSym = curr->sym;
    uPtr<HalfEdge> newHalfSym = mkU<HalfEdge>();

    // set new pointers

    // syms
    newHalf->symWith(oldHalfSym);
    newHalfSym->symWith(curr);

    // faces
    newHalf->face = curr->face;
    newHalfSym->face = oldHalfSym->face;

    // next
    newHalf->next = curr->next;
    newHalfSym->next = oldHalfSym->next;

    // update old pointers

    // next
    curr->next = newHalf.get();
    oldHalfSym->next = newHalfSym.get();

    // vertex

    Vertex* end1 = curr->findPrev();
    Vertex* end2 = curr -> between;

    uPtr<Vertex> midpoint = mkU<Vertex>(glm::vec3((end1->pos + end2->pos
                                                  + f1 + f2) / 4.f));

    newHalfSym->setVert(oldHalfSym -> between);
    newHalf->setVert(curr->between);

    curr->setVert(midpoint.get());
    oldHalfSym->setVert(midpoint.get());

    he_ptrs.push_back(std::move(newHalf));
    he_ptrs.push_back(std::move(newHalfSym));

    return midpoint;

}


std::map<Face*, glm::vec3> Mesh::calculateCentroids() {

    // helper function to calculate centroids in glm::vec3s

    std::map<Face*, glm::vec3> centroids;

    for (const uPtr<Face> &f : f_ptrs) {

        HalfEdge* first = f->he;
        HalfEdge* last = first;
        int verts = 0;
        glm::vec3 centroid = glm::vec3(0,0,0);

        do {
            centroid += last -> between -> pos;
            verts ++;
            last = last -> next;
        } while (last != first);

        centroid /= verts;
        centroids[f.get()] = centroid;
    }
    return centroids;
}


void Mesh::loadObj(QString filename) {

    v_ptrs.clear();
    f_ptrs.clear();
    he_ptrs.clear();

    QFile file = QFile(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    std::map<std::pair<Vertex*, Vertex*>, HalfEdge*> symPtrs;

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList stuff = line.split(" ");

        // process vertices
        if (line.startsWith("v ")) {

           glm::vec3 pos = glm::vec3(stuff.at(1).toFloat(), stuff.at(2).toFloat(), stuff.at(3).toFloat());
           uPtr<Vertex> v = mkU<Vertex>(pos);
           v_ptrs.push_back(move(v));
        }

        Vertex* prevV;

        // process vertices belonging to faces
        if (line.startsWith("f ")) {

            uPtr<Face> f = mkU<Face>();

            HalfEdge *first = nullptr;
            uPtr<HalfEdge> edge;

            Vertex *firstV = nullptr;

            // create half-edges
            for (int i = 1; i < stuff.size(); i ++) {
                QStringList num = stuff.at(i).split("/");
                Vertex *v = v_ptrs.at(num.at(0).toInt()-1).get();
                edge = mkU<HalfEdge>();

                edge->setVert(v);
                edge->face = f.get();

                if (i == 1) {
                    first = edge.get();
                    prevV = v;
                    firstV = v;
                }

                if (i != 1) {

                    he_ptrs.back().get()->next = edge.get();

                    symPtrs.insert(std::pair(std::pair(edge->between, prevV), edge.get()));
                    symPtrs.insert(std::pair(std::pair(prevV, edge->between), edge.get()));

                    prevV = edge -> between;

                }

                if (i == stuff.size() - 1) {
                    symPtrs.insert(std::pair(std::pair(firstV, prevV), first));
                    symPtrs.insert(std::pair(std::pair(prevV, firstV), first));
                }

                he_ptrs.push_back(move(edge));
            }
            he_ptrs.back().get()->next = first;


            f->he = he_ptrs.back().get();
            f_ptrs.push_back(move(f));
        }
    }

    // set sym pointers

    for (uPtr<HalfEdge> & curr: he_ptrs) {
        HalfEdge *nextH = curr -> next;

        while (nextH -> next != curr.get()) {
            nextH = nextH -> next;
        }
        Vertex *prevV = nextH -> between;
        Vertex *currV = curr -> between;

        std::pair<Vertex*, Vertex*> v = std::pair(prevV, currV);

        if (symPtrs.find(v) != symPtrs.end()) {
            HalfEdge *sym = symPtrs.at(v);

            if (sym -> id != curr -> id) {
                curr -> symWith(sym);
            }
        }
    }

    create();
}

void Mesh::create() {

    std::vector<glm::vec4> pos, nor, color;
    std::vector<GLuint> idx;
    std::vector<glm::vec2> weights;
    std::vector<glm::ivec2> jointIDs;

    int totalVerts = 0;

    for (const uPtr<Face> &f : f_ptrs) {
        int vertsOnFace = 0;
        HalfEdge* curr = f -> he;

        do {

            pos.push_back(glm::vec4(curr -> between -> pos, 1.f));
            color.push_back(curr -> face -> color);

            if (binded) {
                weights.push_back(glm::vec2(curr->between->weight1, curr->between->weight2));
                jointIDs.push_back(glm::ivec2(curr->between->inf1->id, curr->between->inf2->id));
            }

            glm::vec3 a = curr -> between -> pos;
            glm::vec3 b = curr -> between -> he -> between -> pos;
            glm::vec3 c = curr -> between -> he -> between -> he -> between -> pos;

            glm::vec4 norm = glm::vec4(glm::normalize(glm::cross(c-b,b-a)), 0.f);
            nor.push_back(norm);

            vertsOnFace++;
            curr = curr -> next;

        } while (curr != f -> he);

        for (int i = 0; i < vertsOnFace - 2; i++) {
            idx.push_back(0 + totalVerts);
            idx.push_back(i + 1 + totalVerts);
            idx.push_back(i + 2 + totalVerts);
        }
        totalVerts += vertsOnFace;
    }

    this->count = idx.size();

    // OpenGL Stuff


    generateJointIDs();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJointIDs);
    mp_context->glBufferData(GL_ARRAY_BUFFER, jointIDs.size() * sizeof(glm::ivec2), jointIDs.data(), GL_STATIC_DRAW);

    generateWeights();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufWeights);
    mp_context->glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(glm::vec2), weights.data(), GL_STATIC_DRAW);

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);

}

GLenum  Mesh::drawMode() {
    return GL_TRIANGLES;
}

/*void Mesh::bind(Joint* root) {

    binded = true;
    for (int i = 0; i < v_ptrs.size(); i++) {
        v_ptrs[i].get()->findJoints(root);
    }
}*/

void Mesh::bind(Joint* root) {

    binded = true;

    for (int i = 0; i < v_ptrs.size(); i++) {

        Vertex* v = v_ptrs[i].get();

        Joint* j1 = nullptr;
        Joint* j2 = nullptr;
        float d1 = std::numeric_limits<float>::max();
        float d2 = std::numeric_limits<float>::max();
        findJoint(v->pos, root, j1, j2, d1, d2);

        float denom = d1 + d2;
        v->weight1 = d2 / denom;
        v->weight2 = d1 / denom;
        v->inf1 = j1;
        v->inf2 = j2;

    }

}

void Mesh::findJoint(glm::vec3 pos, Joint* curr, Joint*& j1, Joint*& j2, float& d1, float& d2) {

    float dist = glm::distance(glm::vec4(pos, 1), curr->computeOverallT() * glm::vec4(0,0,0,1));

    // d1 is smallest, d2 is second smallest

    if (dist < d1) {

        j2 = j1;
        d2 = d1;
        j1 = curr;
        d1 = dist;

    } else if (dist < d2) {
        j2 = curr;
        d2 = dist;
    }

    for (auto& child : curr->children) {
        findJoint(pos, child.get(), j1, j2, d1, d2);
    }
}


