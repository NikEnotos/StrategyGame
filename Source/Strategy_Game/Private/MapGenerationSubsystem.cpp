// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerationSubsystem.h"
#include "Math/UnrealMathUtility.h"
#include "Tile.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

void UMapGenerationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// DEBUG : To keep the same height for each tile        ??? IT DOES NOT WORK IF YOU INITIALIZE IT HERE ???
	//TilesLevelsArray.SetNum(MapWidth * MapHeight);
	//for (auto tile : TilesLevelsArray)
	//{
	//	tile = FMath::Rand() % 5;
	//	
	//	UE_LOG(LogTemp, Warning, TEXT("%f"), tile);
	//}
}
void UMapGenerationSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UMapGenerationSubsystem::CreateMap()
{
	TilesArray.SetNumZeroed(MapWidth * MapHeight);

	for (int x = 0, i = 0; x < MapHeight; ++x)
	{
		for (int y = 0; y < MapWidth; ++y)
		{
			CreateTile(x, y, i++);
		}
	}

	for (auto tile : TilesArray)
	{
		//DEBUG
		if (bCreateBorders) CreateBorders(tile);
		if (bCreateConnections) CreateConnections(tile);
		if (bCreateTriangleConnections) CreateTriangleConnections(tile);
	}
}

void UMapGenerationSubsystem::CreateTile(int x, int y, int i)
{
	FVector position;
	position.X = x * (FTileMetrics::outerRadius * 1.5f);
	position.Y = (y + x * 0.5f - x / 2) * (FTileMetrics::innerRadius * 2.f);
	position.Z = StepHeight * TilesLevelsArray[y + x * MapWidth]; // TODO: Change level height calculation

	// Spawn tile at the position
	ATile* newTile = GetWorld()->SpawnActor<ATile>(ATile::StaticClass(), position, FRotator::ZeroRotator);
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

void UMapGenerationSubsystem::CreateBorders(ATile* tile)
{

	for (int i = 0; i < 6; ++i)
	{
		TArray<FVector> Vertices;
		TArray<int> Triangles;
		TArray<FVector2D> UV0;
		TArray<FVector> Normals;
		TArray<struct FProcMeshTangent> Tangents;

		FVector borderStart = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector solidStart = FTileMetrics::GetFirstSolidCorner(static_cast<EHexDirection>(i));
		FVector solidEnd = FTileMetrics::GetSecondSolidCorner(static_cast<EHexDirection>(i));
		FVector BborderEnd = FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i));

		// TODO: recalculate for each segment
		FVector2D UVforV1(0.2, -.2);
		FVector2D UVforV2(0, 0);
		FVector2D UVforV3(0, 1);
		FVector2D UVforV4(0.2, 1.2);


		//Subdividing border into smaller squears
		for (int iteration = 1; iteration <= NumberOfSegmentsOfTileSide; ++iteration)
		{

			FVector borderSegmentStart = FMath::Lerp(borderStart, BborderEnd, float(iteration - 1) / NumberOfSegmentsOfTileSide);
			FVector solidSegmentStart = FMath::Lerp(solidStart, solidEnd, float(iteration - 1) / NumberOfSegmentsOfTileSide);

			FVector borderSegmentEnd = FMath::Lerp(borderStart, BborderEnd, float(iteration) / NumberOfSegmentsOfTileSide);
			FVector solidSegmentEnd = FMath::Lerp(solidStart, solidEnd, float(iteration) / NumberOfSegmentsOfTileSide);


			// Add edge's slope
			FVector edgeSlope = AddSmoothnessForEdge(tile, static_cast<EHexDirection>(i));
			if (iteration != 1)
			{
				borderSegmentStart += edgeSlope;
			}
			else
			{
				FVector PreviousEdgeSlope = AddSmoothnessForEdge(tile, FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i)));
				borderSegmentStart += (edgeSlope + PreviousEdgeSlope) / 2;
			}

			if (iteration != NumberOfSegmentsOfTileSide)
			{
				borderSegmentEnd += edgeSlope;
			}
			else
			{
				FVector NextEdgeSlope = AddSmoothnessForEdge(tile, FHexDirectionExtensions::Next(static_cast<EHexDirection>(i)));
				borderSegmentEnd += (edgeSlope + NextEdgeSlope) / 2;
			}


			// Add distortion of Border corners
			borderSegmentStart += GetDistortionForTileAtPosition(tile, borderSegmentStart);
			borderSegmentEnd += GetDistortionForTileAtPosition(tile, borderSegmentEnd);

			AddQuad(borderSegmentStart, solidSegmentStart, solidSegmentEnd, borderSegmentEnd, UVforV1, UVforV2, UVforV3, UVforV4, Vertices, Triangles, UV0);

		}

		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

		tile->GetBordersComponent()->CreateMeshSection(i, Vertices, Triangles, Normals, UV0, TArray<FColor>(), Tangents, true);

	}

	//TEST
	//tile->GetBorder(static_cast<EHexDirection>(0))->SetMaterial(index, material);

}
void UMapGenerationSubsystem::CreateConnections(ATile* tile)
{

	for (int i = 0; i < 3; ++i)
	{
		if (tile->GetNeighbor(static_cast<EHexDirection>(i)) == nullptr) continue;

		TArray<FVector> Vertices;
		TArray<int> Triangles;
		TArray<FVector2D> UV0;
		TArray<FVector> Normals;
		TArray<struct FProcMeshTangent> Tangents;

		FVector neighboursFirstCornerLocation = tile->GetNeighbor(static_cast<EHexDirection>(i))->GetActorLocation() + FTileMetrics::GetSecondBorderCorner(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)));
		FVector tilesFirstCornerLocation = tile->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v1 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i)) + neighboursFirstCornerLocation - tilesFirstCornerLocation;

		FVector v2 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v3 = FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i));

		FVector neighboursSecondCornerLocation = tile->GetNeighbor(static_cast<EHexDirection>(i))->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)));
		FVector tilesSecondCornerLocation = tile->GetActorLocation() + FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i));
		FVector v4 = FTileMetrics::GetSecondBorderCorner(static_cast<EHexDirection>(i)) + neighboursSecondCornerLocation - tilesSecondCornerLocation;

		// TODO: recalculate for each segment
		FVector2D UVforV1(0.25, 0);
		FVector2D UVforV2(0, 0);
		FVector2D UVforV3(0, 1);
		FVector2D UVforV4(0.25, 1);

		//Subdividing connection into smaller squears
		for (int iteration = 1; iteration <= NumberOfSegmentsOfTileSide; ++iteration)
		{

			FVector borderSegmentStart = FMath::Lerp(v1, v4, float(iteration - 1) / NumberOfSegmentsOfTileSide);
			FVector solidSegmentStart = FMath::Lerp(v2, v3, float(iteration - 1) / NumberOfSegmentsOfTileSide);

			FVector borderSegmentEnd = FMath::Lerp(v1, v4, float(iteration) / NumberOfSegmentsOfTileSide);
			FVector solidSegmentEnd = FMath::Lerp(v2, v3, float(iteration) / NumberOfSegmentsOfTileSide);

			// Add edge's slope
			FVector tileEdgeSlope = AddSmoothnessForEdge(tile, static_cast<EHexDirection>(i));
			FVector neighborEdgeSlope = AddSmoothnessForEdge(tile->GetNeighbor(static_cast<EHexDirection>(i)), FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)));
			if (iteration != 1)
			{
				solidSegmentStart += tileEdgeSlope;
				borderSegmentStart += neighborEdgeSlope;
			}
			else
			{
				FVector PreviousTileEdgeSlope = AddSmoothnessForEdge(tile, FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i)));
				FVector PreviousNeighborEdgeSlope = AddSmoothnessForEdge(tile->GetNeighbor(static_cast<EHexDirection>(i)), FHexDirectionExtensions::Opposite(FHexDirectionExtensions::Next(static_cast<EHexDirection>(i))));

				solidSegmentStart += (tileEdgeSlope + PreviousTileEdgeSlope) / 2;
				borderSegmentStart += (neighborEdgeSlope + PreviousNeighborEdgeSlope) / 2;
			}

			if (iteration != NumberOfSegmentsOfTileSide)
			{
				solidSegmentEnd += tileEdgeSlope;
				borderSegmentEnd += neighborEdgeSlope;
			}
			else
			{
				FVector NextTileEdgeSlope = AddSmoothnessForEdge(tile, FHexDirectionExtensions::Next(static_cast<EHexDirection>(i)));
				FVector NextNeighborEdgeSlope = AddSmoothnessForEdge(tile->GetNeighbor(static_cast<EHexDirection>(i)), FHexDirectionExtensions::Opposite(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i))));

				solidSegmentEnd += (tileEdgeSlope + NextTileEdgeSlope) / 2;
				borderSegmentEnd += (neighborEdgeSlope + NextNeighborEdgeSlope) / 2;
			}


			// Add distortion of Border corners
			borderSegmentStart += GetDistortionForTileAtPosition(tile, borderSegmentStart);
			borderSegmentEnd += GetDistortionForTileAtPosition(tile, borderSegmentEnd);
			solidSegmentStart += GetDistortionForTileAtPosition(tile, solidSegmentStart);
			solidSegmentEnd += GetDistortionForTileAtPosition(tile, solidSegmentEnd);

			AddQuad(borderSegmentStart, solidSegmentStart, solidSegmentEnd, borderSegmentEnd, UVforV1, UVforV2, UVforV3, UVforV4, Vertices, Triangles, UV0);
		}

		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

		tile->GetConnectionsComponent()->CreateMeshSection(i, Vertices, Triangles, Normals, UV0, TArray<FColor>(), Tangents, true);

	}
}
void UMapGenerationSubsystem::CreateTriangleConnections(ATile* tile)
{

	for (int i = 0; i < 2; ++i)
	{
		if (tile->GetNeighbor(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i))) == nullptr || tile->GetNeighbor(static_cast<EHexDirection>(i)) == nullptr) continue;

		TArray<FVector> Vertices;
		TArray<int> Triangles;
		TArray<FVector2D> UV0;
		TArray<FVector> Normals;
		TArray<struct FProcMeshTangent> Tangents;

		FVector neighboursFirstCornerLocation = tile->GetNeighbor(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i)))->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(FHexDirectionExtensions::Opposite(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i))));
		FVector tilesFirstCornerLocation = tile->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v1 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i)) + neighboursFirstCornerLocation - tilesFirstCornerLocation;

		FVector v2 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));

		FVector neighboursSecondCornerLocation = tile->GetNeighbor(static_cast<EHexDirection>(i))->GetActorLocation() + FTileMetrics::GetSecondBorderCorner(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)));
		FVector tilesSecondCornerLocation = tile->GetActorLocation() + FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i));
		FVector v3 = FTileMetrics::GetFirstBorderCorner(static_cast<EHexDirection>(i)) + neighboursSecondCornerLocation - tilesSecondCornerLocation;


		// Add edge's slope
		v1 += (AddSmoothnessForEdge(tile->GetNeighbor(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i))), FHexDirectionExtensions::Opposite(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i)))) + AddSmoothnessForEdge(tile->GetNeighbor(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i))), FHexDirectionExtensions::Previous(FHexDirectionExtensions::Opposite(FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i)))))) / 2;
		v2 += (AddSmoothnessForEdge(tile, FHexDirectionExtensions::Previous(static_cast<EHexDirection>(i))) + AddSmoothnessForEdge(tile, static_cast<EHexDirection>(i))) / 2;
		v3 += (AddSmoothnessForEdge(tile->GetNeighbor(static_cast<EHexDirection>(i)), FHexDirectionExtensions::Next(FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)))) + AddSmoothnessForEdge(tile->GetNeighbor(static_cast<EHexDirection>(i)), FHexDirectionExtensions::Opposite(static_cast<EHexDirection>(i)))) / 2;

		//Distortion of Border corners
		v1 += GetDistortionForTileAtPosition(tile, v1);
		v2 += GetDistortionForTileAtPosition(tile, v2);
		v3 += GetDistortionForTileAtPosition(tile, v3);

		// TODO: recalculate for each segment
		FVector2D UVforV1(0.25, 0);
		FVector2D UVforV2(0, 0);
		FVector2D UVforV3(0, 1);

		AddTriangle(v1, v2, v3, UVforV1, UVforV2, UVforV3, Vertices, Triangles, UV0);

		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

		tile->GetTriangleConnectionsComponent()->CreateMeshSection(i, Vertices, Triangles, Normals, UV0, TArray<FColor>(), Tangents, true);

	}
}


void UMapGenerationSubsystem::AddTriangle(FVector v1, FVector v2, FVector v3, FVector2D UVforV1, FVector2D UVforV2, FVector2D UVforV3, TArray<FVector>& Vertices, TArray<int>& Triangles, TArray<FVector2D>& UV)
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
void UMapGenerationSubsystem::AddQuad(FVector v1, FVector v2, FVector v3, FVector v4, FVector2D UVforV1, FVector2D UVforV2, FVector2D UVforV3, FVector2D UVforV4, TArray<FVector>& Vertices, TArray<int>& Triangles, TArray<FVector2D>& UV)
{
	AddTriangle(v1, v2, v3, UVforV1, UVforV2, UVforV3, Vertices, Triangles, UV);
	AddTriangle(v3, v4, v1, UVforV3, UVforV4, UVforV1, Vertices, Triangles, UV);
}

FVector UMapGenerationSubsystem::GetDistortionForTileAtPosition(const ATile* tile, FVector position)
{
	float xDistortion = IntensityOfDistortion * FMath::PerlinNoise3D((FVector(tile->GetActorLocation().X, tile->GetActorLocation().Y, tile->GetActorLocation().Z + 0.5) + position) * ScaleOfDestortionNoise);
	float yDistortion = IntensityOfDistortion * FMath::PerlinNoise3D((FVector(tile->GetActorLocation().X, tile->GetActorLocation().Y, tile->GetActorLocation().Z + 100.5) + position) * ScaleOfDestortionNoise);
	return FVector(xDistortion, yDistortion, 0);
}

FVector UMapGenerationSubsystem::AddSmoothnessForEdge(ATile* tile, EHexDirection direction)
{
	ATile* neighbor = tile->GetNeighbor(direction);
	if (neighbor != nullptr && neighbor->GetActorLocation().Z != tile->GetActorLocation().Z)
	{
		return neighbor->GetActorLocation().Z < tile->GetActorLocation().Z ? FVector(0, 0, -EdgeSlope) : FVector(0, 0, EdgeSlope);
	}
	else
	{
		return FVector(0, 0, 0);
	}
}

void UMapGenerationSubsystem::ChangeMapParameters(int32 width, int32 height, float distortionIntensity, float destortionNoiseScale, int numOfSegments, float percentOfEdgeSlope, float levelStepHeight)
{
	MapWidth = width;
	MapHeight = height;
	IntensityOfDistortion = distortionIntensity;
	ScaleOfDestortionNoise = destortionNoiseScale;
	NumberOfSegmentsOfTileSide = numOfSegments;
	EdgeSlope = percentOfEdgeSlope;
	StepHeight = levelStepHeight;

	for (auto tile : TilesArray)
	{
		tile->Destroy();
	}
	TilesArray.Empty(0);

	// Recalculate TilesLevelsArray
	if (TilesLevelsArray.Num() < MapWidth * MapHeight)
	{
		for (int i = TilesLevelsArray.Num(); i < MapWidth * MapHeight; ++i)
		{
			TilesLevelsArray.Add(FMath::Rand() % 5);
		}
	}
	else
	{
		TilesLevelsArray.SetNum(MapWidth * MapHeight);
	}

	CreateMap();
}

void UMapGenerationSubsystem::ChangeTileParameters(int tileLevel)
{

}