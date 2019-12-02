#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;



string int_to_hex(int i)
{
	//Convierte el stream a string
	stringstream stream;
	stream << hex << i;
	string result(stream.str());
	//Comprueba dependiendo de la longitud que hacer
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
		cout << "Something happened and the program crashed, pls check your file";
		exit(EXIT_FAILURE);
	}
}

string getFileExtension(string filePath)
{
	// Create a Path object from given string
	fs::path path {filePath};
	// Check if file name in the path object has extension
	if (path.has_extension()) {
		// Fetch the extension from path object and return
		return path.extension().string();
	}
	// In case of no extension return empty string
	return "";
}

void savetosame(int leng, string traduc, int liemp, string ruta, char* memblock, int end)
{
	ofstream myfile;
	myfile.open(ruta, ios::binary | ios::trunc);
	if (myfile.is_open())
	{
		myfile.seekp(0, std::ios::beg);
		myfile.write(&memblock[0], liemp);
		myfile.write(&traduc[0], leng);
		myfile.write(&memblock[liemp + leng], end - (liemp + leng));
		cout << "Finished.";
		myfile.close();
	}
}

void savetoless(int leng, string traduc, int liemp, string ruta, char* memblock, int end, int bileng, int linusu, vector<string> juntohex, int lineas)
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
	string valres = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' , '\0' , '\0' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}; //Para añadir ceros, no se me ocurre nada mejor
	if (leng > 15) { // Si la frase original es mayor que 1 linea
		if (leng > 31) { // Si la frase original es mayor que 2 lineas
			cout << "Old takes 3 lines";

			if (nleng > 15 && nleng < 32) { //This is for when a sentence takes 3 lines and the new one only takes 2.
				cout << " and the new one only 2\n";
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
					myfile.write(&memblock[pls + 16], end - myfile.tellp() - 16);

				}
				cout << "Finished.";
				myfile.close();
			}
			else if (nleng < 15) {
				cout << " and the new one only 1\n";
				myfile.open(ruta, ios::binary | ios::trunc);
				if (myfile.is_open())
				{
					myfile.seekp(0, std::ios::beg);
					myfile.write(&memblock[0], 10);

					char lastlin = memblock[10];
					char lastlind = memblock[11];
					lastlind = lastlind - 32;
					if ((lastlind == -32) | (lastlind == -16)) {
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
					myfile.write(&memblock[pls + 32], end - myfile.tellp() - 32);

				}
				cout << "Finished.";
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
					myfile.write(&memblock[bileng + 1 + (liemp - (bileng + 1)) + nleng + resto], end - (bileng + 1 + (liemp - (bileng + 1)) + nleng + resto));
					cout << "Finished.";
					myfile.close();
				}
			}
			return;
		}
		cout << "Old one takes 2 lines";
		if (nleng < 16) { //This is for when a sentence takes 2 lines and the new one only takes one.
			cout << " and the new one only 1\n";
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
				myfile.write(&memblock[pls + 16], end - myfile.tellp() - 16);
				
			}
			cout << "Finished.";
			myfile.close();
		}
		else {
			cout << " and the new one too.\n";
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
				myfile.write(&memblock[bileng + 1 + (liemp - (bileng + 1)) + nleng + resto], end - (bileng + 1 + (liemp - (bileng + 1)) + nleng + resto));
				cout << "Finished.";
				myfile.close();
			}
		}
	}

	else { //This is for when a sentence takes 1 lines and the new one too.
		cout << "Old one takes 1 line, new one too.\n";
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
			myfile.write(&memblock[bileng + 1 + (liemp - (bileng + 1)) + nleng + resto], end - (bileng + 1 + (liemp - (bileng + 1)) + nleng + resto));
			cout << "Finished.";
			myfile.close();
		}
	}
}

void savetomore(int leng, string traduc, int liemp, string ruta, char* memblock, int end, int bileng, int linusu, vector<string> juntohex, int lineas)
{
	ofstream myfile;
	char nleng = traduc.length();
	int resto = leng - nleng;
	int findice = 16 + 4 * (linusu + 1); //Direccion del primer indice a modificar
	int poshex = memblock[findice] * 1024 + memblock[findice + 1] * 512 + memblock[findice + 2] * 256 + memblock[findice + 3];
	string linhex = int_to_hex(poshex);
	poshex = stoi(linhex, 0, 16);
	poshex = poshex + 16;
	int startingbit = memblock[findice - 2];
	string valres = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' , '\0' , '\0' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' }; //Para añadir ceros, no se me ocurre nada mejor
	if (leng > 15 && leng < 32) { // Si la frase original ocupa 2 lineas
		if (nleng > 47) {
			cout << "Old one takes 2 lines and you managed to make it 4 lines or more. Wow!\n";
			cout << "Not supported for now.";
			exit(EXIT_FAILURE);
		}
		else if (nleng < 48 && nleng > 31) {
			cout << "Old one takes 2 lines and the new one takes 3.\n";
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
				myfile.write(&memblock[pls - 16], end - myfile.tellp() + 16);

			}
			cout << "Finished.";
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
				myfile.write(&memblock[myfile.tellp()], end - myfile.tellp());
				cout << "Finished.";
				myfile.close();
			}
		}
	}
	else if (leng > 31) { // Si la frase original ocupa 3 lineas
		if (nleng > 47) {
			cout << "Old one takes 3 lines and you managed to make it 4.\n";
			cout << "Not implemented for now.";
				exit(EXIT_FAILURE);
		}
		else {
			cout << "Old one takes 2 lines and the new one too.\n";
			myfile.open(ruta, ios::binary | ios::trunc);
			if (myfile.is_open())
			{
				myfile.seekp(0, std::ios::beg); //Puntero al inicio
				myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
				myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
				myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
				myfile.write(&traduc[0], nleng); //Inserta la traduccion
				//Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
				myfile.write(&memblock[myfile.tellp()], end - myfile.tellp());
				cout << "Finished.";
				myfile.close();
			}
		}
	}
	else { // Si la frase original ocupa 1 lineas
		cout << "Old one takes 1 line";
		if (nleng > 15 && nleng < 32) { // Y la nueva 2
			cout << " and the new one takes 2.\n";
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
				myfile.write(&memblock[pls - 16], end - myfile.tellp() + 16);

			}
			cout << "Finished.";
			myfile.close();
		}
		else if (nleng > 31) { // Y la nueva 3
			cout << " and the new one takes 3.\n";
			myfile.open(ruta, ios::binary | ios::trunc);
			if (myfile.is_open())
			{
				myfile.seekp(0, std::ios::beg);
				myfile.write(&memblock[0], 10);

				char lastlin = memblock[10];
				char lastlind = memblock[11];
				lastlind = lastlind + 32;
				if ((lastlind == 16) | (lastlind == 0)) {
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
				myfile.write(&memblock[pls - 32], end - myfile.tellp() + 32);

			}
			cout << "Finished.";
			myfile.close();
		}
		else {
			cout << " and the new one too.\n";
			myfile.open(ruta, ios::binary | ios::trunc);
			if (myfile.is_open())
			{
				myfile.seekp(0, std::ios::beg); //Puntero al inicio
				myfile.write(&memblock[0], bileng); //Escribe el original hasta el bit de longitud
				myfile.write(&nleng, 1); //Escribe el tamaño de memoria del tamaño de la translation
				myfile.write(&memblock[bileng + 1], liemp - (bileng + 1)); //Sigue escribiendo el ori hasta el liemp (restando lo ya avanzado)
				myfile.write(&traduc[0], nleng); //Inserta la traduccion
				//Para la posicion de memblock es la suma de todos los tamaños anteriores, y para el tamaño es "end" - el memblock.
				myfile.write(&memblock[myfile.tellp()], end - myfile.tellp());
				cout << "Finished.";
				myfile.close();
			}
		}
	}
}


int main()
{
	cout << "File path:\n";
	string ruta;
	getline(cin, ruta);
	//ruta = "C:\\Users\\josen\\Desktop\\kfe_mission_title.ftd";
	cout << "Path: " << ruta << endl;
	if (getFileExtension(ruta) != ".ftd") {
		cout << "Only 'ftd' files allowed" << endl;
		cin.get();
		exit(EXIT_FAILURE);
	}
	inicio:
	ifstream file;
	char* memblock;
	file.open(ruta, ios::binary);
	if (file.is_open()) {
		//Direccion del principio
		streampos start = file.tellg();
		//Direccion del final
		file.seekg(0, std::ios::end);
		int end = file.tellg();
		cout << "Size: " << end << endl;
		//Volver al principio
		file.seekg(0, std::ios::beg);

		// Memblock indica la direccion y end el tamaño
		memblock = new char [end];
		file.read(memblock, end);
		file.close();

		//Guardar numero de lineas
		int lin = memblock[15];
		string linhex = int_to_hex(lin);
		int lineas = stoi(linhex, 0, 16); //Pasar de hex string a decimal int
		cout << "Number of lines: " << lineas << "\n";

		int i = 0;
		vector<string> juntohex;
		int offset = 16; // Posicion inicia del indice
		// Bucle para coger todos los indices
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
			juntohex.push_back(tojunto); //Aqui se añade el valor unido al array en el vector
			offset++;
			i++;
		};
		//Elegir linea a mostrar
		int linusu;
		cout << "Line to edit (From 0 to "<< lineas -1 << "): ";
		cin >> linusu;
		if (linusu > lineas -1) {
			cout << "The file doesn't have that many lines\n\n";
			goto inicio;
		}

		//Mirar la posicion que especifica la longitud y la guardo en leng
		int jint = stoi(juntohex[linusu], 0, 16);
		int bileng = jint + 7;
		int leng = memblock[bileng];
		int listart = jint + 16;
		int index = 0;
		while (index < leng) {
			char filine = memblock[listart];
			cout << filine;
			listart++;
			index++;
		}

		cout << "\n";
		int liemp = jint + 16;
		string traduc;
		cout << "\nNew line (enter 'f' to leave):";
		cin.ignore();
		getline(cin, traduc);
		if ((traduc.length() == 0) | traduc == "f") {
			cout << "Program closed succesfully";
			exit(EXIT_SUCCESS);
		}
		traduc = traduc + '\0';
		int newleng = traduc.length();
		if (newleng == leng) {
			cout << "Same number of characters\n";
			savetosame(leng, traduc, liemp, ruta, memblock, end);
			cin.get();
			cout << "\n";
		}
		else if (newleng < leng) {
			cout << "Less number of characters\n" << endl;
			savetoless(leng, traduc, liemp, ruta, memblock, end, bileng, linusu, juntohex, lineas);
			cin.get();
			cout << "\n";
		}
		else {
			cout << "More number of character\n";
			savetomore(leng, traduc, liemp, ruta, memblock, end, bileng, linusu, juntohex, lineas);
			cin.get();
			cout << "\n";
		}

	}
	else {
		cout << "Can't open file\n";
		cin.get();
		exit(EXIT_FAILURE);
	}
	goto inicio;
}
