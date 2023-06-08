#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <thread>
#include <atomic>
#include <cmath>
#include <map>
#include <functional>
#include <variant>

#include "filters.h"
#include "ppm.h"

using namespace std;

vector<ppm> threadsImageDivision(ppm &img, int threads)
{
    int threadWidth = img.width / threads;
    int restPixel = img.width % threads;

    vector<ppm> vecImages;

    int initialPosX = restPixel == 0 ? 0 : -1 - threadWidth;

    for (int threadIndex = 0; threadIndex < threads; threadIndex++)
    {
        int currentThreadWidth = threadWidth;

        if (restPixel > 0)
        {
            currentThreadWidth++;
            restPixel--;
        }

        initialPosX += currentThreadWidth;

        ppm threadImage(currentThreadWidth, img.height);

        for (unsigned int y = 0; y < img.height; y++)
        {
            for (int x = initialPosX; x < initialPosX + currentThreadWidth; x++)
            {
                threadImage.setPixel(y, x - initialPosX, img.getPixel(y, x));
            }
        }
    }

    return vecImages;
}

ppm plain(map<string, VariantArg> &argsMap)
{
    ppm img = *get_if<ppm>(&argsMap.at("img1"));
    float c = *get_if<float>(&argsMap.at("p1"));

    for (unsigned int i = 0; i < img.height; i++)
        for (unsigned int j = 0; j < img.width; j++)
            img.setPixel(i, j, pixel(c, c, c));

    return img;
}

ppm blackWhite(map<string, VariantArg> &argsMap)
{
    ppm img = *get_if<ppm>(&argsMap.at("img1"));

    for (unsigned int i = 0; i < img.height; i++)
    {
        for (unsigned int j = 0; j < img.width; j++)
        {
            pixel p = img.getPixel(i, j);
            int g = (p.r + p.g + p.b) / 3;
            pixel np(g, g, g);
            img.setPixel(i, j, np);
        }
    }

    return img;
}

ppm contrast(map<string, VariantArg> &argsMap)
{
    ppm img = *get_if<ppm>(&argsMap.at("img1"));
    float contrast = *get_if<float>(&argsMap.at("p1"));

    float c = (259.f * (contrast + 255.f)) / (255.f * (259.f - contrast));
    for (unsigned int i = 0; i < img.height; i++)
    {
        for (unsigned int j = 0; j < img.width; j++)
        {
            pixel p = img.getPixel(i, j);
            int r = c * (p.r - 128) + 128;
            int g = c * (p.g - 128) + 128;
            int b = c * (p.b - 128) + 128;
            pixel np(r, g, b);
            img.setPixel(i, j, np.truncate());
        }
    }

    return img;
}

ppm brightness(map<string, VariantArg> &argsMap)
{
    ppm img = *get_if<ppm>(&argsMap.at("img1"));
    float b = *get_if<float>(&argsMap.at("p1"));

    for (unsigned int i = 0; i < img.height; i++)
    {
        for (unsigned int j = 0; j < img.width; j++)
        {
            pixel p = img.getPixel(i, j);
            int nr = p.r + 255 * b;
            int ng = p.g + 255 * b;
            int nb = p.b + 255 * b;
            pixel np(nr, ng, nb);
            img.setPixel(i, j, np.truncate());
        }
    }

    return img;
}

ppm shades(map<string, VariantArg> &argsMap)
{
    ppm img = *get_if<ppm>(&argsMap.at("img1"));
    unsigned char shades = *get_if<float>(&argsMap.at("p1"));

    for (unsigned int i = 0; i < img.height; i++)
    {
        for (unsigned int j = 0; j < img.width; j++)
        {
            pixel p = img.getPixel(i, j);
            int range = 255 / (shades - 1);
            int gp = (p.r + p.g + p.b) / 3;
            int g = (gp / range) * range;
            pixel np(g, g, g);
            img.setPixel(i, j, np);
        }
    }

    return img;
}

ppm mergeFilter(map<string, VariantArg> &argsMap)
{
    ppm img1 = *get_if<ppm>(&argsMap.at("img1"));
    ppm img2 = *get_if<ppm>(&argsMap.at("img2"));
    float p1 = *get_if<float>(&argsMap.at("p1"));

    for (unsigned int i = 0; i < img1.height; i++)
    {
        for (unsigned int j = 0; j < img1.width; j++)
        {
            pixel pixel1 = img1.getPixel(i, j);
            pixel pixel2 = img2.getPixel(i, j);
            int nr = (pixel1.r * p1 + pixel2.r * (1 - p1));
            int ng = (pixel1.g * p1 + pixel2.g * (1 - p1));
            int nb = (pixel1.b * p1 + pixel2.b * (1 - p1));
            pixel np(nr, ng, nb);
            img1.setPixel(i, j, np);
        }
    }

    return img1;
}

ppm boxBlur(map<string, VariantArg> &argsMap)
{
    ppm img = *get_if<ppm>(&argsMap.at("img1"));

    float kernel[] = {1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9};
    for (unsigned int i = 1; i < img.height - 1; i++)
    {
        for (unsigned int j = 1; j < img.width - 1; j++)
        {
            pixel np;
            pixel pixels[] = {
                img.getPixel(i - 1, j), img.getPixel(i - 1, j + 1),
                img.getPixel(i, j - 1), img.getPixel(i, j), img.getPixel(i, j + 1),
                img.getPixel(i + 1, j - 1), img.getPixel(i + 1, j), img.getPixel(i + 1, j + 1)};

            for (int k = 0; k < 9; k++)
            {
                pixels[k].mult(kernel[k]);
                np.addp(pixels[k]);
            }
            // pixels[0].addp(pixels[1].addp(pixels[2].addp(pixels[3].addp(pixels[4].addp(pixels[5].addp(pixels[6].addp(pixels[7].addp(pixels[8]))))))));
            img.setPixel(i, j, np);
        }
    }

    return img;
}

ppm edgeDetection(map<string, VariantArg> &argsMap)
{
    blackWhite(argsMap);
    boxBlur(argsMap);

    ppm img = *get_if<ppm>(&argsMap.at("img1"));
    ppm img_s = *get_if<ppm>(&argsMap.at("img1"));

    int kernel[] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
    int kernel_t[] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    for (unsigned int i = 1; i < img.height - 1; i++)
    {
        for (unsigned int j = 1; j < img.width - 1; j++)
        {
            pixel np;
            pixel temp;
            pixel temp_d;
            pixel pixels[] = {
                img.getPixel(i - 1, j - 1), img.getPixel(i - 1, j), img.getPixel(i - 1, j + 1),
                img.getPixel(i, j - 1), img.getPixel(i, j), img.getPixel(i, j + 1),
                img.getPixel(i + 1, j - 1), img.getPixel(i + 1, j), img.getPixel(i + 1, j + 1)};
            pixel pixels_s[] = {
                img_s.getPixel(i - 1, j - 1), img_s.getPixel(i - 1, j), img_s.getPixel(i - 1, j + 1),
                img_s.getPixel(i, j - 1), img_s.getPixel(i, j), img_s.getPixel(i, j + 1),
                img_s.getPixel(i + 1, j - 1), img_s.getPixel(i + 1, j), img_s.getPixel(i + 1, j + 1)};
            for (int k = 0; k < 9; k++)
            {
                pixels[k].mult(kernel[k]);
                temp.addp(pixels[k]);
                pixels_s[k].mult(kernel_t[k]);
                temp_d.addp(pixels_s[k]);
            }
            temp.r = temp.r * temp.r;
            temp_d.r = temp_d.r * temp_d.r;
            np = temp.addp(temp_d);
            int sqrt_r = sqrt(np.r);
            np.r = sqrt_r;
            np.g = sqrt_r;
            np.b = sqrt_r;
            img.setPixel(i, j, np);
        }
    }

    return img;
}

ppm sharpen(map<string, VariantArg> &argsMap)
{
    ppm img = *get_if<ppm>(&argsMap["img1"]);

    int kernel[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
    for (unsigned int i = 1; i < img.height - 1; i++)
    {
        for (unsigned int j = 1; j < img.width - 1; j++)
        {
            pixel np;
            pixel pixels[] = {
                img.getPixel(i - 1, j - 1), img.getPixel(i - 1, j), img.getPixel(i - 1, j + 1),
                img.getPixel(i, j - 1), img.getPixel(i, j), img.getPixel(i, j + 1),
                img.getPixel(i + 1, j - 1), img.getPixel(i + 1, j), img.getPixel(i + 1, j + 1)};
            for (int k = 0; k < 9; k++)
            {
                pixels[k].mult(kernel[k]);
                np.addp(pixels[k]);
            }
            np.truncate();
            img.setPixel(i, j, np);
        }
    }

    return img;
}

void applyFilterPerThread(function<ppm(map<string, VariantArg> &)> chosenFilter, map<string, VariantArg> &argsMap, ppm &imgGlobal, int initialPosX)
{
    ppm imgResult = chosenFilter(argsMap);

    for (unsigned int y = 0; y < imgResult.height; y++)
    {
        for (unsigned int x = 0; x < imgResult.width; x++)
        {
            imgGlobal.setPixel(y, x + initialPosX, imgResult.getPixel(y, x));
        }
    }
}