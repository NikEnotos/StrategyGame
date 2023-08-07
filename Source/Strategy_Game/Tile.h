
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

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




UCLASS()
class STRATEGY_GAME_API ATile : public AActor
{
	GENERATED_BODY()
	
public:
	ATile();

	virtual void Tick(float DeltaTime) override;

	ATile* GetNeighbor(EHexDirection direction);
	void SetNeighbor(EHexDirection direction, ATile* tile);

public:
	UPROPERTY(VisibleAnywhere)
	FHexCoordinates coordinates;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
		UStaticMeshComponent* TileMesh;

	UMaterialInstanceDynamic* DynamicMaterial;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<ATile*> neighbors;

};
