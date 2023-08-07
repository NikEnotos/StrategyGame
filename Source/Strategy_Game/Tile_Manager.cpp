
#include "Tile_Manager.h"
#include "Math/UnrealMathUtility.h"

struct TileMetrics{

	static const float outerRadius;

	static const float innerRadius;

	static const FVector corners[6];
};

const FVector TileMetrics::corners[6] = {

	FVector(0.f, TileMetrics::outerRadius, 0.f),
	FVector(TileMetrics::innerRadius, 0.5 * TileMetrics::outerRadius, 0.f),
	FVector(TileMetrics::innerRadius, -0.5 * TileMetrics::outerRadius, 0.f),
	FVector(0.f, -TileMetrics::outerRadius, 0.f),
	FVector(-TileMetrics::innerRadius, -0.5 * TileMetrics::outerRadius, 0.f),
	FVector(-TileMetrics::innerRadius, 0.5 * TileMetrics::outerRadius, 0.f)

};
const float TileMetrics::outerRadius = 150.f;
const float TileMetrics::innerRadius = outerRadius * FMath::Sqrt(3) / 2;



ATile_Manager::ATile_Manager()
{
	PrimaryActorTick.bCanEverTick = false;



}

void ATile_Manager::BeginPlay()
{
	Super::BeginPlay();
	
	TilesArray.SetNumZeroed(MapWidth * MapHeibht);

	for (int y = 0, i = 0; y < MapHeibht; ++y)
	{
		for (int x = 0; x < MapWidth; ++x)
		{
			createTile(x, y, i++);
		}
	}


}

void ATile_Manager::createTile(int x, int y, int i)
{
	FVector position;
	position.X = (x + y * 0.5f - y / 2) * (TileMetrics::innerRadius * 2.f);
	position.Y = y * (TileMetrics::outerRadius * 1.5f);
	position.Z = 0.f;


	// Spawn tile at the position
	ATile* newTile = GetWorld()->SpawnActor<ATile>(TileMesh, position, FRotator::ZeroRotator);

	TilesArray[i] = newTile;
}


