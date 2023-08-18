

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

// Setup values of Metrics
const float FTileMetrics::HexRadius = 100.f;
const float FTileMetrics::HexBorderSize = 30.f;
const float FTileMetrics::ConnectionBridgeSize = 30.f;
const float FTileMetrics::IntensityOfDistortion = 15.f;
const float FTileMetrics::ScaleOfDestortionNoise = 0.03;

const float FTileMetrics::SolidInnerRadius = FMath::Sqrt(FMath::Pow(FTileMetrics::HexRadius, 2) - FMath::Pow(FTileMetrics::HexRadius / 2, 2));
const float FTileMetrics::innerRadius = FTileMetrics::SolidInnerRadius + FTileMetrics::HexBorderSize + FTileMetrics::ConnectionBridgeSize / 2.f;
const float FTileMetrics::outerRadius = 2 * innerRadius / FMath::Sqrt(3);

const FVector FTileMetrics::corners[7] = {
	FVector(FTileMetrics::outerRadius, 0.f, 0.f),
	FVector(0.5 * FTileMetrics::outerRadius, FTileMetrics::innerRadius, 0.f),
	FVector(-0.5 * FTileMetrics::outerRadius, FTileMetrics::innerRadius, 0.f),
	FVector(-FTileMetrics::outerRadius, 0.f, 0.f),
	FVector(-0.5 * FTileMetrics::outerRadius, -FTileMetrics::innerRadius, 0.f),
	FVector(0.5 * FTileMetrics::outerRadius, -FTileMetrics::innerRadius, 0.f),
	FVector(FTileMetrics::outerRadius, 0.f, 0.f)

};

FVector FTileMetrics::GetFirstCorner(EHexDirection direction)
{
	return corners[(int)direction];
}
FVector FTileMetrics::GetSecondCorner(EHexDirection direction)
{
	return corners[(int)direction + 1];
}

FVector FTileMetrics::GetFirstSolidCorner(EHexDirection direction)
{
	return GetFirstCorner(direction) * (HexRadius / outerRadius);
}
FVector FTileMetrics::GetSecondSolidCorner(EHexDirection direction)
{
	return GetSecondCorner(direction) * (HexRadius / outerRadius);
}

FVector FTileMetrics::GetFirstBorderCorner(EHexDirection direction)
{
	return GetFirstCorner(direction) * ((HexRadius + (2.f * HexBorderSize * FMath::Sqrt(3) / 3.f)) / outerRadius);
}
FVector FTileMetrics::GetSecondBorderCorner(EHexDirection direction)
{
	return GetSecondCorner(direction) * ((HexRadius + (2.f * HexBorderSize * FMath::Sqrt(3) / 3.f)) / outerRadius);
}

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set number of elements of array
	neighbors.SetNumZeroed(6);

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;

	DefineBordersProceduralMeshe();
	DefineConnectionsProceduralMeshe();
	DefineTriangleConnectionsProceduralMeshe();
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

UProceduralMeshComponent* ATile::GetBordersComponent()
{
	return (UProceduralMeshComponent*)(RootComponent->GetChildComponent(0));
}
UProceduralMeshComponent* ATile::GetConnectionsComponent()
{
	return (UProceduralMeshComponent*)(RootComponent->GetChildComponent(1));
}
UProceduralMeshComponent* ATile::GetTriangleConnectionsComponent()
{
	return (UProceduralMeshComponent*)(RootComponent->GetChildComponent(2)); 
}

void ATile::DefineBordersProceduralMeshe()
{
	UProceduralMeshComponent* border = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Borders"));
	border->SetupAttachment(RootComponent); // If you attach border to hex, its start coordinates is centre of the hex 
}
void ATile::DefineConnectionsProceduralMeshe()
{
	UProceduralMeshComponent* connection = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("NE_E_SE_Connections"));
	connection->SetupAttachment(RootComponent); // If you attach border to hex, its start coordinates is centre of the hex 
}
void ATile::DefineTriangleConnectionsProceduralMeshe()
{
	UProceduralMeshComponent* triangleConnection = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("NE_E_SE_TriangleConnections"));
	triangleConnection->SetupAttachment(RootComponent); // If you attach border to hex, its start coordinates is centre of the hex 
}



