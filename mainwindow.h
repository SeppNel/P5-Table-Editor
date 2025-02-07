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

public:
    void saveAs(const string& txtPath);
    void importfromCLI(string txtFile);
    void openfile(string ruta);
    void on_save_clicked(string traduc);
    void on_save_clicked_with_save_button();

private slots:
    void on_actionOpen_triggered();
    //void openfile(string ruta);

    void on_list_itemClicked();
    void on_delete_element_clicked();

private:
    Ui::MainWindow *ui;

protected:
    void keyPressEvent(QKeyEvent* pe);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void changed(const QMimeData *mimeData = nullptr);
};
#endif // MAINWINDOW_H
