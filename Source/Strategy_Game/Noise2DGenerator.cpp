// Fill out your copyright notice in the Description page of Project Settings.

#include <iostream>
#include <random> 
#include "Noise2DGenerator.h"

Noise2DGenerator::Noise2DGenerator(int widthX, int heightY, int friquency, int seedIn, bool useCosLerp, float minThreshold, float maxThreshold)
{
    std::vector<float> row(widthX, 0);
    std::vector<std::vector<float>> tempArray(heightY, row);
    noise2DArray = tempArray; // Creating of resulted noise array

    f = friquency;

    // Setting seed in the object
    if (seedIn == 0)
    {
        std::random_device rd;
        seed = rd();
    }
    else seed = seedIn;


    const int gridStepX = widthX / f;
    const int gridStepY = heightY / f;

    // All possible vectors for basic noise
    Point possibleVectors[]{ {1.0,1.0}, {-1.0,1.0}, {1.0,-1.0}, {-1.0,-1.0}, {std::sqrt(2),0}, {0,std::sqrt(2)}, {-std::sqrt(2),0}, {0,-std::sqrt(2)} };  //1.4142

    // Initialization of array with basic noise grid
    std::vector<std::vector<Point>> initial2DNoiseGrid(gridStepY + 1, std::vector<Point>(gridStepX + 1));
    for (int y = 0; y < gridStepY + 1; y++) //initial2DNoiseGrid[0].size()
    {
        for (int x = 0; x < gridStepX + 1; x++)
        {
            std::mt19937 firstRandomization(seed * (x + 1) + y);
            std::mt19937 secondRandomization(firstRandomization() * (y + 1) + x);
            std::mt19937 randChoise(secondRandomization());
            initial2DNoiseGrid[x][y] = possibleVectors[randChoise() % std::size(possibleVectors)];


            // DEBUG
            //std::cout << initial2DNoiseGrid[x][y].x << " " << initial2DNoiseGrid[x][y].y << "\t";
            //if (x == initial2DNoiseGrid.size() - 1) {std::cout << " initial2DNoiseGrid.size() = " << initial2DNoiseGrid.size()<< " initial2DNoiseGrid[0].size() = " << initial2DNoiseGrid[0].size(); std::cout << "\n"; }
        }
    }

    // Generate Perlin noise value for each element of noise2DArray
    for (int y = 0; y < heightY; y++) //noise2DArray[0].size()
    {
        for (int x = 0; x < widthX; x++)
        {
            // Creating 4 vectors from 4 nearest base noise dots to pixel position
            Point vectorLT;
            vectorLT.x = (x % f) * float(1 / float(f - 1));
            vectorLT.y = (y % f) * float(1 / float(f - 1));

            Point vectorRT;
            vectorRT.x = ((x % f) - f + 1) * float(1 / float(f - 1));
            vectorRT.y = (y % f) * float(1 / float(f - 1));

            Point vectorLB;
            vectorLB.x = (x % f) * float(1 / float(f - 1));
            vectorLB.y = ((y % f) - f + 1) * float(1 / float(f - 1));

            Point vectorRB;
            vectorRB.x = ((x % f) - f + 1) * float(1 / float(f - 1));
            vectorRB.y = ((y % f) - f + 1) * float(1 / float(f - 1));


            // Doing dot product of 4 nearest base noise dots and previous 4 vectors
            float LTDp = dotProduct(vectorLT, initial2DNoiseGrid[int(x / f)][int(y / f)]);
            float RTDp = dotProduct(vectorRT, initial2DNoiseGrid[int(x / f) + 1][int(y / f)]);
            float LBDp = dotProduct(vectorLB, initial2DNoiseGrid[int(x / f)][int(y / f) + 1]);
            float RBDp = dotProduct(vectorRB, initial2DNoiseGrid[int(x / f) + 1][int(y / f) + 1]);

            // Choose method of interpolation
            if (useCosLerp) {
                // Finding interpolation with fade for "top" two dot products  
                float UpLerp = cosLerp((x % f) * float(1 / float(f - 1)), LTDp, RTDp);
                // Finding interpolation with fade for "bottom" two dot products     
                float DownLerp = cosLerp((x % f) * float(1 / float(f - 1)), LBDp, RBDp);
                // Finding result value for Perling noise in position x y  
                float finalLerp = cosLerp((y % f) * float(1 / float(f - 1)), UpLerp, DownLerp, minThreshold, maxThreshold);

                // Set value of Perling noise in position to array
                noise2DArray[x][y] = finalLerp;
            }
            else {
                // Finding interpolation with fade for "top" two dot products
                float UpLerp = fadeLerp((x % f) * float(1 / float(f - 1)), LTDp, RTDp);
                // Finding interpolation with fade for "bottom" two dot products
                float DownLerp = fadeLerp((x % f) * float(1 / float(f - 1)), LBDp, RBDp);
                // Finding result value for Perling noise in position x y
                float finalLerp = fadeLerp((y % f) * float(1 / float(f - 1)), UpLerp, DownLerp);

                // Set value of Perling noise in position to array
                noise2DArray[x][y] = finalLerp;
            }
        }
    }
}

float Noise2DGenerator::dotProduct(Point A, Point B)
{
    return (A.x * B.x) + (A.y * B.y);
}

float Noise2DGenerator::fadeLerp(float fract, float A, float B)
{
    return A + fadeFunction(fract) * (B - A);
}

float Noise2DGenerator::cosLerp(float fract, float A, float B, float minResult, float maxResult)
{
    float ft = fract * 3.1415927f;
    float c = (1.0f - std::cos(ft)) * 0.5f;
    float result = A * (1.0f - c) + B * c;
    result = minResult + (result + 1.0f) * 0.5f * (maxResult - minResult);
    return result;
}

float Noise2DGenerator::fadeFunction(float pos)
{
    if (pos < 0 || pos > 1)
    {
        std::cout << "ERROR: pos = " << pos << ". It MUST be 0 < pos < 1!";
        return -1.0;
    }

    return 6 * std::pow(pos, 5) - 15 * std::pow(pos, 4) + 10 * std::pow(pos, 3);
}

float Noise2DGenerator::getValueAtPoint(int x, int y)
{
    return noise2DArray[x][y];
}