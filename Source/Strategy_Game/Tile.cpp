// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	//TileMesh->SetupAttachment(RootComponent);
	RootComponent = TileMesh;

	//DynamicMaterial->SetScalarParameterValue(TEXT("TileColour"), tileColour);

}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();


	//auto tile = FindComponentByClass<UStaticMeshComponent>();
	auto Material = TileMesh->GetMaterial(0);

	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	TileMesh->SetMaterial(0, DynamicMaterial);
	DynamicMaterial->SetScalarParameterValue(TEXT("Color"), (NoiseValue + 1) / 2);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DynamicMaterial->SetScalarParameterValue(TEXT("Color"), (NoiseValue + 1) / 2);
}

void ATile::SetTileColour(FLinearColor colour)
{
	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), colour);
}

void ATile::setTileNoiseValue(float value)
{
	NoiseValue = value;
}

FIntPoint ATile::GetTileIndex()
{
	return TileIndex;
}

void ATile::SetTileIndex(FIntPoint index)
{
	TileIndex = index;
}