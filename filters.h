#ifndef FILTERS_H
#define FILTERS_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <atomic>
#include <variant>
#include <map>
#include "ppm.h"

// DECLARACIÓN DEL TIPO DE DATO ARGUMENTO VARIANTE QUE RECIBIRÁ CADA KEY DEL MAPA DE LOS FILTROS

using VariantArg = variant<string, unsigned int, float, ppm>;

// FILTROS SINGLE-THREAD

/*
void blackWhite(ppm &img);                              // implementado
void contrast(ppm &img, float contrast);                // implementado
void brightness(ppm &img, float b, int start, int end); // implementado
void shades(ppm &img, unsigned char shades);            // implementado
void merge(ppm &img1, ppm &img2, float alpha);          // implementado
void boxBlur(ppm &img);                                 // implementado
void edgeDetection(ppm &img);                           // implementado
void sharpen(ppm &img);                                 // implementado
void plain(ppm &img, unsigned char c);                  // implementado
*/

ppm plain(map<string, VariantArg> &argsMap);         // IMPLEMENTADO
ppm blackWhite(map<string, VariantArg> &argsMap);    // IMPLEMENTADO
ppm contrast(map<string, VariantArg> &argsMap);      // IMPLEMENTADO
ppm brightness(map<string, VariantArg> &argsMap);    // IMPLEMENTADO
ppm shades(map<string, VariantArg> &argsMap);        // IMPLEMENTADO
ppm mergeFilter(map<string, VariantArg> &argsMap);         // IMPLEMENTADO
ppm boxBlur(map<string, VariantArg> &argsMap);       // IMPLEMENTADO
ppm edgeDetection(map<string, VariantArg> &argsMap); // IMPLEMENTADO
ppm sharpen(map<string, VariantArg> &argsMap);       // IMPLEMENTADO

// FILTROS MULTI-THREAD

// void applyFilter(char *argv[]);
// void applyFilterMultiThread();
vector<ppm> threadsImageDivision(ppm &img, int threads);

#endif