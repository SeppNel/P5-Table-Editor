#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <string>

using namespace std;

void exportFile(const string& tablePath, const string& txtPath, MainWindow& a) {
    a.openfile(tablePath);
    a.saveAs(txtPath);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}