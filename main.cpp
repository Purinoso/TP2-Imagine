#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include "filters.h" // Incluye el archivo de encabezado "filters.h"
#include <thread>
#include <unistd.h>
#include <fstream>
#include <map>
#include <variant>
#include <functional>

using namespace std;

int main(int argc, char *argv[])
{
    if (string(argv[1]) == "-help")
    {
        cout << "Uso: ./main <filtro> <nthreads> <[p1]> <img1> <custom_output> <[p2]> <img2>" << endl;
        return 0;
    }

    // Definición del mapa de las funciones

    map<string, function<ppm(map<string, VariantArg> &)>> functionMap;
    functionMap["plain"] = plain;
    functionMap["blackWhite"] = blackWhite;
    functionMap["contrast"] = contrast;
    functionMap["brightness"] = brightness;
    functionMap["shades"] = shades;
    functionMap["mergeFilter"] = mergeFilter;
    functionMap["boxBlur"] = boxBlur;
    functionMap["edgeDetection"] = edgeDetection;
    functionMap["sharpen"] = sharpen;

    // Se reciben los parámetros pasados desde main.cpp y se procesan
    // Después se guardan en un mapa que almacena claves en string y valores de tipos variantes.

    string filterName = string(argv[1]);
    unsigned int nthreads = atoi(argv[2]);
    float p1 = atof(argv[3]);
    string imgStr1(argv[4]);
    string out = string(argv[5]);
    string imgStr2(argv[7]);

    ppm img1(imgStr1);
    ppm img2(imgStr2);

    map<string, VariantArg> argsMap;

    function<ppm(map<string, VariantArg> &)> chosenFilter = functionMap.at(filterName);

    if (nthreads == 1)
    {
        argsMap["p1"] = p1;
        argsMap["out"] = out;
        argsMap["img1"] = img1;
        argsMap["img2"] = img2;

        ppm imgResult = chosenFilter(argsMap);
        imgResult.write(out);
    }
    else if (nthreads > 1)
    {
        thread threads[nthreads];
        map<string, VariantArg> argsMaps[nthreads];

        ppm imgResult = img1;

        if (filterName == "merge")
        {
            vector<ppm> imgs2perThread = threadsImageDivision(img2, nthreads);

            for (int threadIndex = 0; threadIndex < nthreads; threadIndex++)
            {
                argsMaps[threadIndex]["img2"] = imgs2perThread[threadIndex];
            }
        }

        vector<ppm> imgs1perThread = threadsImageDivision(img1, nthreads);

        for (int threadIndex = 0; threadIndex < nthreads; threadIndex++)
        {   
            argsMaps[threadIndex]["p1"] = p1;
            argsMaps[threadIndex]["out"] = out;
            argsMaps[threadIndex]["img1"] = imgs1perThread[threadIndex];
            
            string nombreArchivo = "out/cebra" + to_string(threadIndex) + ".ppm";
            imgs1perThread[threadIndex].write(nombreArchivo);

            threads[threadIndex] = thread(applyFilterPerThread, chosenFilter, ref(argsMaps[threadIndex]), ref(imgResult), threadIndex);
        }           

        for (int threadIndex = 0; threadIndex < nthreads; threadIndex++) {
            threads[threadIndex].join();
        }

        imgResult.write(out);
    }
    else
    {
        cout << "Sos un boludo qué querés que te diga flaco.";
    }

    cout << "Listo" << endl;
    return 0;
}