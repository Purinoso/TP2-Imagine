#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <vector>

#include "ppm.h"

// Funcion auxiliar para truncar un pixel a valores entre 0 a 255
unsigned char truncate_pixel(float color) { 
    return (color > 255) ? 255 : ((color < 0) ? 0: (unsigned char)color); 
}

// Init valores por defecto
void ppm::init(int _width, int _height) {
    width = _width;
    height = _height;
    max_col_val = 255;
    size = _width*_height;
}

// Crea el objeto PPM
ppm::ppm() {
    init(0, 0);
}

ppm::ppm(int _width, int _height){
    init(_width,_height);
    bitmap.resize(height);
    for(unsigned int i = 0; i<height;i++)
        bitmap[i].resize(width);
}

// Crea un objeto PPM y lo inicializa con la info de fname
ppm::ppm(const  string &fname) {
    init(0,0);
    read(fname);
}

// Lee la imagen PPM de fname
void ppm::read(const  string &fname) {
    ifstream inp(fname.c_str(),  ios::in |  ios::binary);
    if (inp.is_open()) {
        string line;
        
        // Obtiene el Header
        getline(inp, line);
        if (line != "P6") {
             cout << "Error. Unrecognized file format." <<  endl;
            return;
        }

        // Ignora los comentarios
        getline(inp, line);
        while (line[0] == '#') {
             getline(inp, line);
        }
        
        // Obtiene las dimensiones de la imagen
        stringstream dimensions(line);
        try {
            dimensions >> width;
            dimensions >> height;
            nr_lines = height;
            nr_columns = width;
        } catch ( exception &e) {
             cout << "Header file format error. " << e.what() <<  endl;
            return;
        }

        // Obtiene el maximo valor de color
        getline(inp, line);
        stringstream max_val(line);
        try {
            max_val >> max_col_val;
        } catch ( exception &e) {
             cout << "Header file format error. " << e.what() <<  endl;
            return;
        }
        size = width*height;

        // Inicializa la matriz que representa la imagen
        bitmap.resize(height);
        for(unsigned int i = 0; i<height;i++)
            bitmap[i].resize(width);
        
        // Itera y escribe la informacion en la matriz
        char aux;
        for (unsigned int i = 0; i < size; ++i) {
            inp.read(&aux, 1);
            bitmap[i/width][i%width].r = (unsigned char)aux;
            inp.read(&aux, 1);
            bitmap[i/width][i%width].g = (unsigned char)aux;
            inp.read(&aux, 1);
            bitmap[i/width][i%width].b = (unsigned char)aux;
        }

    } else {
         cout << "Error. Unable to open " << fname <<  endl;
    }
    inp.close();
}

// Escribe la imagen PPM de fname
void ppm::write(const  string &fname) {
    ofstream inp(fname.c_str(), ios::out | ios::binary);
    if (inp.is_open()) {

        inp << "P6\n";
        inp << width;
        inp << " ";
        inp << height << "\n";
        inp << max_col_val << "\n";

        char aux;

        for (unsigned int i = 0; i < height; ++i) {
            for(unsigned int j = 0; j < width;j++){
                aux = (char) bitmap[i][j].r;
                inp.write(&aux, 1);
                aux = (char) bitmap[i][j].g;
                inp.write(&aux, 1);
                aux = (char) bitmap[i][j].b;
                inp.write(&aux, 1);
            }
        }

    } else {
         cout << "Error. Unable to open " << fname <<  endl;
    }
    inp.close();
}
