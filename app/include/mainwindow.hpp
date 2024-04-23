#pragma once

#include <QMainWindow>
#include <memory>
#include "mesh_viewer.hpp"

class MainWindow: public QMainWindow {
public:
    MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();
    void openFile();
private:
    std::unique_ptr<MeshViewer> meshViewer;
};
