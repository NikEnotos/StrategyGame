// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "Tile.h" // TODO: figur out how to not connect all Tile.h file but only "EHexDirection"

#include "MapGenerationSubsystem.generated.h"


UCLASS()
class STRATEGY_GAME_API UMapGenerationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//TEST
	UFUNCTION(BlueprintCallable)
		void CreateMap();
	UFUNCTION(BlueprintCallable)
		void ChangeMapParameters(int32 width = 5, int32 height = 5, float distortionIntensity = 15.f, float destortionNoiseScale = 0.021, int numOfSegments = 2, float percentOfEdgeSlope = 7.f, float levelStepHeight = 20.f);
	UFUNCTION(BlueprintCallable)
		void ChangeTileParameters(int tileLevel);

protected:
	// Map SIZE
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 MapWidth = 10;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 MapHeight = 10;

	// Segment distortion
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		float IntensityOfDistortion = 15.f;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		float ScaleOfDestortionNoise = 0.021;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1", ClampMax = "35"))
		int NumberOfSegmentsOfTileSide = 2;

	// Edge smoothness
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "0.0", ClampMax = "100.0"))
		float EdgeSlope = 7.f;

	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1.0", ClampMax = "200.0"))
		float StepHeight = 20.f;



	//DEBUG
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		bool bCreateBorders = true;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		bool bCreateConnections = true;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		bool bCreateTriangleConnections = true;

	// TEST
	//UPROPERTY(EditAnywhere, Category = "Map Settings")
	//	//TSubclassOf<ATile> TileMesh;
	//	ATile* TileMesh;

protected:
	UFUNCTION(BlueprintCallable)
		void CreateTile(int x, int y, int i);


private:
	TArray<ATile*> TilesArray;

private:
	void CreateBorders(ATile* tile);
	void CreateConnections(ATile* tile);
	void CreateTriangleConnections(ATile* tile);

	void AddTriangle(FVector v1, FVector v2, FVector v3, FVector2D UVforV1, FVector2D UVforV2, FVector2D UVforV3, TArray<FVector>& Vertices, TArray<int>& Triangles, TArray<FVector2D>& UV);
	void AddQuad(FVector v1, FVector v2, FVector v3, FVector v4, FVector2D UVforV1, FVector2D UVforV2, FVector2D UVforV3, FVector2D UVforV4, TArray<FVector>& Vertices, TArray<int>& Triangles, TArray<FVector2D>& UV);

	FVector GetDistortionForTileAtPosition(const ATile* tile, FVector position);

	FVector AddSmoothnessForEdge(ATile* tile, EHexDirection direction);
	
};
