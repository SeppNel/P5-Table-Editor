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
    if (argc == 4 && string(argv[1]) == "export") {
        string tablePath = argv[2];
        string txtPath = argv[3];
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        a.exec();
        return 0;
    }
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}