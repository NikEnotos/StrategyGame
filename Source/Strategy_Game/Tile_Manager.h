// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Tile.h"

#include "Tile_Manager.generated.h"

class Tile;

UCLASS()
class STRATEGY_GAME_API ATile_Manager : public AActor
{
	GENERATED_BODY()

protected:

	TArray<TArray<ATile*>> HexGrid2DArray;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 MapWidth = 40;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 MapHeight = 40;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 Friquency = 10;

	UPROPERTY(EditAnywhere, Category = "Map Settings | Tile Position")
		int32 heightOfEachLevel = 35;

	UPROPERTY(EditAnywhere, Category = "Map Settings | Tile Position")
		int32 numOfLandLevels = 6;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 Seed = 12345;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		bool seamlessVertically = false;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		bool seamlessHorizontally = false;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		bool CosInterpolation = false;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		//TArray<TSubclassOf<ATile>> AllTileTypesArray;
		TSubclassOf<ATile> TileType;

	UPROPERTY(EditAnywhere, Category = "Map Settings | Tile position")
		float HorizontalOffset = 173.20508075688772935274463415059; // CHANGE IF TILE SIZE IS CHANGED

	UPROPERTY(EditAnywhere, Category = "Map Settings | Tile position")
		float OddHorizontalOffset = HorizontalOffset/2;	
	
	UPROPERTY(EditAnywhere, Category = "Map Settings | Tile position")
		float VerticalOffset = 150.0; // CHANGE IF TILE SIZE IS CHANGED


	
public:	
	// Sets default values for this actor's properties
	ATile_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;


};
