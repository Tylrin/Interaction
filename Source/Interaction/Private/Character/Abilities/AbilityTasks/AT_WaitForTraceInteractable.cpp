// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AbilityTasks/AT_WaitForTraceInteractable.h"
#include "Character/InteractableInterface.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "GASBlueprintLibrary.h"
#include "TimerManager.h"
#include "Interaction.h"

UAT_WaitForTraceInteractable::UAT_WaitForTraceInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTraceAffectsAimPitch = true;
}

UAT_WaitForTraceInteractable* UAT_WaitForTraceInteractable::WaitForTraceInteractable(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	FCollisionProfileName TraceProfile,
	//FVector StartLocation,
	//FVector AimDirection,
	float MinRange,
	float MaxRange, 
	float TimerPeriod, 
	bool bShowDebug
)
{
	UAT_WaitForTraceInteractable* MyObj = NewAbilityTask<UAT_WaitForTraceInteractable>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->TraceProfile = TraceProfile;
	//MyObj->StartLocation = StartLocation;
	//MyObj->AimDirection = AimDirection;
	MyObj->MinRange = MinRange;
	MyObj->MaxRange = MaxRange;
	MyObj->TimerPeriod = TimerPeriod;
	MyObj->bShowDebug = bShowDebug;

	MyObj->TDStartLocation = FGameplayAbilityTargetingLocationInfo();
	
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
	check(World);

	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
	{
		const FHitResult& Hit = HitResults[HitIdx];

		if (!Hit.HitObjectHandle.IsValid() || Hit.HitObjectHandle != Ability->GetCurrentActorInfo()->AvatarActor.Get())
		{
			// If bLookForInteractableActor is false, we're looking for an endpoint to trace to
			if (bLookForInteractableActor && Hit.HitObjectHandle.IsValid())
			{
				// bLookForInteractableActor is true, hit component must overlap COLLISION_INTERACTABLE trace channel
				// This is so that a big Actor like a computer can have a small interactable button.
				if (Hit.Component.IsValid() && Hit.Component.Get()->GetCollisionResponseToChannel(COLLISION_INTERACTABLE)
					== ECollisionResponse::ECR_Overlap)
				{
					// UE_LOG(LogTemp, Warning, TEXT("Collision Overlap"));
					// Component/Actor must be available to interact
					bool bIsInteractable = Hit.HitObjectHandle.FetchActor()->Implements<UInteractableInterface>();

					if (bIsInteractable && IInteractableInterface::Execute_IsAvailableForInteraction(Hit.HitObjectHandle.FetchActor(), Hit.Component.Get()))
					{
						OutHitResult = Hit;
						OutHitResult.bBlockingHit = true; // treat it as a blocking hit

						// UE_LOG(LogTemp, Warning, TEXT("Is Interactable"));

						return;
					}
				}

				OutHitResult.TraceEnd = Hit.Location;
				OutHitResult.bBlockingHit = false; // False means it isn't valid to interact with
				return;
			}

			// This is for the first line trace to get an end point to trace to
			// !Hit.HitObjectHandle.IsValid() implies we didn't hit anything so return the endpoint as a blocking hit
			// Or if we hit something else
			OutHitResult = Hit;
			OutHitResult.bBlockingHit = true; // treat it as a blocking hit
			return;
		}
	}
}

void UAT_WaitForTraceInteractable::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector TraceStart, FVector OutTraceEnd, bool bIgnorePitch) const
{
	if (!Ability) // Server and launching client only
	{
		return;
	}

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	// Default to TraceStart if no PlayerController
	FVector ViewStart;
	FRotator ViewRot(0.0f);
	if (PC)
	{
		PC->GetPlayerViewPoint(ViewStart, ViewRot);
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("No PC for player view point location and rotation"));
	}

	const FVector ViewDir = ViewRot.Vector();
	
	ClipCameraRayToAbilityRange(TraceStart, ViewDir, TraceStart, MaxRange, OutTraceEnd);

	FHitResult HitResult;
	LineTrace(HitResult, InSourceActor->GetWorld(), TraceStart, OutTraceEnd, TraceProfile.Name, Params, false);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));

	const FVector AdjustedEnd = (bUseTraceResult) ? HitResult.Location : OutTraceEnd;

	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = ViewDir;
	}

	if (!bTraceAffectsAimPitch && bUseTraceResult)
	{
		FVector OriginalAimDir = (OutTraceEnd - TraceStart).GetSafeNormal();

		if (!OriginalAimDir.IsZero())
		{
			// Convert to angles and use original pitch
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
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
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	AActor* SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!SourceActor)
	{
		return;
	}

	ActorsToIgnore.Add(SourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_SingleLineTrace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	// Default to TraceStart if no PlayerController
	FRotator ViewRot(0.0f);
	if (PC)
	{
		PC->GetPlayerViewPoint(StartLocation, ViewRot);
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("No PC for player view point location and rotation"));
	}

	const FVector ViewDir = ViewRot.Vector();
	StartLocation = StartLocation + (ViewDir * MinRange); // Get Trace start including Min Range
	FVector TraceEnd = StartLocation + (ViewDir * MaxRange); // Get normal Trace End

	// Calculate TraceEnd
	FVector TraceStart = StartLocation;
	AimWithPlayerController(SourceActor, Params, TraceStart, TraceEnd); //Effective on server and launching client only

	// ------------------------------------------------------

	// Perform the final search trace
	FHitResult ReturnHitResult;
	LineTrace(ReturnHitResult, GetWorld(), TraceStart, TraceEnd, TraceProfile.Name, Params, true);

	// Default to end of trace line if we don't hit a valid, available Interactable Actor
	// bBlockingHit = valid, available Interactable Actor
	if (!ReturnHitResult.bBlockingHit)
	{
		// No valid, available Interactable Actor

		ReturnHitResult.Location = TraceEnd;
		if (TargetData.Num() > 0 && TargetData.Get(0)->GetHitResult()->HitObjectHandle.FetchActor())
		{
			// Previous trace had a valid Interactable Actor, now we don't have one
			// Broadcast last valid target
			LostInteractableTarget.Broadcast(TargetData);
		}

		TargetData = MakeTargetData(ReturnHitResult);
	}
	else
	{
		// Valid, available Interactable Actor

		bool bBroadcastNewTarget = true;

		if (TargetData.Num() > 0)
		{
			const AActor* OldTarget = TargetData.Get(0)->GetHitResult()->HitObjectHandle.FetchActor();

			if (OldTarget == ReturnHitResult.HitObjectHandle.FetchActor())
			{
				// Old target is the same as the new target, don't broadcast the target
				bBroadcastNewTarget = false;
			}
			else if (OldTarget)
			{
				// Old target exists and is different from the new target
				// Broadcast last valid target
				LostInteractableTarget.Broadcast(TargetData);
			}
		}

		if (bBroadcastNewTarget)
		{
			// Broadcast new valid target
			TargetData = MakeTargetData(ReturnHitResult);
			FoundNewInteractableTarget.Broadcast(TargetData);
		}
	}

//#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, TimerPeriod);

		if (ReturnHitResult.bBlockingHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit"));
			DrawDebugSphere(GetWorld(), ReturnHitResult.Location, 20.0f, 16, FColor::Red, false, TimerPeriod);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("not Hit"));
			DrawDebugSphere(GetWorld(), ReturnHitResult.TraceEnd, 20.0f, 16, FColor::Green, false, TimerPeriod);
		}
	}
//#endif // ENABLE_DRAW_DEBUG
}

FGameplayAbilityTargetDataHandle UAT_WaitForTraceInteractable::MakeTargetData(const FHitResult& HitResult) const
{
	/** Note: This will be cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
	return TDStartLocation.MakeTargetDataHandleFromHitResult(Ability, HitResult);
}
