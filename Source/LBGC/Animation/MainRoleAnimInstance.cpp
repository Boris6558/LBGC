// Fill out your copyright notice in the Description page of Project Settings.


#include "MainRoleAnimInstance.h"
#include "../Character/MainRole.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include <Components/CapsuleComponent.h>
#include <Engine/EngineTypes.h>
#include <Kismet/KismetSystemLibrary.h>
#include "../GameInstance/LBGCGameInstance.h"

namespace
{
const float g_fFootIkInterpSpeed = 15.f;
}

UMainRoleAnimInstance::UMainRoleAnimInstance()
	: m_mainRole(NULL)
	, MoveSpeed(0.f)
	, IsJumping(false)
	, m_fLFootGroundDistance(0.f)
	, m_fRFootGroundDistance(0.f)
{
}

void UMainRoleAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UMainRoleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateMainRoleAnimationProperties(DeltaSeconds);
	UpdateFootIK(DeltaSeconds);
}

void UMainRoleAnimInstance::UpdateMainRoleAnimationProperties(float fDeltaSeconds)
{
	if (!CheckMainRole()) { return; }

	MoveSpeed = m_mainRole->GetMoveSpeed();
	if (m_mainRole->GetMovementComponent())
	{
		IsJumping = m_mainRole->GetMovementComponent()->IsFalling();
	}
}

void UMainRoleAnimInstance::UpdateFootIK(float fDeltaSeconds)
{
	if (!TraceCheckFootIK(FOOT_IK_L_TRACE) || !TraceCheckFootIK(FOOT_IK_R_TRACE))
	{
		PelvisZ = FMath::FInterpTo(PelvisZ, 0.f, fDeltaSeconds, g_fFootIkInterpSpeed);
		LFootEffectorX = FMath::FInterpTo(LFootEffectorX, 0.f, fDeltaSeconds, g_fFootIkInterpSpeed);
		RFootEffectorX = FMath::FInterpTo(RFootEffectorX, 0.f, fDeltaSeconds, g_fFootIkInterpSpeed);
		return;
	}

	float fMinDistrance = FMath::Min(m_fLFootGroundDistance, m_fRFootGroundDistance);
	PelvisZ = FMath::FInterpTo(PelvisZ, fMinDistrance, fDeltaSeconds, g_fFootIkInterpSpeed);

	if (m_fLFootGroundDistance < m_fRFootGroundDistance)
	{
		// The right foot is on it
		RFootEffectorX = FMath::FInterpTo(RFootEffectorX, FMath::Abs(PelvisZ) + m_fRFootGroundDistance, fDeltaSeconds, g_fFootIkInterpSpeed);
		LFootEffectorX = FMath::FInterpTo(LFootEffectorX, 0.f, fDeltaSeconds, g_fFootIkInterpSpeed);
	}
	else
	{
		// The left foot is on it
		LFootEffectorX = FMath::FInterpTo(LFootEffectorX, FMath::Abs(PelvisZ) + m_fLFootGroundDistance, fDeltaSeconds, g_fFootIkInterpSpeed);
		RFootEffectorX = FMath::FInterpTo(RFootEffectorX, 0.f, fDeltaSeconds, g_fFootIkInterpSpeed);
	}

	if (LBGC_INSTANCE)
	{
		LBGC_INSTANCE->PrintDebugMessageOnScreen(-1, 5.f, FColor::Red, 
			FString::Printf(TEXT("m_fLFootGroundDistance[%02f]�� m_fRFootGroundDistance[%02f]"), m_fLFootGroundDistance, m_fRFootGroundDistance));
	}

}

bool UMainRoleAnimInstance::CheckMainRole()
{
	if (!m_mainRole)
	{
		m_mainRole =  Cast<AMainRole>(TryGetPawnOwner());
		if (!m_mainRole)
		{
			return false;
		}
	}
	return true;
}

bool UMainRoleAnimInstance::TraceCheckFootIK(EnFootIkTraceType type)
{
	if (!CheckMainRole() || 
		!m_mainRole->GetMesh() ||
		!m_mainRole->GetCapsuleComponent())
	{
		return false;
	}

	FVector VecStartLoc;
	FVector VecEndLoc;

	/*
		Trace range��
		Z : half of GetScaledCapsuleHalfHeight -> A GetCapsuleComponent length ray [knee to sole]
		X Y: foot socket location
	*/
	FVector vecFootSocket;
	if (type == FOOT_IK_L_TRACE)
	{
		vecFootSocket = m_mainRole->GetMesh()->GetSocketLocation(FName("foot_lSocket"));
	}
	else
	{
		vecFootSocket = m_mainRole->GetMesh()->GetSocketLocation(FName("foot_rSocket"));
	}

	float fTraceStartZ =
		m_mainRole->GetCapsuleComponent()->GetRelativeLocation().Z -
		m_mainRole->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2.f;
	VecStartLoc.X = vecFootSocket.X;
	VecStartLoc.Y = vecFootSocket.Y;
	VecStartLoc.Z = fTraceStartZ;
	VecEndLoc.X = vecFootSocket.X;
	VecEndLoc.Y = vecFootSocket.Y;
	VecEndLoc.Z = VecStartLoc.Z - m_mainRole->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	/*
	Default ObjectTypeQuery1 ���� WorldStatic
	Default ObjectTypeQuery2 ���� WorldDynamic
	Default ObjectTypeQuery3 ���� Oawn
	Default ObjectTypeQuery4 ���� PhysicasBody
	Default ObjectTypeQuery5 ���� Vehicle
	Default ObjectTypeQuery6 ���� Destructible
	*/
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2);

	FHitResult hitResult;
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), VecStartLoc, VecEndLoc,
		ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, hitResult, true);

	if (!bHit)
	{
		m_fLFootGroundDistance = 0.f;
		m_fRFootGroundDistance = 0.f;
		return false;
	}

	// This distance has a negative sign
	if (type == FOOT_IK_L_TRACE)
	{
		m_fLFootGroundDistance = hitResult.Location.Z - m_mainRole->GetMesh()->GetComponentLocation().Z;
	}
	else
	{
		m_fRFootGroundDistance = hitResult.Location.Z - m_mainRole->GetMesh()->GetComponentLocation().Z;
	}

	return true;
}
