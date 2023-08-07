
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

	ATile();

	virtual void Tick(float DeltaTime) override;


protected:

	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
		UStaticMeshComponent* TileMesh;

	virtual void BeginPlay() override;


};
