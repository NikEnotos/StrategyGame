
#include "Tile_Manager.h"
#include "Math/UnrealMathUtility.h"
#include "Tile.h"
#include "ProceduralMeshComponent.h"

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
			CreateTile(x, y, i);
			CreateBorders(TilesArray[i]);
			++i;
		}
	}

}

void ATile_Manager::CreateTile(int x, int y, int i)
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

void ATile_Manager::CreateBorders(ATile* tile)
{
	for (int i = 0; i < 6; ++i)
	{
		TArray<FVector> Vertices;
		TArray<int> Triangles;
		TArray<FVector2D> UV0;

		FVector v1 = FTileMetrics::GetFirsBorderCorner(static_cast<EHexDirection>(i));
		FVector v2 = FTileMetrics::GetFirsSolidCorner(static_cast<EHexDirection>(i));
		FVector v3 = FTileMetrics::GetSecondSolidCorner(static_cast<EHexDirection>(i));
		FVector v4 = FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i));

		//Distortion of Border corners
		v1 += GetDistortionForTileAtPosition(tile, v1);	
		v4 += GetDistortionForTileAtPosition(tile, v4);

		//FVector2D UVforV1(FVector::);
		FVector2D UVforV1(0.2, -.2);
		FVector2D UVforV2(0, 0);
		FVector2D UVforV3(0, 1);
		FVector2D UVforV4(0.2, 1.2);

		AddQuad(v1, v2, v3, v4, UVforV1, UVforV2, UVforV3, UVforV4, Vertices, Triangles, UV0);

		tile->GetBorder(static_cast<EHexDirection>(i))->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	}
}

void ATile_Manager::AddTriangle(FVector v1, FVector v2, FVector v3, FVector2D UVforV1, FVector2D UVforV2, FVector2D UVforV3, TArray<FVector>& Vertices, TArray<int>& Triangles, TArray<FVector2D>& UV)
{
	int count = Vertices.Num();

	Vertices.Add(v1);
	Vertices.Add(v2);
	Vertices.Add(v3);

	Triangles.Add(count);
	Triangles.Add(count + 1);
	Triangles.Add(count + 2);

	UV.Add(UVforV1);
	UV.Add(UVforV2);
	UV.Add(UVforV3);
}
void ATile_Manager::AddQuad(FVector v1, FVector v2, FVector v3, FVector v4, FVector2D UVforV1, FVector2D UVforV2, FVector2D UVforV3, FVector2D UVforV4, TArray<FVector>& Vertices, TArray<int>& Triangles, TArray<FVector2D>& UV)
{
	AddTriangle(v1, v2, v3, UVforV1, UVforV2, UVforV3, Vertices, Triangles, UV);
	AddTriangle(v3, v4, v1, UVforV3, UVforV4, UVforV1, Vertices, Triangles, UV);
}

FVector ATile_Manager::GetDistortionForTileAtPosition(ATile* tile, FVector position)
{
	float xDistortion = FTileMetrics::IntensityOfDistortion * FMath::PerlinNoise3D((FVector(tile->GetActorLocation().X, tile->GetActorLocation().Y, tile->GetActorLocation().Z + 0.5) + position) * FTileMetrics::ScaleOfDestortionNoise);
	float yDistortion = FTileMetrics::IntensityOfDistortion * FMath::PerlinNoise3D((FVector(tile->GetActorLocation().X, tile->GetActorLocation().Y, tile->GetActorLocation().Z + 100.5) + position) * FTileMetrics::ScaleOfDestortionNoise);
	return FVector(xDistortion, yDistortion, 0);
}
