
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Tile.h"

#include "Tile_Manager.generated.h"

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
};


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

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		TSubclassOf<ATile> TileMesh;
		
	
protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable)
	void createTile(int x, int y, int i);


private:
	TArray<ATile*> TilesArray;
};
