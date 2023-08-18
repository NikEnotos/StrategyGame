
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
			//DEBUG
			if (bCreateBorders) CreateBorders(TilesArray[i]);
			++i;
		}
	}

	for (auto tile : TilesArray)
	{
		//DEBUG
		if (bCreateConnections) CreateConnections(tile);
		if (bCreateTriangleConnections) CreateTriangleConnections(tile);
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

	TilesArray[i] = newTile;

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
}

void ATile_Manager::CreateBorders(ATile* tile)
{
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;

	for (int i = 0; i < 6; ++i)
	{


		FVector v1 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v2 = FTileMetrics::GetFirstSolidCorner(static_cast<EHexDirection>(i));
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

		tile->GetBordersComponent()->CreateMeshSection(i, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}

	//TEST
	//tile->GetBorder(static_cast<EHexDirection>(0))->SetMaterial(index, material);

}
void ATile_Manager::CreateConnections(ATile* tile)
{
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;

	for (int i = 0; i < 3; ++i)
	{
		if (tile->GetNeighbor(static_cast<EHexDirection>(i)) == nullptr) continue;
	
		FVector neighboursFirstCornerLocation = tile->GetNeighbor(static_cast<EHexDirection>(i))->GetActorLocation() + FTileMetrics::GetSecondBorderCorner(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)));
		FVector tilesFirstCornerLocation = tile->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v1 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i)) + neighboursFirstCornerLocation - tilesFirstCornerLocation;

		FVector v2 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v3 = FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i));

		//DrawDebugSphere(GetWorld(), v3, 10.f, 4, FColor::Green, true);

		FVector neighboursSecondCornerLocation = tile->GetNeighbor(static_cast<EHexDirection>(i))->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)));
		FVector tilesSecondCornerLocation = tile->GetActorLocation() + FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i));
		FVector v4 = FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i)) + neighboursSecondCornerLocation - tilesSecondCornerLocation;

		//Distortion of Border corners
		v1 += GetDistortionForTileAtPosition(tile, v1);
		v2 += GetDistortionForTileAtPosition(tile, v2);
		v3 += GetDistortionForTileAtPosition(tile, v3);
		v4 += GetDistortionForTileAtPosition(tile, v4);

		//FVector2D UVforV1(FVector::);
		FVector2D UVforV1(0.25, 0);
		FVector2D UVforV2(0, 0);
		FVector2D UVforV3(0, 1);
		FVector2D UVforV4(0.25, 1);

		AddQuad(v1, v2, v3, v4, UVforV1, UVforV2, UVforV3, UVforV4, Vertices, Triangles, UV0);

		tile->GetConnectionsComponent()->CreateMeshSection(i, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}
}
void ATile_Manager::CreateTriangleConnections(ATile* tile)
{
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;

	for (int i = 0; i < 3; ++i)
	{
		if (tile->GetNeighbor(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i))) == nullptr || tile->GetNeighbor(static_cast<EHexDirection>(i)) == nullptr) continue;


		FVector neighboursFirstCornerLocation = tile->GetNeighbor(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i)))->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(FHexDirectionExtensions::Previous(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i))));
		FVector tilesFirstCornerLocation = tile->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v1 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i)) + neighboursFirstCornerLocation - tilesFirstCornerLocation;

		FVector v2 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));

		FVector neighboursSecondCornerLocation = tile->GetNeighbor(static_cast<EHexDirection>(i))->GetActorLocation() + FTileMetrics::GetSecondBorderCorner(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)));
		FVector tilesSecondCornerLocation = tile->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v3 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i)) + neighboursSecondCornerLocation - tilesSecondCornerLocation;

		
		//Distortion of Border corners
		v1 += GetDistortionForTileAtPosition(tile, v1);
		v2 += GetDistortionForTileAtPosition(tile, v2);
		v3 += GetDistortionForTileAtPosition(tile, v3);

		//FVector2D UVforV1(FVector::);
		FVector2D UVforV1(0.25, 0);
		FVector2D UVforV2(0, 0);
		FVector2D UVforV3(0, 1);

		AddTriangle(v1, v2, v3, UVforV1, UVforV2, UVforV3, Vertices, Triangles, UV0);

		tile->GetTriangleConnectionsComponent()->CreateMeshSection(i, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

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
