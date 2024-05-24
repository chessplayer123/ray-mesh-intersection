#include "mainwindow.hpp"

#include <QMenuBar>
#include <QMessageBox>
#include <QShortcut>
#include <QFileDialog>
#include <fstream>
#include <string>

#include "rmilib/reader.hpp"

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent) {
    meshViewer = std::make_unique<MeshViewer>(this);
    setCentralWidget(meshViewer.get());
    meshViewer->setFocusPolicy(Qt::StrongFocus);

    auto openAction = new QAction("Open", this);
    menuBar()->addAction(openAction);
    openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
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
        TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filePath);
        meshViewer->setMesh(std::move(mesh));
    } catch (char const* error_msg) {
        QMessageBox::critical(this, "Can't read file", QString(error_msg));
    }
}
