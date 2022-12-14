// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Network/TcpClient.h"
#include "StartupPlayerController.generated.h"

UENUM()
enum class EnSwitchHUD : uint8
{
	ESH_STARTUP,
	ESH_LOGIN,
	ESH_SETTING,
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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		void SwitchToView(EnSwitchHUD target);

	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		void OnlineClick();

	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		void Login();

	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		bool CheckRoleNameSize();

	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Funcv")
		bool CheckRolePasswordSize();

	UFUNCTION(BlueprintNativeEvent, Category = "AStartupPlayerController Func")
		void OnConnectServerOk();

	// User Setting
	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		void OnSwitchWindowsMode();

	UFUNCTION(BlueprintCallable, Category = "AStartupPlayerController Func")
		void OnSwitchVisualEffectQuality();

private:
	void InitFromBeginPlay();
	void ConnectToServer();
	void SwitchHUDToStartUp();
	void SwitchHUDToLogin();
	void SwitchHUDToSetting();

	void SendLoginInfo();
	void OnLoginSC(const uint8* msg);

public:
	// startup HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AStartupPlayerController Var")
		TSubclassOf<UUserWidget> HUDStartupClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AStartupPlayerController Var")
		TSubclassOf<UUserWidget> HUDLoginClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AStartupPlayerController Var")
		TSubclassOf<UUserWidget> HUDSettingClass;

private:
	class UStartupWidget* m_HUDStartup;
	class ULoginWidget* m_HUDLogin;
	class USettingWidget* m_HUDSetting;
	FMsgCallbackToExpectMsg m_dgLoginSC;

	bool m_quieGame;
};
