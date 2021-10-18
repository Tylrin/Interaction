// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

//=============================================================================
/**
 * This is a component that handles finding Interactable actors.
 *
 */

UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class INTERACTION_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Initialize the interaction component.
	UFUNCTION()
	void InitializeInteractionComponent();

	// Update the interaction component.
	UFUNCTION()
	void UpdateInteractionComponent();

	UFUNCTION()
	void FindInteractable();

	UFUNCTION()
	void InitializeColliderEvents();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bUseFuzzyAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FName InteractableTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FName InteractableFocusTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	float InteractionRangeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	float InteractionRangeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	bool bTraceComplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	bool bUseCameraManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	bool bSphereTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	float TraceSphereRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming", ClampMin = "0", ClampMax = "1"))
	float InteractableThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming"))
	bool bPrioritizeBasedOnDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming"))
	float DistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming"))
	bool bPrioritizeBasedOnBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming"))
	TArray<FName> BonesToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming"))
	bool bUseDistanceTest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming"))
	int32 DistanceAccuracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming", ClampMin = "0", ClampMax = "1"))
	float TraceDistanceAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Dot", meta = (EditCondition = "bUseFuzzyAiming"))
	bool UseAdditionalTrace;

	UPROPERTY()
	UPrimitiveComponent* OwnerCollider;
};
