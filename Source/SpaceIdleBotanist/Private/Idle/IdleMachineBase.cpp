// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Idle/IdleMachineBase.h"

#include "Core/IdleEconomySubsystem.h"
#include "Engine/GameInstance.h"
#include "Idle/ResourceYieldComponent.h"
#include "SpaceIdleBotanist.h"

AIdleMachineBase::AIdleMachineBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	YieldComponent = CreateDefaultSubobject<UResourceYieldComponent>(TEXT("YieldComponent"));
}

void AIdleMachineBase::BeginPlay()
{
	Super::BeginPlay();

	if (const FMachineDef* Def = MachineDefRow.GetRow<FMachineDef>(TEXT("AIdleMachineBase::BeginPlay")))
	{
		CachedDef = *Def;
		bDefResolved = true;
		OnProductionStateChanged(true);
	}
	else
	{
		UE_LOG(LogSIB, Warning, TEXT("%s has no valid DT_Machines row assigned; it will not produce."), *GetName());
	}
}

void AIdleMachineBase::Interact_Implementation(APawn* InstigatorPawn)
{
	CollectPendingYield();
}

FText AIdleMachineBase::GetInteractionPrompt_Implementation() const
{
	return CachedDef.DisplayName;
}

bool AIdleMachineBase::CanInteract_Implementation(const APawn* InstigatorPawn) const
{
	return bDefResolved;
}

int64 AIdleMachineBase::GetPendingYield() const
{
	if (!bDefResolved || !YieldComponent)
	{
		return 0;
	}
	return YieldComponent->ComputePendingYield(CachedDef.YieldPerSecond, CachedDef.MaxOfflineHours);
}

int64 AIdleMachineBase::CollectPendingYield()
{
	const int64 Pending = GetPendingYield();
	if (Pending <= 0)
	{
		return 0;
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UIdleEconomySubsystem* Economy = GameInstance->GetSubsystem<UIdleEconomySubsystem>())
		{
			Economy->AddResource(CachedDef.YieldResourceRow, Pending);
		}
	}

	YieldComponent->MarkCollected();
	return Pending;
}
