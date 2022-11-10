// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StartupPlayerController.generated.h"

UENUM()
enum class EnSwitchHUD : uint8
{
	ESH_STARTUP,
	ESH_LOGIN,
};

/**
 * 
 */
UCLASS()
class LBGC_API AStartupPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AStartupPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		void SwitchToView(EnSwitchHUD target);

	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		void OnlineClick();

private:
	void InitFromBeginPlay();
	void SwitchHUDToStartUp();
	void SwitchHUDToLogin();

public:
	// startup HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AStartupPlayerController Var")
		TSubclassOf<UUserWidget> HUDStartupClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AStartupPlayerController Var")
		TSubclassOf<UUserWidget> HUDLoginClass;

private:
	class UStartupWidget* m_HUDStartup;
	class ULoginWidget* m_HUDLogin;

};