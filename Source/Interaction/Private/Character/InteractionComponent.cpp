// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/InteractionComponent.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bUseFuzzyAiming = false;
	InteractableTag = "interactable";
	InteractableFocusTag = "interactablefocus";
	InteractionRangeMin = 20.0f;
	InteractionRangeMax = 350.0f;
	bTraceComplex = false;
	bUseCameraManager = true;
	bSphereTrace = false;
	TraceSphereRadius = 5.0f;
	InteractableThreshold = 0.75f;
	bPrioritizeBasedOnDistance = true;
	DistanceThreshold = 100.0f;
	bPrioritizeBasedOnBone = false;
	BonesToIgnore = { "root", "ik_foot_root", "ik_foot_l", "ik_foot_r", "ik_hand_root", "ik_hand_gun", "ik_hand_l", "ik_hand_r", "spine_03", "", "clavicle_l", "clavicle_r", "neck_01" };
	bUseDistanceTest = false;
	DistanceAccuracy = 3;
	TraceDistanceAlpha = 0.4f;
	UseAdditionalTrace = false;
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeInteractionComponent();
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateInteractionComponent();
}

void UInteractionComponent::InitializeInteractionComponent()
{
	return;
}

void UInteractionComponent::UpdateInteractionComponent()
{
	FindInteractable();
}

void UInteractionComponent::FindInteractable()
{
	InitializeColliderEvents();
	return;
}

void UInteractionComponent::InitializeColliderEvents()
{
	// if(GetOwner->GetOwnerCollider()){
	// OwnerCollider = GetOwner->GetOwnerCollider()
	//}
	//else
	//{
	// OwnerCollider = Cast<UPrimitiveComponent>(GetOwner()->RootComponent())
	//}
}