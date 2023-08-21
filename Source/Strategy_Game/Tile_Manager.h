
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Tile.h" // TODO: figur out how to not connect all Tile.h file but only "EHexDirection"

#include "Tile_Manager.generated.h"



class ATile;


UCLASS()
class STRATEGY_GAME_API ATile_Manager : public AActor
{
	GENERATED_BODY()

public:
	ATile_Manager();

protected:
	// Map SIZE
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 MapWidth = 40;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		int32 MapHeight = 40;

	// Segment distortion
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		float IntensityOfDistortion = 15.f;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		float ScaleOfDestortionNoise = 0.021;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1", ClampMax = "35"))
		int NumberOfSegmentsOfTileSide = 2;

	// Edge smoothness
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "0.0", ClampMax = "100.0"))
		float EdgeSlope = 0.f;

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
	UPROPERTY(EditAnywhere, Category = "Map Settings")
		TSubclassOf<ATile> TileMesh;
	
protected:
	virtual void BeginPlay() override;

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





