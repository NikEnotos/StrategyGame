

#include "Tile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"

FHexCoordinates::FHexCoordinates(int x, int y) { X = x; Y = y; Z = -x - y; };
FHexCoordinates::FHexCoordinates() { X = 0; Y = 0; };
FHexCoordinates FHexCoordinates::FromOffsetCoordinates(int x, int y) { return FHexCoordinates(x, y - int(x / 2)); };

EHexDirection FHexDirectionExtensions::Opposite(EHexDirection direction)
{
	return (int)direction < 3 ? static_cast<EHexDirection>((int)direction + 3) : static_cast<EHexDirection>((int)direction - 3);
}
EHexDirection FHexDirectionExtensions::Previous(EHexDirection direction)
{
	return direction == EHexDirection::NE ? EHexDirection::NW : static_cast<EHexDirection>((int)direction - 1);
}
EHexDirection FHexDirectionExtensions::Next(EHexDirection direction)
{
	return direction == EHexDirection::NW ? EHexDirection::NE : static_cast<EHexDirection>((int)direction + 1);
}
FString FHexDirectionExtensions::DirectionToString(EHexDirection direction)
{
	switch (direction)
	{
		case EHexDirection::NE:
			return TEXT("NE");
			break;

		case EHexDirection::E:
			return TEXT("E");
			break;

		case EHexDirection::SE:
			return TEXT("SE");
			break;

		case EHexDirection::SW:
			return TEXT("SW");
			break;

		case EHexDirection::W:
			return TEXT("W");
			break;

		case EHexDirection::NW:
			return TEXT("NW");
			break;

		default:
			return TEXT("NONE");
	}
}

// Set up connection size
const float FTileMetrics::HexRadius = 100.f;
const float FTileMetrics::HexBorderSize = 30.f;
const float FTileMetrics::ConnectionBridgeSize = 30.f;
const FVector FTileMetrics::corners[7] = {
	FVector(FTileMetrics::outerRadius, 0.f, 0.f),
	FVector(0.5 * FTileMetrics::outerRadius, FTileMetrics::innerRadius, 0.f),
	FVector(-0.5 * FTileMetrics::outerRadius, FTileMetrics::innerRadius, 0.f),
	FVector(-FTileMetrics::outerRadius, 0.f, 0.f),
	FVector(-0.5 * FTileMetrics::outerRadius, -FTileMetrics::innerRadius, 0.f),
	FVector(0.5 * FTileMetrics::outerRadius, -FTileMetrics::innerRadius, 0.f),
	FVector(FTileMetrics::outerRadius, 0.f, 0.f)

};
const float FTileMetrics::outerRadius = FTileMetrics::HexRadius + FTileMetrics::HexBorderSize + FTileMetrics::ConnectionBridgeSize/2.f;
const float FTileMetrics::innerRadius = outerRadius * FMath::Sqrt(3) / 2;

FVector FTileMetrics::GetFirstCorner(EHexDirection direction)
{
	return corners[(int)direction];
}
FVector FTileMetrics::GetSecondCorner(EHexDirection direction)
{
	return corners[(int)direction + 1];
}

FVector FTileMetrics::GetFirsSolidCorner(EHexDirection direction)
{
	return GetFirstCorner(direction) * (HexRadius / outerRadius);
}
FVector FTileMetrics::GetSecondSolidCorner(EHexDirection direction)
{
	return GetSecondCorner(direction) * (HexRadius / outerRadius);
}

FVector FTileMetrics::GetFirsBorderCorner(EHexDirection direction)
{
	return GetFirstCorner(direction) * ((HexRadius + HexBorderSize) / outerRadius);
}
FVector FTileMetrics::GetSecondBorderCorner(EHexDirection direction)
{
	return GetSecondCorner(direction) * ((HexRadius + HexBorderSize) / outerRadius);
}


ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set number of elements of array
	neighbors.SetNumZeroed(6);

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;

	DefineBordersProceduralMeshes();

}

void ATile::BeginPlay()
{
	Super::BeginPlay();

	auto Material = TileMesh->GetMaterial(0);
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	TileMesh->SetMaterial(0, DynamicMaterial);
	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FColor::White);
}

void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ATile* ATile::GetNeighbor(EHexDirection direction)
{
	return neighbors[int(direction)];
}
void ATile::SetNeighbor(EHexDirection direction, ATile* tile)
{
	neighbors[int(direction)] = tile;
	tile->neighbors[int(FHexDirectionExtensions::Opposite(direction))] = this;
}

UProceduralMeshComponent* ATile::GetBorder(EHexDirection direction)
{
	return Borders[(int)direction];
}

void ATile::DefineBordersProceduralMeshes()
{
	for (int i = 0; i < 6; ++i)
	{
		UProceduralMeshComponent* border = CreateDefaultSubobject<UProceduralMeshComponent>((FName)FHexDirectionExtensions::DirectionToString(static_cast<EHexDirection>(i)));
		border->SetupAttachment(GetRootComponent()); // If you attach border to hex, its start coordinates is centre of the hex 
		Borders.Add(border);
	}
}


