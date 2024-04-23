#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <string>
#include <readers/reader.hpp>
#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent) {
    meshViewer = std::make_unique<MeshViewer>(this);
    setCentralWidget(meshViewer.get());

    auto openAction = new QAction("Open", this);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    menuBar()->addAction(openAction);
}

MainWindow::~MainWindow() {
}

void MainWindow::openFile() {
    std::string filePath = QFileDialog::getOpenFileName(
        this,
        "Choose the mesh file",
        "",
        "Mesh Files (*.obj *.ply *.stl);;All Files (* *.*)"
    ).toStdString();

    try {
        TriangularMesh mesh = read_triangular_mesh(filePath);
        QMessageBox::information(
            this,
            "Triangular mesh was successfully read",
            QString("Size of mesh: %1 triangle(-s)")
                .arg(QString::number(mesh.size()))
        );
        meshViewer->setMesh(std::move(mesh));
    } catch (char const* error_msg) {
        QMessageBox::critical(this, "Can't read file", QString(error_msg));
    }
}
