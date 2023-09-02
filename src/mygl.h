#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include "mesh.h"
#include "halfedge.h"
#include "display.h"
#include "joint.h"
#include "smartpointerhelp.h"


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void createMesh(QString filename);
    void createSkeleton(QString filename);
    void updateLists();
    Joint* loadJson(QJsonObject, Joint*);

    VertexDisplay m_vertDisplay;
    EdgeDisplay m_edgeDisplay;
    FaceDisplay m_faceDisplay;

    void drawJoints(Joint* parent);

    Mesh m_geomMesh;
    uPtr<Joint> m_geomJoint;

    Joint *selectedJoint;

    ShaderProgram m_progSkeleton;

    bool jointsLoaded;


protected:
    void keyPressEvent(QKeyEvent *e);

signals:

    void sig_sendV(QListWidgetItem*);
    void sig_sendHE(QListWidgetItem*);
    void sig_sendF(QListWidgetItem*);
    void sig_sendJoint(QTreeWidgetItem*);


};


#endif // MYGL_H
