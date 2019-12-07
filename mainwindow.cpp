#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <QStringList>
#include <QSignalMapper>

using namespace std;
string ruta;
char* memblock;
vector<string> juntohex;
int fin;
int lineas;
int currentline;
bool fileopen = false;
bool listitemclicked = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::keyPressEvent(QKeyEvent* pe)
{
if(pe->key() == Qt::Key_Return) on_save_clicked();
}

string int_to_hex(int i);
void savetosame(int leng, string traduc, int liemp, string ruta, char* memblock, int fin);
void savetoless(int leng, string traduc, int liemp, string ruta, char* memblock, int fin, int bileng, int linusu, int lineas);
void savetomore(int leng, string traduc, int liemp, string ruta, char* memblock, int fin, int bileng, int linusu, int lineas);

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file");
    ruta = filename.toUtf8().constData();
    ui->list->clear();
    openfile(ruta);
    fileopen = true;

}


string int_to_hex(int i)
{
    //Convert an int to a string with its hex value
    stringstream stream;
    stream << hex << i;
    string result(stream.str());
    //Then adapt the format to be 2 characters
    if (result.length() == 1) {
        result.insert(result.begin(), '0');
        return result;
    }
    else if (result.length() == 2) {
        return result;
    }
    else if (result.length() == 3) {
        return result;
    }
    else if (result.length() == 4) {
        return result;
    }
    else if (result[0, 1, 2, 3, 4, 5] == 'f') {
        result.erase(0, 6);
        return result;
    }
    else {
        exit(EXIT_FAILURE);
    }
}


void MainWindow::openfile(string ruta){
    ifstream file;
        file.open(ruta, ios::binary);
        if (file.is_open()) {
            //Save last address
            file.seekg(0, std::ios::end);
            fin = file.tellg();
            //Go back to the start
            file.seekg(0, std::ios::beg);
            // Memblock holds all the addreses of the file
            memblock = new char [fin];
            file.read(memblock, fin);
            file.close();

            //Save the number of lines
            int lin = memblock[15];
            string linhex = int_to_hex(lin);
            lineas = stoi(linhex, 0, 16); //Convert hex string to int (maybe this is not necesary but ¯\_(ツ)_/¯)
            juntohex.clear(); //Clear the array with the index addreses in case the user opens a new file
            int i = 0;
            int offset = 16; // Address where the index starts
            // Get all index and put it in  juntohex
            while( i < lineas ){
                int uno = memblock[offset];
                string suno = int_to_hex(uno);
                offset++;
                int dos = memblock[offset];
                string sdos = int_to_hex(dos);
                offset++;
                int tres = memblock[offset];
                string stres = int_to_hex(tres);
                offset++;
                int cuatro = memblock[offset];
                string scuatro = int_to_hex(cuatro);

                stringstream test;
                test << suno << sdos << stres << scuatro;
                string tojunto(test.str());
                juntohex.push_back(tojunto);
                offset++;
                i++;
            }

            int linusu;
            linusu = 0;
            vector<string> juntotext;
            stringstream test2;
            i = 0;
            while(i < lineas){
                int jint = stoi(juntohex[linusu], 0, 16);
                int bileng = jint + 7; //Address position that holds the lenght of the current line
                int leng = memblock[bileng];
                int listart = jint + 16;
                int index = 0;
                while (index < leng) {
                    char filine = memblock[listart];

                    test2 << filine; //Store all characters of the line and save them

                    listart++;
                    index++;
                }
                string tojuntotext(test2.str()); //String that holds the current full line
                QString Qjuntext = QString::fromUtf8(tojuntotext.c_str());
                QString array[lineas];
                array[i].append(Qjuntext); //Append the current line to the array
                ui->list->addItem(array[i]); //Add item to the listview
                test2.str("");
                test2.clear(); //Clear "test2" so it can hold the next line
                linusu++;
                i++;

            }
     }
}


void MainWindow::on_list_itemClicked(QListWidgetItem *item)
{
    QString lintotmp = ui->list->currentItem()->text();
    currentline = ui->list->currentRow();
    string lintoed = lintotmp.toUtf8().constData();
    ui->textedit->setText(lintotmp); // Add the selected item to the TextEdit
    listitemclicked = true;
}

void MainWindow::on_save_clicked()
{
    if (fileopen == false || listitemclicked == false){
        QMessageBox msgBox;
        msgBox.setText("You need to open and select a line.");
        msgBox.exec();
        return;
    }

    QString translation = ui->textedit->text();
    string traduc = translation.toUtf8().constData(); //Get the content of textedit and convert it to string

    int linusu = currentline;
    int jint = stoi(juntohex[linusu], 0, 16);
    int bileng = jint + 7;
    int leng = memblock[bileng];

    int liemp = jint + 16;
    if ((traduc.length() == 0)) {
        exit(EXIT_SUCCESS);
    }
    traduc = traduc + '\0';
    int newleng = traduc.length();
    if (newleng == leng) {
        savetosame(leng, traduc, liemp, ruta, memblock, fin);
        ui->list->clear();
        openfile(ruta);
    }
    else if (newleng < leng) {
        savetoless(leng, traduc, liemp, ruta, memblock, fin, bileng, linusu, lineas);
        ui->list->clear();
        openfile(ruta);
    }
    else {
        savetomore(leng, traduc, liemp, ruta, memblock, fin, bileng, linusu, lineas);
        ui->list->clear();
        openfile(ruta);
    }
}

void savetosame(int leng, string traduc, int liemp, string ruta, char* memblock, int fin)
{
    ofstream myfile;
    myfile.open(ruta, ios::binary | ios::trunc);
    if (myfile.is_open())
    {
        myfile.seekp(0, std::ios::beg);
        myfile.write(&memblock[0], liemp);
        myfile.write(&traduc[0], leng);
        myfile.write(&memblock[liemp + leng], fin - (liemp + leng));
        QMessageBox msgBox;
        msgBox.setText("Finished.");
        msgBox.exec();
        myfile.close();
    }
}

void savetoless(int leng, string traduc, int liemp, string ruta, char* memblock, int fin, int bileng, int linusu, int lineas)
{
    ofstream myfile;
    char nleng = traduc.length();
    int resto = leng - nleng;
    int findice = 16 + 4 * (linusu + 1); //Direccion del primer indice a modificar
    int poshex = memblock[findice] * 1024 + memblock[findice + 1] * 512 + memblock[findice + 2] * 256 + memblock[findice + 3];
    string linhex = int_to_hex(poshex);
    poshex = stoi(linhex, 0, 16);
    poshex = poshex - 16;
    int startingbit = memblock[findice - 2];
    QMessageBox msgBox;
    msgBox.setText("Finished.");
    string valres = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' , '\0' , '\0' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}; //Para añadir ceros, no se me ocurre nada mejor
    if (leng > 15) { // Si la frase original es mayor que 1 linea
        if (leng > 31) { // Si la frase original es mayor que 2 lineas

            if (nleng > 15 && nleng < 32) { //This is for when a sentence takes 3 lines and the new one only takes 2.
                myfile.open(ruta, ios::binary | ios::trunc);
                if (myfile.is_open())
                {
                    myfile.seekp(0, std::ios::beg);
                    myfile.write(&memblock[0], 10);

                    char lastlin = memblock[10];
                    char lastlind = memblock[11];
                    lastlind = lastlind - 16;
                    if (lastlind == -16) {
                        lastlin = lastlin - 1;
                    }
                    myfile.write(&lastlin, 1);
                    myfile.write(&lastlind, 1);

                    myfile.write(&memblock[12], findice - 12);

                    int i = 1;
                    int t = 4;
                    char newposhex = startingbit;
                    while (i < (lineas - linusu)) {
                        int tmp = memblock[findice + t - 5];
                        int tmpdos = memblock[findice + t - 1];
                        string tempura = int_to_hex(tmp);
                        tmp = stoi(tempura, 0, 16);
                        if (i != 1) {
                            tmp = tmp - 16;
                        }
                        string tempurados = int_to_hex(tmpdos);
                        tmpdos = stoi(tempurados, 0, 16);
                        tmpdos = tmpdos - 16;
                        if (tmp < 0) {
                            tmp = tmp + 256;
                        }
                        if (tmpdos < 0) {
                            tmpdos = tmpdos + 256;
                        }
                        if (tmp > tmpdos) {
                            newposhex++;
                        }
                        char dirindi = poshex;
                        char dirando = newposhex;
                        myfile.write(&valres[0], 2);
                        myfile.write(&dirando, 1);
                        myfile.write(&dirindi, 1);

                        poshex = memblock[findice + t] * 1024 + memblock[findice + t + 1] * 512 + memblock[findice + t + 2] * 256 + memblock[findice + t + 3];
                        t = t + 4;
                        linhex = int_to_hex(poshex);
                        poshex = stoi(linhex, 0, 16);
                        poshex = poshex - 16;


                        i++;
                    }
                    //Escribir despues de modificar el indice:

                    t = t - 4;
                    myfile.write(&memblock[findice + t], bileng - myfile.tellp()); //Escribe el original hasta el bit de longitud
                    myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                    myfile.write(&memblock[bileng + 1], liemp - myfile.tellp()); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                    myfile.write(&traduc[0], nleng); //Inserta la traduccion
                    int restlin = 32 - nleng;
                    myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                    //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                    int pls = myfile.tellp();
                    myfile.write(&memblock[pls + 16], fin - myfile.tellp() - 16);

                }
                msgBox.exec();
                myfile.close();
            }
            else if (nleng < 15) {
                myfile.open(ruta, ios::binary | ios::trunc);
                if (myfile.is_open())
                {
                    myfile.seekp(0, std::ios::beg);
                    myfile.write(&memblock[0], 10);

                    char lastlin = memblock[10];
                    char lastlind = memblock[11];
                    lastlind = lastlind - 32;
                    if ((lastlind == -32) || (lastlind == -16)) {
                        lastlin = lastlin - 1;
                    }
                    myfile.write(&lastlin, 1);
                    myfile.write(&lastlind, 1);

                    myfile.write(&memblock[12], findice - 12);

                    int i = 1;
                    int t = 4;
                    char newposhex = startingbit;
                    poshex = poshex - 16;
                    while (i < (lineas - linusu)) {
                        int tmp = memblock[findice + t - 5];
                        int tmpdos = memblock[findice + t - 1];
                        string tempura = int_to_hex(tmp);
                        tmp = stoi(tempura, 0, 16);
                        if (i != 1) {
                            tmp = tmp - 32;
                        }
                        string tempurados = int_to_hex(tmpdos);
                        tmpdos = stoi(tempurados, 0, 16);
                        tmpdos = tmpdos - 32;
                        if (tmp < 0) {
                            tmp = tmp + 256;
                        }
                        if (tmpdos < 0) {
                            tmpdos = tmpdos + 256;
                        }
                        if (tmp > tmpdos) {
                            newposhex++;
                        }
                        char dirindi = poshex;
                        char dirando = newposhex;
                        myfile.write(&valres[0], 2);
                        myfile.write(&dirando, 1);
                        myfile.write(&dirindi, 1);

                        poshex = memblock[findice + t] * 1024 + memblock[findice + t + 1] * 512 + memblock[findice + t + 2] * 256 + memblock[findice + t + 3];
                        t = t + 4;
                        linhex = int_to_hex(poshex);
                        poshex = stoi(linhex, 0, 16);
                        poshex = poshex - 32;

                        i++;
                    }
                    //Escribir despues de modificar el indice:

                    t = t - 4;
                    myfile.write(&memblock[findice + t], bileng - myfile.tellp()); //Escribe el original hasta el bit de longitud
                    myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                    myfile.write(&memblock[bileng + 1], liemp - myfile.tellp()); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                    myfile.write(&traduc[0], nleng); //Inserta la traduccion
                    int restlin = 16 - nleng;
                    myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                    //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                    int pls = myfile.tellp();
                    myfile.write(&memblock[pls + 32], fin - myfile.tellp() - 32);

                }
                msgBox.exec();
                myfile.close();
            }
            else {
                myfile.open(ruta, ios::binary | ios::trunc);
                if (myfile.is_open())
                {
                    myfile.seekp(0, std::ios::beg); //Puntero al inicio
                    myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
                    myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                    myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                    myfile.write(&traduc[0], nleng); //Inserta la traduccion
                    myfile.write(&valres[0], resto); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                    //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                    myfile.write(&memblock[bileng + 1 + (liemp - (bileng + 1)) + nleng + resto], fin - (bileng + 1 + (liemp - (bileng + 1)) + nleng + resto));
                    msgBox.exec();
                    myfile.close();
                }
            }
            return;
        }
        if (nleng < 16) { //This is for when a sentence takes 2 lines and the new one only takes one.
            myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg);
                myfile.write(&memblock[0], 10);

                char lastlin = memblock[10];
                char lastlind = memblock[11];
                lastlind = lastlind - 16;
                if (lastlind == -16 ) {
                    lastlin = lastlin - 1;
                }
                myfile.write(&lastlin, 1);
                myfile.write(&lastlind, 1);

                myfile.write(&memblock[12], findice - 12);

                int i = 1;
                int t = 4;
                char newposhex = startingbit;
                while (i < (lineas - linusu)) {
                    int tmp = memblock[findice + t - 5];
                    int tmpdos = memblock[findice + t - 1];
                    string tempura = int_to_hex(tmp);
                    tmp = stoi(tempura, 0, 16);
                    if (i != 1) {
                        tmp = tmp - 16;
                    }
                    string tempurados = int_to_hex(tmpdos);
                    tmpdos = stoi(tempurados, 0, 16);
                    tmpdos = tmpdos - 16;
                    if (tmp < 0) {
                        tmp = tmp + 256;
                    }
                    if (tmpdos < 0) {
                        tmpdos = tmpdos + 256;
                    }
                    if (tmp > tmpdos) {
                        newposhex++;
                    }
                    char dirindi = poshex;
                    char dirando = newposhex;
                    myfile.write(&valres[0], 2);
                    myfile.write(&dirando, 1);
                    myfile.write(&dirindi, 1);

                    poshex = memblock[findice + t] * 1024 + memblock[findice + t + 1] * 512 + memblock[findice + t + 2] * 256 + memblock[findice + t + 3];
                    t = t + 4;
                    linhex = int_to_hex(poshex);
                    poshex = stoi(linhex, 0, 16);
                    poshex = poshex - 16;

                    i++;
                }
                //Escribir despues de modificar el indice:

                t = t - 4;
                myfile.write(&memblock[findice + t], bileng - myfile.tellp()); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - myfile.tellp()); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                int restlin = 16 - nleng;
                myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                int pls = myfile.tellp();
                myfile.write(&memblock[pls + 16], fin - myfile.tellp() - 16);

            }
            msgBox.exec();
            myfile.close();
        }
        else {
            myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg); //Puntero al inicio
                myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                myfile.write(&valres[0], resto); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                myfile.write(&memblock[bileng + 1 + (liemp - (bileng + 1)) + nleng + resto], fin - (bileng + 1 + (liemp - (bileng + 1)) + nleng + resto));
                msgBox.exec();
                myfile.close();
            }
        }
    }

    else { //This is for when a sentence takes 1 lines and the new one too.
        myfile.open(ruta, ios::binary | ios::trunc);
        if (myfile.is_open())
        {
            myfile.seekp(0, std::ios::beg); //Puntero al inicio
            myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
            myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
            myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
            myfile.write(&traduc[0], nleng); //Inserta la traduccion
            myfile.write(&valres[0], resto); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
            //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
            myfile.write(&memblock[bileng + 1 + (liemp - (bileng + 1)) + nleng + resto], fin - (bileng + 1 + (liemp - (bileng + 1)) + nleng + resto));
            msgBox.exec();
            myfile.close();
        }
    }
}

void savetomore(int leng, string traduc, int liemp, string ruta, char* memblock, int fin, int bileng, int linusu, int lineas)
{
    ofstream myfile;
    char nleng = traduc.length();
    int findice = 16 + 4 * (linusu + 1); //Direccion del primer indice a modificar
    int poshex = memblock[findice] * 1024 + memblock[findice + 1] * 512 + memblock[findice + 2] * 256 + memblock[findice + 3];
    string linhex = int_to_hex(poshex);
    poshex = stoi(linhex, 0, 16);
    poshex = poshex + 16;
    int startingbit = memblock[findice - 2];
    QMessageBox msgBox;
    msgBox.setText("Finished.");
    string valres = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' , '\0' , '\0' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' }; //Para añadir ceros, no se me ocurre nada mejor
    if (leng > 15 && leng < 32) { // Si la frase original ocupa 2 lineas
        if (nleng > 47) {
            msgBox.setText("New line takes 4 lines or more. Not supported for now.");
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
        else if (nleng < 48 && nleng > 31) {
                myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg);
                myfile.write(&memblock[0], 10);

                char lastlin = memblock[10];
                char lastlind = memblock[11];
                lastlind = lastlind + 16;
                if (lastlind == 0) {
                    lastlin = lastlin + 1;
                }
                myfile.write(&lastlin, 1);
                myfile.write(&lastlind, 1);

                myfile.write(&memblock[12], findice - 12);

                int i = 1;
                int t = 4;
                char newposhex = startingbit;
                while (i < (lineas - linusu)) {
                    int tmp = memblock[findice + t - 5];
                    int tmpdos = memblock[findice + t - 1];
                    string tempura = int_to_hex(tmp);
                    tmp = stoi(tempura, 0, 16);
                    if (i != 1) {
                        tmp = tmp + 16;
                    }
                    string tempurados = int_to_hex(tmpdos);
                    tmpdos = stoi(tempurados, 0, 16);
                    tmpdos = tmpdos + 16;
                    if (tmp > 255) {
                        tmp = tmp - 256;
                    }
                    if (tmpdos > 255) {
                        tmpdos = tmpdos - 256;
                    }
                    if (tmp > tmpdos) {
                        newposhex++;
                    }
                    char dirindi = poshex;
                    char dirando = newposhex;
                    myfile.write(&valres[0], 2);
                    myfile.write(&dirando, 1);
                    myfile.write(&dirindi, 1);

                    poshex = memblock[findice + t] * 1024 + memblock[findice + t + 1] * 512 + memblock[findice + t + 2] * 256 + memblock[findice + t + 3];
                    t = t + 4;
                    linhex = int_to_hex(poshex);
                    poshex = stoi(linhex, 0, 16);
                    poshex = poshex + 16;

                    i++;
                }
                //Escribir despues de modificar el indice:

                t = t - 4;
                myfile.write(&memblock[findice + t], bileng - myfile.tellp()); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - myfile.tellp()); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                int restlin = 48 - nleng;
                myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                int pls = myfile.tellp();
                myfile.write(&memblock[pls - 16], fin - myfile.tellp() + 16);

            }
            msgBox.exec();
            myfile.close();
        }
        else {
            myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg); //Puntero al inicio
                myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                myfile.write(&memblock[myfile.tellp()], fin - myfile.tellp());
                msgBox.exec();
                myfile.close();
            }
        }
    }
    else if (leng > 31) { // Si la frase original ocupa 3 lineas
        if (nleng > 47) {
            msgBox.setText("New line takes 4 lines or more. Not supported for now.");
            msgBox.exec();
                exit(EXIT_FAILURE);
        }
        else {
            myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg); //Puntero al inicio
                myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                myfile.write(&memblock[myfile.tellp()], fin - myfile.tellp());
                msgBox.exec();
                myfile.close();
            }
        }
    }
    else { // Si la frase original ocupa 1 lineas
        if (nleng > 15 && nleng < 32) { // Y la nueva 2
            myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg);
                myfile.write(&memblock[0], 10);

                char lastlin = memblock[10];
                char lastlind = memblock[11];
                lastlind = lastlind + 16;
                if (lastlind == 0) {
                    lastlin = lastlin + 1;
                }
                myfile.write(&lastlin, 1);
                myfile.write(&lastlind, 1);

                myfile.write(&memblock[12], findice - 12);

                int i = 1;
                int t = 4;
                char newposhex = startingbit;
                while (i < (lineas - linusu)) {
                    int tmp = memblock[findice + t - 5];
                    int tmpdos = memblock[findice + t - 1];
                    string tempura = int_to_hex(tmp);
                    tmp = stoi(tempura, 0, 16);
                    if (i != 1) {
                        tmp = tmp + 16;
                    }
                    string tempurados = int_to_hex(tmpdos);
                    tmpdos = stoi(tempurados, 0, 16);
                    tmpdos = tmpdos + 16;
                    if (tmp > 255) {
                        tmp = tmp - 256;
                    }
                    if (tmpdos > 255) {
                        tmpdos = tmpdos - 256;
                    }
                    if (tmp > tmpdos) {
                        newposhex++;
                    }
                    char dirindi = poshex;
                    char dirando = newposhex;
                    myfile.write(&valres[0], 2);
                    myfile.write(&dirando, 1);
                    myfile.write(&dirindi, 1);

                    poshex = memblock[findice + t] * 1024 + memblock[findice + t + 1] * 512 + memblock[findice + t + 2] * 256 + memblock[findice + t + 3];
                    t = t + 4;
                    linhex = int_to_hex(poshex);
                    poshex = stoi(linhex, 0, 16);
                    poshex = poshex + 16;

                    i++;
                }
                //Escribir despues de modificar el indice:

                t = t - 4;
                myfile.write(&memblock[findice + t], bileng - myfile.tellp()); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - myfile.tellp()); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                int restlin = 32 - nleng;
                myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                int pls = myfile.tellp();
                myfile.write(&memblock[pls - 16], fin - myfile.tellp() + 16);

            }
            msgBox.exec();
            myfile.close();
        }
        else if (nleng > 31) { // Y la nueva 3
            myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg);
                myfile.write(&memblock[0], 10);

                char lastlin = memblock[10];
                char lastlind = memblock[11];
                lastlind = lastlind + 32;
                if ((lastlind == 16) || (lastlind == 0)) {
                    lastlin = lastlin + 1;
                }
                myfile.write(&lastlin, 1);
                myfile.write(&lastlind, 1);

                myfile.write(&memblock[12], findice - 12);

                int i = 1;
                int t = 4;
                char newposhex = startingbit;
                poshex = poshex + 16;
                while (i < (lineas - linusu)) {
                    int tmp = memblock[findice + t - 5];
                    int tmpdos = memblock[findice + t - 1];
                    string tempura = int_to_hex(tmp);
                    tmp = stoi(tempura, 0, 16);
                    if (i != 1) {
                        tmp = tmp + 32;
                    }
                    string tempurados = int_to_hex(tmpdos);
                    tmpdos = stoi(tempurados, 0, 16);
                    tmpdos = tmpdos + 32;
                    if (tmp > 255) {
                        tmp = tmp - 256;
                    }
                    if (tmpdos > 255) {
                        tmpdos = tmpdos - 256;
                    }
                    if (tmp > tmpdos) {
                        newposhex++;
                    }
                    char dirindi = poshex;
                    char dirando = newposhex;
                    myfile.write(&valres[0], 2);
                    myfile.write(&dirando, 1);
                    myfile.write(&dirindi, 1);

                    poshex = memblock[findice + t] * 1024 + memblock[findice + t + 1] * 512 + memblock[findice + t + 2] * 256 + memblock[findice + t + 3];
                    t = t + 4;
                    linhex = int_to_hex(poshex);
                    poshex = stoi(linhex, 0, 16);
                    poshex = poshex + 32;

                    i++;
                }
                //Escribir despues de modificar el indice:

                t = t - 4;
                myfile.write(&memblock[findice + t], bileng - myfile.tellp()); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - myfile.tellp()); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                int restlin = 48 - nleng;
                myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                int pls = myfile.tellp();
                myfile.write(&memblock[pls - 32], fin - myfile.tellp() + 32);

            }
            msgBox.exec();
            myfile.close();
        }
        else {
            myfile.open(ruta, ios::binary | ios::trunc);
            if (myfile.is_open())
            {
                myfile.seekp(0, std::ios::beg); //Puntero al inicio
                myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
                myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
                myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
                myfile.write(&traduc[0], nleng); //Inserta la traduccion
                //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                myfile.write(&memblock[myfile.tellp()], fin - myfile.tellp());
                msgBox.exec();
                myfile.close();
            }
        }
    }
}
