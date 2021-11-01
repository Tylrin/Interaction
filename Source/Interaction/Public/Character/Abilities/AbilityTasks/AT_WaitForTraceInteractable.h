// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Engine/CollisionProfile.h"
#include "AT_WaitForTraceInteractable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForTraceInteractableDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
 * Performs a line trace on a timer, looking for an Actor that implements IGSInteractable that is available for interaction.
 * The StartLocations are hardcoded for GASShooter since we can be in first and third person so we have to check every time
 * we trace. If you only have one start location, you should make it more generic with a parameter on your AbilityTask node.
 */
UCLASS()
class INTERACTION_API UAT_WaitForTraceInteractable : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitForTraceInteractableDelegate FoundNewInteractableTarget;

	UPROPERTY(BlueprintAssignable)
	FWaitForTraceInteractableDelegate LostInteractableTarget;

	/**
	* Traces a line on a timer looking for InteractableTargets.
	* @param MinRange The start location of a trace. A save minimum distance away from the camera.
	* @param MaxRange How far to trace.
	* @param TimerPeriod Period of trace timer.
	* @param bShowDebug Draws debug lines for traces.
	*/
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UAT_WaitForTraceInteractable* WaitForTraceInteractable(
		UGameplayAbility* OwningAbility, 
		FName TaskInstanceName, 
		FCollisionProfileName TraceProfile, 
		FVector StartLocation,
		FVector AimDirection, 
		float MinRange = 30.0f, 
		float MaxRange = 200.0f, 
		float TimerPeriod = 0.1f, 
		bool bShowDebug = true
	);

	virtual void Activate() override;

protected:

	float MinRange;

	float MaxRange;

	FVector StartLocation;

	FVector AimDirection;

	float TimerPeriod;

	bool bShowDebug;

	bool bTraceAffectsAimPitch;

	FCollisionProfileName TraceProfile;

	FGameplayAbilityTargetDataHandle TargetData;

	FTimerHandle TraceTimerHandle;

	virtual void OnDestroy(bool AbilityEnded) override;

	/** Traces as normal, but will manually filter all hit actors */
	void LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params, bool bLookForInteractableActor) const;

	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch = false) const;

	bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition) const;

	UFUNCTION()
		void PerformTrace();
};
