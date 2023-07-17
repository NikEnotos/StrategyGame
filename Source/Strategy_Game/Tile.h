// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UStaticMeshComponent;

UCLASS()
class STRATEGY_GAME_API ATile : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleInstanceOnly, Category = "Tile")
	FIntPoint TileIndex;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
		UStaticMeshComponent* TileMesh;

public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
