#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <QStringList>
#include <QSignalMapper>
#include <regex>

using namespace std;
string ruta;
char* memblock;
vector<string> juntohex;
int fin;
int lineas;
string linhexplus;
int currentline;
bool fileopen = false;
bool listitemclicked = false;
bool ctd = false;
int startingByte;
int blockLength;

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
    if(pe->key() == Qt::Key_Return) on_save_clicked(); //Save Hotkey
    //if(pe->key() == Qt::Key_Shift) on_list_itemClicked(); // Select Hotckey (Really, fucking shift. You didn't have any other key in the damn keyboard)
}

string int_to_hex(int i);
void savetosame(int leng, string traduc, int liemp, string ruta, char* memblock, int fin);
void savetoless(int leng, string traduc, int liemp, string ruta, char* memblock, int fin, int bileng, int linusu, int lineas);
void savetomore(int leng, string traduc, int liemp, string ruta, char* memblock, int fin, int bileng, int linusu, int lineas);
void saveCtd(string traduc, string ruta, char* memblock);

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file","",tr("FTD files (*.ftd *.ctd)"));
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

        int version = memblock[13]; //Get ftd version
        int linp = memblock[14]; // Get complete number of lines, bakka!
        if (linp > 0){
            linhexplus =  int_to_hex(linp);
        }
        else{
            linhexplus = ""; //Reset if new file open
        }
        int lin = memblock[15]; //Save the number of lines
        string linhex = int_to_hex(lin);
        linhex = linhexplus + linhex;
        lineas = stoi(linhex, 0, 16); //Convert hex string to int (maybe this is not necesary but ¯\_(ツ)_/¯)
        juntohex.clear(); //Clear the array with the index addreses in case the user opens a new file

        if (lineas == 1){
            ctd = true;
        }

        int i = 0;
        int offset = 16; // Address where the index starts
        // Get all index and put it in  juntohex
        if (ctd == false){
            while( i < lineas )
            {
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
        }
        else{
            lineas = stoi(int_to_hex(memblock[43]), 0, 16);; //Entries of ctd files
            linp = memblock[38]; // Get actual number of bytes
            lin = memblock[39];
            linhexplus =  int_to_hex(linp);
            linhex = int_to_hex(lin);
            linhex = linhexplus + linhex;
            int bytes = stoi(linhex, 0, 16); //Total bytes of ctd files


            blockLength = bytes / lineas;
        }

        int linusu;
        int bileng;
        int listart;
        linusu = 0;
        if (ctd == true){
            startingByte = 48;
        }

        vector<string> juntotext;
        stringstream test2;
        i = 0;
        int leng;
        while(i < lineas)
        {
            if (ctd == false){
                int jint = stoi(juntohex[linusu], 0, 16);
                if (version == 0){
                    bileng = jint + 7; //Address position that holds the lenght of the current line
                }
                else{
                    bileng = jint; //Address position that holds the lenght of the current line
                }
                if (ctd == true){
                    bileng = blockLength;
                }
                leng = memblock[bileng];
                if( version == 0){
                    listart = jint + 16;
                }
                else{
                    listart = jint + 4;
                }
            }
            else{
                leng = blockLength;
                listart = startingByte;
                startingByte = startingByte + blockLength;
            }
            int index = 0;
            while (index < leng)
            {
                char filine = memblock[listart];
                test2 << filine; //Store all characters of the line and save them
                listart++;
                index++;
            }
            string tojuntotext(test2.str()); //String that holds the current full line

            //cout << tojuntotext << endl;
            //Check for tildes by hex code of backported font
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x84\x96"), "á");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x84\x9E"), "é");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x84\xA2"), "í");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x84\xA8"), "ó");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x84\xAE"), "ú");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x84\xA6"), "ñ");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x83\xF7"), "Á");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x84\x8F"), "Ú");
            //regular letters after special character, idk why
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x80\xC1"), "a");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x80\xC2"), "b");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x80\xC3"), "c");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x80\xC4"), "d");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x80\xC5"), "e");
            tojuntotext = regex_replace(tojuntotext, std::regex("\\\x80\xC6"), "f");

            QString Qjuntext = QString::fromStdString(tojuntotext);
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


void MainWindow::on_list_itemClicked()
{
    QString lintotmp = ui->list->currentItem()->text();
    currentline = ui->list->currentRow();
    string lintoed = lintotmp.toStdString();
    ui->textedit->setText(lintotmp); // Add the selected item to the TextEdit
    listitemclicked = true;
}

void MainWindow::on_save_clicked()
{
    if (fileopen == false || listitemclicked == false){
        QMessageBox msgBox;
        msgBox.setText("You need to open a file and select a line.");
        msgBox.exec();
        return;
    }

    QString translation = ui->textedit->text();
    string traduc = translation.toStdString(); //Get the content of textedit and convert it to string
    cout << traduc << endl;


    if ((traduc.length() == 0)) {
        return;
    }

    //Check for tildes by hex code of backported font
    traduc = regex_replace(traduc, std::regex("\\á"), "\x84\x96");
    traduc = regex_replace(traduc, std::regex("\\é"), "\x84\x9E");
    traduc = regex_replace(traduc, std::regex("\\í"), "\x84\xA2");
    traduc = regex_replace(traduc, std::regex("\\ó"), "\x84\xA8");
    traduc = regex_replace(traduc, std::regex("\\ú"), "\x84\xAE");
    traduc = regex_replace(traduc, std::regex("\\ñ"), "\x84\xA6");
    traduc = regex_replace(traduc, std::regex("\\Á"), "\x83\xF7");
    traduc = regex_replace(traduc, std::regex("\\Ú"), "\x84\x8F");

    if (ctd){
        saveCtd(traduc, ruta, memblock);
        ui->list->clear();
        openfile(ruta);
    }
    else{
        int linusu = currentline;
        int jint = stoi(juntohex[linusu], 0, 16);
        int bileng;
        int version = memblock[13]; //Get ftd version
        if( version == 0){
            bileng = jint + 7; //Address position that holds the lenght of the current line
        }
        else{
            bileng = jint; //Address position that holds the lenght of the current line
        }
        int leng = memblock[bileng];
        int liemp;
        if( version == 0){
            liemp = jint + 16;
        }
        else{
            liemp = jint + 4;
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
        msgBox.setText("Saved");
        msgBox.exec();
        myfile.close();
    }
}

void savetoless(int leng, string traduc, int liemp, string ruta, char* memblock, int fin, int bileng, int linusu, int lineas)
{
    ofstream myfile;
    char nleng = traduc.length();
    int resto = leng - nleng;
    int findice = 16 + 4 * (linusu + 1); //Address of the first index to modify
    int poshex = memblock[findice] * 1024 + memblock[findice + 1] * 512 + memblock[findice + 2] * 256 + memblock[findice + 3];
    string linhex = int_to_hex(poshex);
    poshex = stoi(linhex, 0, 16);
    poshex = poshex - 16;
    int startingbit = memblock[findice - 2];
    QMessageBox msgBox;
    msgBox.setText("Saved");
    string valres = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' , '\0' , '\0' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}; //Para añadir ceros, no se me ocurre nada mejor
    int version = memblock[13]; //Get ftd version
    if (version == 0){
        if (leng > 16) { // Si la frase original es mayor que 1 linea
            if (leng > 32) { // Si la frase original es mayor que 2 lineas
                if (nleng > 16 && nleng <= 32) { //This is for when a sentence takes 3 lines and the new one only takes 2.
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
                        while (i < (lineas - linusu))
                        {
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
                        int version = memblock[13]; //Get ftd version
                        if (version == 1){
                            restlin = 28 - nleng;
                        }
                        myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                        //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                        int pls = myfile.tellp();
                        myfile.write(&memblock[pls + 16], fin - myfile.tellp() - 16);

                    }
                    msgBox.exec();
                    myfile.close();
                }
                else if (nleng <= 16) {
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
                        while (i < (lineas - linusu))
                        {
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
                        int version = memblock[13]; //Get ftd version
                        if (version == 1){
                            restlin = 12 - nleng;
                        }
                        myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he hecho.
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
            if (nleng <= 16) { //This is for when a sentence takes 2 lines and the new one only takes one.
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 12 - nleng;
                    }
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
    else{
        if (leng > 12) { // Si la frase original es mayor que 1 linea
            if (leng > 28) { // Si la frase original es mayor que 2 lineas
                if (nleng > 12 && nleng <= 28) { //This is for when a sentence takes 3 lines and the new one only takes 2.
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
                        while (i < (lineas - linusu))
                        {
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
                        int version = memblock[13]; //Get ftd version
                        if (version == 1){
                            restlin = 28 - nleng;
                        }
                        myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                        //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                        int pls = myfile.tellp();
                        myfile.write(&memblock[pls + 16], fin - myfile.tellp() - 16);

                    }
                    msgBox.exec();
                    myfile.close();
                }
                else if (nleng <= 12) {
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
                        while (i < (lineas - linusu))
                        {
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
                        int version = memblock[13]; //Get ftd version
                        if (version == 1){
                            restlin = 12 - nleng;
                        }
                        myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he hecho.
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
            if (nleng <= 12) { //This is for when a sentence takes 2 lines and the new one only takes one.
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 12 - nleng;
                    }
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
    msgBox.setText("Saved");
    string valres = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' , '\0' , '\0' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' }; //Para añadir ceros, no se me ocurre nada mejor
    int version = memblock[13]; //Get ftd version
    if (version == 0){
        if (leng > 16 && leng <= 32) { // Si la frase original ocupa 2 lineas
            if (nleng > 47) {
                msgBox.setText("New line takes 4 lines or more. Not supported for now.");
                msgBox.exec();
                exit(EXIT_FAILURE);
            }
            else if (nleng <= 48 && nleng > 32) {
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 44 - nleng;
                    }
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
        else if (leng > 32) { // Si la frase original ocupa 3 lineas
            if (nleng > 48) {
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
            if (nleng > 16 && nleng <= 32) { // Y la nueva 2
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 28 - nleng;
                    }
                    myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                    //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                    int pls = myfile.tellp();
                    myfile.write(&memblock[pls - 16], fin - myfile.tellp() + 16);

                }
                msgBox.exec();
                myfile.close();
            }
            else if (nleng > 32) { // Y la nueva 3
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 44 - nleng;
                    }
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
    else{
        if (leng > 12 && leng <= 28) { // Si la frase original ocupa 2 lineas
            if (nleng > 43) {
                msgBox.setText("New line takes 4 lines or more. Not supported for now.");
                msgBox.exec();
                exit(EXIT_FAILURE);
            }
            else if (nleng <= 44 && nleng > 28) {
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 44 - nleng;
                    }
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
        else if (leng > 28) { // Si la frase original ocupa 3 lineas
            if (nleng > 44) {
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
            if (nleng > 12 && nleng <= 28) { // Y la nueva 2
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 28 - nleng;
                    }
                    myfile.write(&valres[0], restlin); //Como es de menor tamaño, rellena los bits faltantes con el array ese chungo que he echo.
                    //Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
                    int pls = myfile.tellp();
                    myfile.write(&memblock[pls - 16], fin - myfile.tellp() + 16);

                }
                msgBox.exec();
                myfile.close();
            }
            else if (nleng > 32) { // Y la nueva 3
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
                    while (i < (lineas - linusu))
                    {
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
                    int version = memblock[13]; //Get ftd version
                    if (version == 1){
                        restlin = 44 - nleng;
                    }
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
}

void saveCtd(string traduc, string ruta, char* memblock){
    int diff = blockLength - traduc.length();
    string valres = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' , '\0' , '\0' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}; //Para añadir ceros, no se me ocurre nada mejor
    int posByte = blockLength * currentline + 48;

    ofstream myfile;
    myfile.open(ruta, ios::binary | ios::trunc);
    if (myfile.is_open())
    {
        myfile.seekp(0, std::ios::beg);
        myfile.write(&memblock[0], posByte);
        myfile.write(&traduc[0], traduc.length());
        int i = 0;
        while (i < diff){
            myfile.write(&valres[0], 1);
            i++;
        }
        myfile.write(&memblock[posByte + traduc.length() + diff], fin - (posByte + traduc.length() + diff));
        QMessageBox msgBox;
        msgBox.setText("Saved");
        msgBox.exec();
        myfile.close();
    }
}













