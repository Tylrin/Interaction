// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerControlsComponent.h"
#include "GameplayAbilitySpec.h" // for FGameplayAbilitySpecHandle
#include "EnhancedInputComponent.h" // for FInputBindingHandle
#include "AbilityInputBindingComponent.generated.h"

class UInputAction;
class UAbilitySystemComponent;
class UEnhancedInputComponent;

USTRUCT()
struct FAbilityInputBinding
{
	GENERATED_BODY()

	int32  InputID = 0;
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;
};

/** Component that hooks up enhanced input to the ability system input logic */
UCLASS(meta = (BlueprintSpawnableComponent))
class UAbilityInputBindingComponent : public UPlayerControlsComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Abilities")
		void SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Abilities")
		void ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Abilities")
		void ClearAbilityBindings(UInputAction* InputAction);

	//~ Begin UPlayerControlsComponent interface
	virtual void SetupPlayerControls_Implementation(UEnhancedInputComponent* PlayerInputComponent) override;
	virtual void ReleaseInputComponent(AController* OldController) override;
	//~ End UPlayerControlsComponent interface

private:
	void ResetBindings();
	void RunAbilitySystemSetup();
	void OnAbilityInputPressed(UInputAction* InputAction);
	void OnAbilityInputReleased(UInputAction* InputAction);

	void RemoveEntry(UInputAction* InputAction);

	FGameplayAbilitySpec* FindAbilitySpec(FGameplayAbilitySpecHandle Handle);
	void TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding);

private:
	UPROPERTY(transient)
		UAbilitySystemComponent* AbilityComponent;

	UPROPERTY(transient)
		TMap<UInputAction*, FAbilityInputBinding> MappedAbilities;
};
