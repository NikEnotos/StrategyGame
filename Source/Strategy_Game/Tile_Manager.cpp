// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile_Manager.h"
#include "Noise2DGenerator.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ATile_Manager::ATile_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATile_Manager::BeginPlay()
{
	Super::BeginPlay();
	
	HexGrid2DArray.SetNumZeroed(MapWidth);
	for (int32 i = 0; i < HexGrid2DArray.Num(); i++)
	{
		HexGrid2DArray[i].SetNumZeroed(MapHeight);
	}

	// Initialize noise
	Noise2DGenerator Noise(MapWidth, MapHeight, Friquency, Seed, CosInterpolation);

	for (int32 y = 0; y < MapHeight; y++)
	{
		for (int32 x = 0; x < MapWidth; x++)
		{
			// Set position of each tile
			const float xPos = y % 2 == 1 ? (x * HorizontalOffset) + OddHorizontalOffset : x * HorizontalOffset; // if it`s a odd row we add extra horizontal offset
			const float yPos = y * VerticalOffset;

			// Spawn tile at the position
			ATile* newTile = GetWorld()->SpawnActor<ATile>(TileType, FVector(xPos, yPos, 0), FRotator::ZeroRotator);
			newTile->SetTileIndex(FIntPoint(x, y));
			newTile->setTileNoiseValue(Noise.getValueAtPoint(x, y));

			newTile->SetTileColour(1.0-((Noise.getValueAtPoint(x, y)+1)/2.0));

			HexGrid2DArray[x][y] = newTile;
		}
	}


}

// Called every frame
//void ATile_Manager::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

