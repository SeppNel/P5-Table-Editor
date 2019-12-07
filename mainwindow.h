#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QKeyEvent>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void openfile(string ruta);

    void on_list_itemClicked(QListWidgetItem *item);


    void on_save_clicked();

private:
    Ui::MainWindow *ui;

protected:
    void keyPressEvent(QKeyEvent* pe);

};
#endif // MAINWINDOW_H
