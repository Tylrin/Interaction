// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AbilityTasks/AT_WaitInteractableTarget.h"
#include "DrawDebugHelpers.h"
#include "GASBlueprintLibrary.h"
#include "TimerManager.h"

UAT_WaitInteractableTarget::UAT_WaitInteractableTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTraceAffectsAimPitch = true;
}

UAT_WaitInteractableTarget* UAT_WaitInteractableTarget::WaitForInteractableTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, FCollisionProfileName TraceProfile, float MaxRange, float TimerPeriod, bool bShowDebug)
{
	UAT_WaitInteractableTarget* MyObj = NewAbilityTask<UAT_WaitInteractableTarget>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->TraceProfile = TraceProfile;
	MyObj->MaxRange = MaxRange;
	MyObj->TimerPeriod = TimerPeriod;
	MyObj->bShowDebug = bShowDebug;

	return MyObj;
}

void UAT_WaitInteractableTarget::Activate()
{
	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TraceTimerHandle, this, &UAT_WaitInteractableTarget::PerformTrace, TimerPeriod, true);
}

void UAT_WaitInteractableTarget::OnDestroy(bool AbilityEnded)
{
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(TraceTimerHandle);

	Super::OnDestroy(AbilityEnded);
}

void UAT_WaitInteractableTarget::PerformTrace()
{
	UE_LOG(LogTemp, Warning, TEXT("Perform Trace"));
}