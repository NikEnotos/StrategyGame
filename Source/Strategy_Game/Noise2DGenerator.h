// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>

struct Point
{
	float x;
	float y;
};

class STRATEGY_GAME_API Noise2DGenerator
{
public:
	
	Noise2DGenerator(int widthX, int heightY, int friquency, int seedIn = 0, bool seamlessVertically = false, bool seamlessHorizontally = false, bool useCosLerp = false, float minThreshold = -1.0, float maxThreshold = 1.0);

	inline int getSeed() { return seed; }
	//inline void setSeed(int newSeed) { seed = newSeed; }

	float getValueAtPoint(int x, int y);

	int Width;
	int Height;

private:

	int seed; // The seed is used to initialize the state of the generator and determines the sequence of random numbers it produces.
	int f; // Friquency

	std::vector<std::vector<float>> noise2DArray;

	float fadeFunction(float pos);

	float dotProduct(Point A, Point B);

	float thresholdsSetup(float value, float minThreshold, float maxThreshold);

	float fadeLerp(float fract, float A, float B);
	float cosLerp(float fract, float A, float B);
};
