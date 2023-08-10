
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

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

	UPROPERTY(EditAnywhere, Category = "Map Settings")
		TSubclassOf<ATile> TileMesh;
		
	
protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable)
	void CreateTile(int x, int y, int i);


private:
	TArray<ATile*> TilesArray;

};





