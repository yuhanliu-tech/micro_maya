#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mesh.h"
#include <QListWidgetItem>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void on_objButton_clicked();

    void slot_addVert(QListWidgetItem*);

    void slot_addFace(QListWidgetItem*);

    void slot_addEdge(QListWidgetItem*);

    void slot_addJoint(QTreeWidgetItem*);

    void on_vertsListWidget_itemClicked(QListWidgetItem *item);

    void on_halfEdgesListWidget_itemClicked(QListWidgetItem *item);

    void on_facesListWidget_itemClicked(QListWidgetItem *item);

    void on_faceRedSpinBox_valueChanged(double arg1);

    void on_faceGreenSpinBox_valueChanged(double arg1);

    void on_faceBlueSpinBox_valueChanged(double arg1);

    void on_vertPosXSpinBox_valueChanged(double arg1);

    void on_vertPosZSpinBox_valueChanged(double arg1);

    void on_vertPosYSpinBox_valueChanged(double arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_translatej_x_valueChanged(double arg1);

    void on_translatej_y_valueChanged(double arg1);


    void on_translatej_z_valueChanged(double arg1);

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    void update_rlabel();
    void update_tlabel();

};


#endif // MAINWINDOW_H
