
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UProceduralMeshComponent;

USTRUCT()
struct FHexCoordinates 
{
	GENERATED_BODY()

	FHexCoordinates(int x, int y);

	FHexCoordinates();

	inline int getX() { return X; };
	inline int getY() { return Y; };
	inline int getZ() { return - X - Y; };

	static FHexCoordinates FromOffsetCoordinates(int x, int y);

private:
	UPROPERTY(VisibleAnywhere)
	int X;
	UPROPERTY(VisibleAnywhere)
	int Y;

	// OPTIONAL; TODO: Delete
	UPROPERTY(VisibleAnywhere)
	int Z;
};

UENUM()
enum class EHexDirection : uint8 { NE, E, SE, SW, W, NW };

USTRUCT()
struct FHexDirectionExtensions
{
	GENERATED_BODY()

	static EHexDirection Opposite(EHexDirection direction);

	static EHexDirection Previous(EHexDirection direction);

	static EHexDirection Next(EHexDirection direction);

	static FString DirectionToString(EHexDirection direction);
};

USTRUCT()
struct FTileMetrics
{
	GENERATED_BODY()

		static const float outerRadius;

	static const float innerRadius;

	static const FVector corners[7];

	static const float HexRadius;

	static const float HexBorderSize;

	static const float ConnectionBridgeSize;

	static FVector GetFirstCorner(EHexDirection direction);
	static FVector GetSecondCorner(EHexDirection direction);

	static FVector GetFirsSolidCorner(EHexDirection direction);
	static FVector GetSecondSolidCorner(EHexDirection direction);

	static FVector GetFirsBorderCorner(EHexDirection direction);
	static FVector GetSecondBorderCorner(EHexDirection direction);
};

UCLASS()
class STRATEGY_GAME_API ATile : public AActor
{
	GENERATED_BODY()
	

public:
	UPROPERTY(VisibleAnywhere)
		FHexCoordinates coordinates;

	////// Borders
	UPROPERTY(VisibleAnywhere)
		TArray<UProceduralMeshComponent*> Borders;

public:
	ATile();

	virtual void Tick(float DeltaTime) override;

	ATile* GetNeighbor(EHexDirection direction);
	void SetNeighbor(EHexDirection direction, ATile* tile);

	void CreateBorders();



protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
		UStaticMeshComponent* TileMesh;

	UMaterialInstanceDynamic* DynamicMaterial;

protected:
	virtual void BeginPlay() override;



private:
	UPROPERTY(VisibleAnywhere)
	TArray<ATile*> neighbors;

private:
	void DefineBordersProceduralMeshes();


};
