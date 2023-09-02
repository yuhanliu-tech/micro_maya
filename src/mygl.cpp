#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>



MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this), m_progSkeleton(this),
      m_glCamera(), m_geomMesh(this),
      m_vertDisplay(this), m_edgeDisplay(this), m_faceDisplay(this),
      selectedJoint(nullptr), jointsLoaded(false)
{
    setFocusPolicy(Qt::StrongFocus);
    m_geomJoint = mkU<Joint>(this);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
}

//method for if list item clicked, emit signal

void MyGL::createMesh(QString filename) {
    m_geomMesh.loadObj(filename);

    for (auto& v: m_geomMesh.v_ptrs) {
        emit sig_sendV(v.get());
    }

    for (auto& he: m_geomMesh.he_ptrs) {
        emit sig_sendHE(he.get());
    }

    for (auto& f: m_geomMesh.f_ptrs) {
        emit sig_sendF(f.get());
    }
}

void MyGL::createSkeleton(QString filename) {

    QString contents;
    QFile file;
    file.setFileName(filename);

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    contents = file.readAll();
    file.close();

    QJsonDocument d = QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject object = d.object();
    QJsonObject json = object["root"].toObject();

    *loadJson(json, nullptr);
    emit sig_sendJoint(m_geomJoint.get());
}


Joint* MyGL::loadJson(QJsonObject json, Joint* parent) {

    QJsonArray translate = json["pos"].toArray();
    QJsonArray rotate = json["rot"].toArray();
    QJsonArray children = json["children"].toArray();

    QString n = json["name"].toString();

    uPtr<Joint> newJoint = mkU<Joint>(this);
    newJoint->name = n;
    newJoint->translate = glm::vec3(translate[0].toDouble(), translate[1].toDouble(), translate[2].toDouble());

    double angle = rotate[0].toDouble();
    glm::vec3 axis = glm::vec3(rotate[1].toDouble(), rotate[2].toDouble(), rotate[3].toDouble());
    newJoint->rotate = glm::quat(glm::angleAxis(float(angle),axis));

    newJoint->setText(0,QString(n));

    for (int i=0; i < children.size(); i++) {
        loadJson(children[i].toObject(), newJoint.get());
    }

    Joint* joint_ptr = newJoint.get();

    if (parent != nullptr) {
        parent -> children.push_back(std::move(newJoint));
        joint_ptr->parent = parent;
        parent->addChild(joint_ptr);
    } else {
        m_geomJoint = std::move(newJoint);
    }

    joint_ptr->create();

    return joint_ptr;
}


void MyGL::updateLists() {
    for (auto& v: m_geomMesh.v_ptrs) {
        emit sig_sendV(v.get());
    }

    for (auto& he: m_geomMesh.he_ptrs) {
        emit sig_sendHE(he.get());
    }

    for (auto& f: m_geomMesh.f_ptrs) {
        emit sig_sendF(f.get());
    }
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    m_progSkeleton.create(":/glsl/skeleton.vert.glsl", ":/glsl/skeleton.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    m_geomMesh.create();
    m_geomJoint.get() -> createFirst();

}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    m_progSkeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

void MyGL::drawJoints(Joint* parent) {

    for (uPtr<Joint>& child : parent->children) {
        m_progFlat.setModelMatrix(child->computeOverallT());
        m_progFlat.draw(*child.get());
        drawJoints(child.get());
    }

}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progSkeleton.setViewProjMatrix(m_glCamera.getViewProj());

    m_progFlat.setModelMatrix(glm::mat4(1.f));
    m_progLambert.setModelMatrix(glm::mat4(1.f));
    m_progSkeleton.setModelMatrix(glm::mat4(1.f));

    if (jointsLoaded) {
        m_progSkeleton.draw(m_geomMesh);
    } else {
        m_progFlat.draw(m_geomMesh);
    }

    glDisable(GL_DEPTH_TEST);

    m_progFlat.setModelMatrix(m_geomJoint.get()->computeOverallT());
    m_progFlat.draw(*m_geomJoint.get());
    drawJoints(m_geomJoint.get());

    m_progFlat.setModelMatrix(glm::mat4(1.f));
    if (m_vertDisplay.representedVertex != nullptr) {
        m_progFlat.draw(m_vertDisplay);
    }

    if (m_faceDisplay.representedFace != nullptr) {
        m_progFlat.draw(m_faceDisplay);
    }

    if (m_edgeDisplay.representedEdge != nullptr) {
        m_progFlat.draw(m_edgeDisplay);
    }

    glEnable(GL_DEPTH_TEST);
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.rotY(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.rotY(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.rotX(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.rotX(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    }

    switch(e->key()) {

        case Qt::Key_N :

        m_edgeDisplay.updateEdge(m_edgeDisplay.representedEdge->next);
        m_edgeDisplay.create();
        update();

        break;

        case Qt::Key_M :

        m_edgeDisplay.updateEdge(m_edgeDisplay.representedEdge->sym);
        m_edgeDisplay.create();
        update();

        break;

        case Qt::Key_F :

        m_faceDisplay.updateFace(m_edgeDisplay.representedEdge->face);
        m_faceDisplay.create();
        update();

        break;

        case Qt::Key_V :

        m_vertDisplay.updateVertex(m_edgeDisplay.representedEdge->between);
        m_vertDisplay.create();
        update();

        break;

        case Qt::Key_H :

        if(e->modifiers() & Qt::ShiftModifier) {

            // half-edge of face
            m_edgeDisplay.updateEdge(m_faceDisplay.representedFace->he);
            m_edgeDisplay.create();
            update();

        } else {

            // half-edge of vertex
            m_edgeDisplay.updateEdge(m_vertDisplay.representedVertex->he);
            m_edgeDisplay.create();
            update();
        }

        break;

    }

    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}
