

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

	DefineBordersProceduralMeshes();
	DefineConnectionsProceduralMeshes();
	DefineTriangleConnectionsProceduralMeshes();
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
	//return Borders[(int)direction];
	//DEBUG
	return (UProceduralMeshComponent*)(RootComponent->GetChildComponent((int)direction));
}
UProceduralMeshComponent* ATile::GetConnection(EHexDirection direction)
{
	if (direction <= EHexDirection::SE)
	{
		return (UProceduralMeshComponent*)(RootComponent->GetChildComponent(6 + (int)direction)); // 6 is number of borders
	}
	else
	{
		if (GetNeighbor(direction) == nullptr) return nullptr; // if there is no neighbor in the direction, return nullprt
		return (UProceduralMeshComponent*)(GetNeighbor(direction)->GetConnection(FHexDirectionExtensions::Opposite(direction)));
	}
}
UProceduralMeshComponent* ATile::GetTriangleConnection(EHexDirection direction)
{
	if (direction <= EHexDirection::SE)
	{
		return (UProceduralMeshComponent*)(RootComponent->GetChildComponent(6 + 3 + (int)direction)); // 6 is number of borders and 3 is number of connections
	}
	else
	{
		if (GetNeighbor(direction) == nullptr) return nullptr; // if there is no neighbor in the direction, return nullprt
		return (UProceduralMeshComponent*)(GetNeighbor(direction)->GetTriangleConnection(FHexDirectionExtensions::Opposite(direction)));
	}
}

void ATile::DefineBordersProceduralMeshes()
{
	TArray<FName> borderName{ FName(TEXT("Border NE")), FName(TEXT("Border E")), FName(TEXT("Border SE")), FName(TEXT("Border SW")), FName(TEXT("Border W")), FName(TEXT("Border NW")) };

	for (int i = 0; i < 6; ++i)
	{
		UProceduralMeshComponent* border = CreateDefaultSubobject<UProceduralMeshComponent>(borderName[i]);
		border->SetupAttachment(RootComponent); // If you attach border to hex, its start coordinates is centre of the hex 
		//DEBUG
		Borders.Add(border);
	}

}
void ATile::DefineConnectionsProceduralMeshes()
{
	TArray<FName> borderName{ FName(TEXT("Connection NE")), FName(TEXT("Connection E")), FName(TEXT("Connection SE")) };

	for (int i = 0; i < 3; ++i)
	{
		UProceduralMeshComponent* connection = CreateDefaultSubobject<UProceduralMeshComponent>(borderName[i]);
		connection->SetupAttachment(RootComponent); // If you attach border to hex, its start coordinates is centre of the hex 
		//DEBUG
		Connections.Add(connection);
	}
}
void ATile::DefineTriangleConnectionsProceduralMeshes()
{
	TArray<FName> borderName{ FName(TEXT("Triangle connection NE")), FName(TEXT("Triangle connection E")), FName(TEXT("Triangle connection SE")) };

	for (int i = 0; i < 3; ++i)
	{
		UProceduralMeshComponent* triangleConnection = CreateDefaultSubobject<UProceduralMeshComponent>(borderName[i]);
		triangleConnection->SetupAttachment(RootComponent); // If you attach border to hex, its start coordinates is centre of the hex 
		//DEBUG
		TriangleConnections.Add(triangleConnection);
	}
}



