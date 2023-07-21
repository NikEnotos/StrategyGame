// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UStaticMeshComponent;

class UMaterialInstanceDynamic;

UCLASS()
class STRATEGY_GAME_API ATile : public AActor
{
	GENERATED_BODY()
	
public:


	// Sets default values for this actor's properties
	ATile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FIntPoint GetTileIndex();

	void SetTileIndex(FIntPoint index);

	void SetTileColour(float colour);

	void setTileNoiseValue(float value);

protected:

	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
		UStaticMeshComponent* TileMesh;

	UPROPERTY(VisibleInstanceOnly, Category = "Tile")
		FIntPoint TileIndex;

	UPROPERTY(VisibleAnywhere, Category = "Tile")
		float NoiseValue;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
