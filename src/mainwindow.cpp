#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <QFileDialog>
#include "mesh.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    connect(ui->mygl, SIGNAL(sig_sendV(QListWidgetItem*)),
            this,
            SLOT(slot_addVert(QListWidgetItem*)));

    connect(ui->mygl, SIGNAL(sig_sendF(QListWidgetItem*)),
            this,
            SLOT(slot_addFace(QListWidgetItem*)));

    connect(ui->mygl, SIGNAL(sig_sendHE(QListWidgetItem*)),
            this,
            SLOT(slot_addEdge(QListWidgetItem*)));

    connect(ui -> mygl, SIGNAL(sig_sendJoint(QTreeWidgetItem*)),
            this,
            SLOT(slot_addJoint(QTreeWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::on_objButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(0, QString(""), QDir::currentPath().append(QString("../..")), QString("*.obj"));

    if ( !filename.isNull() ) {
        qDebug() << "selected file path :" << filename.toUtf8();
        ui->mygl->createMesh(filename);
    }
}

void MainWindow::slot_addVert(QListWidgetItem * item) {
    ui->vertsListWidget->addItem(item);
}

void MainWindow::slot_addEdge(QListWidgetItem * item) {
    ui->halfEdgesListWidget->addItem(item);
}

void MainWindow::slot_addFace(QListWidgetItem * item) {
    ui->facesListWidget->addItem(item);
}

void MainWindow::slot_addJoint(QTreeWidgetItem * item) {
    ui -> treeWidget -> addTopLevelItem(item);
}

void MainWindow::on_vertsListWidget_itemClicked(QListWidgetItem *item)
{
    ui -> mygl -> m_vertDisplay.updateVertex((Vertex*)item);
    ui -> mygl -> m_vertDisplay.create();
    ui -> mygl -> update();
}


void MainWindow::on_halfEdgesListWidget_itemClicked(QListWidgetItem *item)
{
    ui -> mygl -> m_edgeDisplay.updateEdge((HalfEdge*)item);
    ui -> mygl -> m_edgeDisplay.create();
    ui -> mygl -> update();
}


void MainWindow::on_facesListWidget_itemClicked(QListWidgetItem *item)
{
    ui -> mygl -> m_faceDisplay.updateFace((Face*)item);
    ui -> mygl -> m_faceDisplay.create();
    ui -> mygl -> update();
}


void MainWindow::on_faceRedSpinBox_valueChanged(double arg1)
{
    if (ui->mygl->m_faceDisplay.representedFace != nullptr) {
        ui->mygl->m_faceDisplay.representedFace->color.r = arg1;
        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> m_faceDisplay.create();
        ui -> mygl -> update();
    }
}


void MainWindow::on_faceGreenSpinBox_valueChanged(double arg1)
{
    if (ui->mygl->m_faceDisplay.representedFace != nullptr) {
        ui->mygl->m_faceDisplay.representedFace->color.g = arg1;
        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> m_faceDisplay.create();
        ui -> mygl -> update();
    }
}

void MainWindow::on_faceBlueSpinBox_valueChanged(double arg1)
{
    if (ui->mygl->m_faceDisplay.representedFace != nullptr) {
        ui->mygl->m_faceDisplay.representedFace->color.b = arg1;
        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> m_faceDisplay.create();
        ui -> mygl -> update();
    }
}

void MainWindow::on_vertPosXSpinBox_valueChanged(double arg1)
{
    if (ui -> mygl -> m_vertDisplay.representedVertex != nullptr) {
        ui -> mygl -> m_vertDisplay.representedVertex->pos[0] = arg1;
        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> m_vertDisplay.create();
        ui -> mygl -> update();
    }
}

void MainWindow::on_vertPosZSpinBox_valueChanged(double arg1)
{
    if (ui -> mygl -> m_vertDisplay.representedVertex != nullptr) {
        ui -> mygl -> m_vertDisplay.representedVertex->pos[1] = arg1;
        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> m_vertDisplay.create();
        ui -> mygl -> update();
    }
}

void MainWindow::on_vertPosYSpinBox_valueChanged(double arg1)
{
    if (ui -> mygl -> m_vertDisplay.representedVertex != nullptr) {
        ui -> mygl -> m_vertDisplay.representedVertex->pos[2] = arg1;
        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> m_vertDisplay.create();
        ui -> mygl -> update();
    }
}


void MainWindow::on_pushButton_clicked() // add vertex
{
    if (ui -> mygl -> m_edgeDisplay.representedEdge != nullptr) {

        HalfEdge* oldHalf = ui -> mygl -> m_edgeDisplay.representedEdge;
        uPtr<HalfEdge> newHalf = mkU<HalfEdge>();

        HalfEdge* oldHalfSym = ui -> mygl -> m_edgeDisplay.representedEdge->sym;
        uPtr<HalfEdge> newHalfSym = mkU<HalfEdge>();

        // set new pointers

        // syms
        newHalf->symWith(oldHalfSym);
        newHalfSym->symWith(oldHalf);

        // faces
        newHalf->face = oldHalf->face;
        newHalfSym->face = oldHalfSym->face;

        // next
        newHalf->next = oldHalf->next;
        newHalfSym->next = oldHalfSym->next;

        // vertex
        newHalfSym->setVert(oldHalfSym->between);
        newHalf->setVert(oldHalf->between);

        // update old pointers

        // next
        oldHalf->next = newHalf.get();
        oldHalfSym->next = newHalfSym.get();

        // vertex

        Vertex* end1 = oldHalf->findPrev();
        Vertex* end2 = newHalf -> between;

        uPtr<Vertex> midpoint = mkU<Vertex>(
                    glm::vec3(
                        (end1->pos[0] + end2->pos[0])/2,
                        (end1->pos[1] + end2->pos[1])/2,
                        (end1->pos[2] + end2->pos[2])/2));

        oldHalfSym->setVert(midpoint.get());
        oldHalf->setVert(midpoint.get());

        // update gui

        ui -> mygl -> m_geomMesh.he_ptrs.push_back(std::move(newHalf));
        ui -> mygl -> m_geomMesh.he_ptrs.push_back(std::move(newHalfSym));
        ui -> mygl -> m_geomMesh.v_ptrs.push_back(std::move(midpoint));

        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> updateLists();

        ui -> mygl -> m_edgeDisplay.updateEdge(oldHalf);
        ui -> mygl -> m_edgeDisplay.create();

        ui -> mygl -> update();

    }
}


void MainWindow::on_pushButton_2_clicked() // triangulate
{
    if (ui -> mygl -> m_faceDisplay.representedFace != nullptr) {

        HalfEdge* h0 = ui -> mygl -> m_faceDisplay.representedFace -> he;
        int n = h0->findVertsInFace();

        for (int i = 0; i < n - 2; i ++) {

            // step 1

            uPtr<HalfEdge> a = mkU<HalfEdge>();
            uPtr<HalfEdge> b = mkU<HalfEdge>();

            // step 2

            a->setVert(h0->between);
            b->setVert(h0->next->next->between);
            a->symWith(b.get());

            uPtr<Face> f = mkU<Face>();
            f->he = a.get();

            a->face = f.get();
            h0->next->face = f.get();
            h0->next->next->face = f.get();

            b->face = ui -> mygl -> m_faceDisplay.representedFace;

            // step 3

            b->next = h0->next->next->next;
            h0->next->next->next = a.get();
            a->next = h0->next;
            h0->next = b.get();

            // add to ptr lists yay

            ui -> mygl -> m_geomMesh.he_ptrs.push_back(std::move(a));
            ui -> mygl -> m_geomMesh.he_ptrs.push_back(std::move(b));
            ui -> mygl -> m_geomMesh.f_ptrs.push_back(std::move(f));

        }

        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> updateLists();
        ui -> mygl -> update();
    }
}

void MainWindow::on_pushButton_3_clicked() // subdivide
{
    ui -> mygl -> m_geomMesh.subdivide();
    ui -> mygl -> m_geomMesh.create();
    ui -> mygl -> updateLists();
    ui -> mygl -> update();
}


void MainWindow::on_pushButton_5_clicked() // load JSON
{
    QString filename = QFileDialog::getOpenFileName(0, QString(""), QDir::currentPath().append(QString("../..")), QString("*.json"));

    if ( !filename.isNull() ) {
        qDebug() << "selected file path :" << filename.toUtf8();
        ui->mygl->createSkeleton(filename);
    }

    ui -> mygl -> update();
}


void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    ui -> mygl -> selectedJoint = (Joint*)item;
    ui->mygl->m_geomJoint->createClicked(item);
    update_rlabel();
    ui -> mygl -> update();
    update_tlabel();
}

void MainWindow::on_translatej_x_valueChanged(double arg1)
{
    if (ui -> mygl -> selectedJoint != nullptr) {
        ui -> mygl -> selectedJoint->translate.x = arg1;
        ui-> mygl-> m_geomJoint->createClicked(ui -> mygl -> selectedJoint);

        glm::mat4 t[100];
        ui -> mygl -> m_geomJoint -> getAllTransformations(t);
        ui -> mygl -> m_progSkeleton.setTransformationMat(t);

        ui -> mygl -> update();
        update_tlabel();
    }
}

void MainWindow::on_translatej_y_valueChanged(double arg1)
{
    if (ui -> mygl -> selectedJoint != nullptr) {
        ui -> mygl -> selectedJoint->translate.y = arg1;
        ui-> mygl-> m_geomJoint->createClicked(ui -> mygl -> selectedJoint);

        glm::mat4 t[100];
        ui -> mygl -> m_geomJoint -> getAllTransformations(t);
        ui -> mygl -> m_progSkeleton.setTransformationMat(t);

        ui -> mygl -> update();
        update_tlabel();
    }
}

void MainWindow::on_translatej_z_valueChanged(double arg1)
{
    if (ui -> mygl -> selectedJoint != nullptr) {
        ui -> mygl -> selectedJoint->translate.z = arg1;
        ui-> mygl-> m_geomJoint->createClicked(ui -> mygl -> selectedJoint);

        glm::mat4 t[100];
        ui -> mygl -> m_geomJoint -> getAllTransformations(t);
        ui -> mygl -> m_progSkeleton.setTransformationMat(t);

        ui -> mygl -> update();
        update_tlabel();
    }
}



void MainWindow::on_pushButton_6_clicked() // rotate x
{
    if (ui -> mygl -> selectedJoint != nullptr) {
        ui -> mygl -> selectedJoint->rotate *= glm::quat(glm::angleAxis(0.5f, glm::vec3(1,0,0)));
        ui-> mygl-> m_geomJoint->createClicked(ui -> mygl -> selectedJoint);

        glm::mat4 t[100];
        ui -> mygl -> m_geomJoint -> getAllTransformations(t);
        ui -> mygl -> m_progSkeleton.setTransformationMat(t);

        ui -> mygl -> update();
        update_rlabel();

    }
}




void MainWindow::on_pushButton_7_clicked() // rotate y
{
    if (ui -> mygl -> selectedJoint != nullptr) {
        ui -> mygl -> selectedJoint->rotate *= glm::quat(glm::angleAxis(0.5f, glm::vec3(0,1,0)));
        ui-> mygl-> m_geomJoint->createClicked(ui -> mygl -> selectedJoint);

        glm::mat4 t[100];
        ui -> mygl -> m_geomJoint -> getAllTransformations(t);
        ui -> mygl -> m_progSkeleton.setTransformationMat(t);

        ui -> mygl -> update();
        update_rlabel();
    }
}




void MainWindow::on_pushButton_8_clicked() // rotate z
{
    if (ui -> mygl -> selectedJoint != nullptr) {
        ui -> mygl -> selectedJoint->rotate *= glm::quat(glm::angleAxis(0.5f, glm::vec3(0,0,1)));
        ui-> mygl-> m_geomJoint->createClicked(ui -> mygl -> selectedJoint);

        glm::mat4 t[100];
        ui -> mygl -> m_geomJoint -> getAllTransformations(t);
        ui -> mygl -> m_progSkeleton.setTransformationMat(t);

        ui -> mygl -> update();
        update_rlabel();
    }
}



void MainWindow::on_pushButton_4_clicked() // bind skeleton
{
    if (ui -> mygl -> m_geomJoint.get() != nullptr) {

        glm::mat4 t[100];
        glm::mat4 b[100];

        ui -> mygl -> jointsLoaded = true;

        ui -> mygl -> m_geomMesh.bind(ui -> mygl -> m_geomJoint.get());

        ui -> mygl -> m_geomJoint -> computeBind();

        ui -> mygl -> m_geomJoint -> getAllTransformations(t);
        ui -> mygl -> m_geomJoint -> getAllBinds(b);

        ui -> mygl -> m_progSkeleton.setTransformationMat(t);
        ui -> mygl -> m_progSkeleton.setBindMat(b);

        ui -> mygl -> m_geomMesh.create();
        ui -> mygl -> update();

    }
}

void MainWindow::update_rlabel() {
    float w = ui -> mygl -> selectedJoint -> rotate.w;
    float x = ui -> mygl -> selectedJoint -> rotate.x;
    float y = ui -> mygl -> selectedJoint -> rotate.y;
    float z = ui -> mygl -> selectedJoint -> rotate.z;
    ui -> quat_label -> setText("[" + QString::number(w,'f',2) + ", " + QString::number(x,'f',2) + ", "+ QString::number(y,'f',2) + ", " + QString::number(z,'f',2) + "]");
}

void MainWindow::update_tlabel() {
    float x = ui -> mygl -> selectedJoint -> translate.x;
    float y = ui -> mygl -> selectedJoint -> translate.y;
    float z = ui -> mygl -> selectedJoint -> translate.z;
    ui -> trans_label -> setText("[" + QString::number(x,'f',2) + ", "+ QString::number(y,'f',2) + ", " + QString::number(z,'f',2) + "]");
}

