
#include "Tile_Manager.h"
#include "Math/UnrealMathUtility.h"

// Set up connection size
const float FTileMetrics::HexRadius = 100.f;
const float FTileMetrics::HexBorderSize = 10.f;
const float FTileMetrics::ConnectionBridgeSize = 30.f;

const FVector FTileMetrics::corners[7] = {

	FVector(0.f, FTileMetrics::outerRadius, 0.f),
	FVector(FTileMetrics::innerRadius, 0.5 * FTileMetrics::outerRadius, 0.f),
	FVector(FTileMetrics::innerRadius, -0.5 * FTileMetrics::outerRadius, 0.f),
	FVector(0.f, -FTileMetrics::outerRadius, 0.f),
	FVector(-FTileMetrics::innerRadius, -0.5 * FTileMetrics::outerRadius, 0.f),
	FVector(-FTileMetrics::innerRadius, 0.5 * FTileMetrics::outerRadius, 0.f),
	FVector(0.f, FTileMetrics::outerRadius, 0.f)

};
const float FTileMetrics::outerRadius = FTileMetrics::HexRadius + FTileMetrics::HexBorderSize + FTileMetrics::ConnectionBridgeSize;
const float FTileMetrics::innerRadius = outerRadius * FMath::Sqrt(3) / 2;



ATile_Manager::ATile_Manager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATile_Manager::BeginPlay()
{
	Super::BeginPlay();
	
	TilesArray.SetNumZeroed(MapWidth * MapHeight);

	for (int x = 0, i = 0; x < MapHeight; ++x)
	{
		for (int y = 0; y < MapWidth; ++y)
		{
			createTile(x, y, i++);
		}
	}


}

void ATile_Manager::createTile(int x, int y, int i)
{
	FVector position;
	position.X = x * (FTileMetrics::outerRadius * 1.5f);
	position.Y = (y + x * 0.5f - x / 2) * (FTileMetrics::innerRadius * 2.f);
	position.Z = 0.f;


	// Spawn tile at the position
	ATile* newTile = GetWorld()->SpawnActor<ATile>(TileMesh, position, FRotator::ZeroRotator);
	newTile->coordinates = FHexCoordinates::FromOffsetCoordinates(x, y);

	// Associate neighbors
	if (y > 0)
	{
		newTile->SetNeighbor(EHexDirection::W, TilesArray[i - 1]);
	}
	if (x > 0)
	{
		if ((x & 1) == 0) // if even /2 /4...
		{
			newTile->SetNeighbor(EHexDirection::SE, TilesArray[i - MapWidth]);

			if (y > 0)
			{
				newTile->SetNeighbor(EHexDirection::SW, TilesArray[i - MapWidth - 1]);
			}
		}
		else
		{
			newTile->SetNeighbor(EHexDirection::SW, TilesArray[i - MapWidth]);

			if (y < MapWidth - 1)
			{
				newTile->SetNeighbor(EHexDirection::SE, TilesArray[i - MapWidth + 1]);
			}
		}

	}


	TilesArray[i] = newTile;
}


