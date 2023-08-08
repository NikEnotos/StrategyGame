

#include "Tile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

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

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set number of elements of array
	neighbors.SetNumZeroed(6);

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	//TileMesh->SetupAttachment(RootComponent);

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;

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