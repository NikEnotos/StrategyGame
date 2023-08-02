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
	Noise2DGenerator Noise(MapWidth, MapHeight, Friquency, Seed, seamlessVertically, seamlessHorizontally, CosInterpolation);

	for (int32 y = 0; y < MapHeight; y++)
	{
		for (int32 x = 0; x < MapWidth; x++)
		{
			// Set position of each tile
			const float xPos = y % 2 == 1 ? (x * HorizontalOffset) + OddHorizontalOffset : x * HorizontalOffset; // if it`s a odd row we add extra horizontal offset
			const float yPos = y * VerticalOffset;

			// Calculate level of land
			int levelOfLand = FMath::GetMappedRangeValueClamped( FVector2D(-1, 1), FVector2D(0, 255), Noise.getValueAtPoint(x, y)) / (256/ numOfLandLevels);


			// Spawn tile at the position
			ATile* newTile = GetWorld()->SpawnActor<ATile>(TileType, FVector(xPos, yPos, heightOfEachLevel * levelOfLand), FRotator::ZeroRotator);
			newTile->SetTileIndex(FIntPoint(x, y));
			newTile->setTileNoiseValue(Noise.getValueAtPoint(x, y));


			if (Noise.getValueAtPoint(x, y) <= -0.6) // Water
			{
				newTile->SetTileColour(FLinearColor::FromSRGBColor(FColor(100,150,232)));
			}
			else if (Noise.getValueAtPoint(x, y) <= -0.32) // Sand
			{
				newTile->SetTileColour(FLinearColor::FromSRGBColor(FColor(252,248,0)));
			}
			else if (Noise.getValueAtPoint(x, y) <= -0.04) // L1
			{
				newTile->SetTileColour(FLinearColor::FromSRGBColor(FColor(22,201,70)));
			}
			else if (Noise.getValueAtPoint(x, y) <= 0.24) // L2
			{
				newTile->SetTileColour(FLinearColor::FromSRGBColor(FColor(17,166,57)));
			}
			else if (Noise.getValueAtPoint(x, y) <= 0.52) // L3
			{
				newTile->SetTileColour(FLinearColor::FromSRGBColor(FColor(15,145,50)));
			}
			else if (Noise.getValueAtPoint(x, y) <= 0.8) // L4
			{
				newTile->SetTileColour(FLinearColor::FromSRGBColor(FColor(9,110,36)));
			}
			else if (Noise.getValueAtPoint(x, y) <= 1) // Mountains
			{
				newTile->SetTileColour(FLinearColor::FromSRGBColor(FColor(174,191,179)));
			}

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

