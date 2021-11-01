// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AbilityTasks/AT_WaitForTraceInteractable.h"
#include "Character/InteractableInterface.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "GASBlueprintLibrary.h"
#include "TimerManager.h"

UAT_WaitForTraceInteractable::UAT_WaitForTraceInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTraceAffectsAimPitch = true;
}

UAT_WaitForTraceInteractable* UAT_WaitForTraceInteractable::WaitForTraceInteractable(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	FCollisionProfileName TraceProfile,
	FVector StartLocation,
	FVector AimDirection,
	float MinRange,
	float MaxRange, 
	float TimerPeriod, 
	bool bShowDebug
)
{
	UAT_WaitForTraceInteractable* MyObj = NewAbilityTask<UAT_WaitForTraceInteractable>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->TraceProfile = TraceProfile;
	MyObj->StartLocation = StartLocation;
	MyObj->AimDirection = AimDirection;
	MyObj->MinRange = MinRange;
	MyObj->MaxRange = MaxRange;
	MyObj->TimerPeriod = TimerPeriod;
	MyObj->bShowDebug = bShowDebug;

	return MyObj;
}

void UAT_WaitForTraceInteractable::Activate()
{
	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TraceTimerHandle, this, &UAT_WaitForTraceInteractable::PerformTrace, TimerPeriod, true);
}

void UAT_WaitForTraceInteractable::OnDestroy(bool AbilityEnded)
{
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(TraceTimerHandle);

	Super::OnDestroy(AbilityEnded);
}

void UAT_WaitForTraceInteractable::LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params, bool bLookForInteractableActor) const
{

}

void UAT_WaitForTraceInteractable::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch) const
{
	if (!Ability) // Server and launching client only
	{
		return;
	}

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	// Default to TraceStart if no PlayerController
	FVector ViewStart = TraceStart;
	FRotator ViewRot(0.0f);
	if (PC)
	{
		PC->GetPlayerViewPoint(ViewStart, ViewRot);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No PC for player view point location and rotation"));
	}

	const FVector ViewDir = ViewRot.Vector();
	ViewStart = ViewStart + (ViewDir * MinRange); // Get Trace start including Min Range
	FVector ViewEnd = ViewStart + (ViewDir * MaxRange); // Get normal Trace End

	DrawDebugLine(GetWorld(), ViewStart, ViewEnd, FColor::Green, false, TimerPeriod);

	ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

	FHitResult HitResult;
	LineTrace(HitResult, InSourceActor->GetWorld(), ViewStart, ViewEnd, TraceProfile.Name, Params, false);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));

	const FVector AdjustedEnd = (bUseTraceResult) ? HitResult.Location : ViewEnd;

	FVector AdjustedAimDir = (AdjustedEnd - ViewStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = ViewDir;
	}

	if (!bTraceAffectsAimPitch && bUseTraceResult)
	{
		FVector OriginalAimDir = (ViewEnd - ViewStart).GetSafeNormal();

		if (!OriginalAimDir.IsZero())
		{
			// Convert to angles and use original pitch
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = ViewStart + (AdjustedAimDir * MaxRange);
}

bool UAT_WaitForTraceInteractable::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition) const
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);
	if (DotToCenter >= 0)		//If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;						//Subtracting instead of adding will get the other intersection point
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);		//Cam aim point clipped to range sphere
			return true;
		}
	}
	return false;
}

void UAT_WaitForTraceInteractable::PerformTrace()
{
	UE_LOG(LogTemp, Warning, TEXT("Perform Trace"));

	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	AActor* SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!SourceActor)
	{
		// Hero is dead
		//UE_LOG(LogTemp, Error, TEXT("%s %s SourceActor was null"), *FString(__FUNCTION__), *UGSBlueprintFunctionLibrary::GetPlayerEditorWindowRole(GetWorld()));
		return;
	}

	ActorsToIgnore.Add(SourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_SingleLineTrace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	// Calculate TraceEnd
	FVector TraceStart = StartLocation;
	FVector TraceEnd;
	AimWithPlayerController(SourceActor, Params, TraceStart, TraceEnd); //Effective on server and launching client only

	// ------------------------------------------------------

}
