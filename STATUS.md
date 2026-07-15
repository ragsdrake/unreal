# STATUS.md — Space Idle Botanist (UE5)

> Durable project memory. Updated and re-output at the start of every working session.
> Rule: no new C++ or Blueprint logic is generated without this context first.

**Last Updated:** 2026-07-07
**Phase:** 2 — C++ Headers + Compile Scaffolding (COMPLETE, UE 5.5). Awaiting user approval for Phase 3 (Logic Implementation + Content).

---

## 1. Current Architecture State

- **Engine target:** UE 5.5. Single C++ module `SpaceIdleBotanist` (prefix `SIB`). Project compiles-ready: `.uproject` (PCG + EnhancedInput plugins), Game/Editor targets (`BuildSettingsVersion.V5`, `IncludeOrderVersion.Unreal5_5`), Build.cs (deps: EnhancedInput, UMG, PCG, GameplayTags), primary game module with `LogSIB`, Config INIs.
- **All 7 layers transcribed from `docs/ARCHITECTURE.md` into code** (headers + stub/partial cpps):
  - **Data:** `FResourceDef`, `FMachineDef`, `FSectorDef` (FTableRowBase), `FDeployedMachineRecord` (UTC-tick timestamps), `SIBDelegates.h` (central dynamic delegates + `EJumpDriveState`).
  - **Interfaces:** `ISIBInteractable` (BlueprintNativeEvent: Interact / GetInteractionPrompt / CanInteract).
  - **Core:** `USIBGameInstance` (working save/load via UGameplayStatics, stamps `LastSessionUtcTicks` on Shutdown), `USIBSaveGame`, `UIdleEconomySubsystem` (working ledger + TrySpend + FTimerManager heartbeat; offline-progress math stubbed), `USectorTravelSubsystem` (working timer-driven jump state machine; level-open stubbed), thin game modes, `ASIBPlayerController` (Enhanced Input contexts, UI/game input modes).
  - **Hub:** `ASIBCharacter` (Enhanced Input, FP/TP toggle, working 10 Hz timer interaction trace + focus-changed delegate), `AInteractableBase` (+ BP highlight hooks), `APilotConsole`, `AGeneLab` (working TrySpend), `UShipStatusComponent` (working clamped setters + delegates).
  - **Idle:** `APlanetarySector` (persisted seed, machine registry), `AIdleMachineBase` (DataTable row resolution, working collect flow), `UResourceYieldComponent` (**working clamped timestamp-delta accrual math**).
  - **Procedural:** `USIBRandomLibrary` (working CRC-based `DeriveSeed` — content-hashed, session-stable; jittered grid scatter), `USectorGenParams`, `UPCGAsteroidClusterSettings`, `UPCGFloraScatterSettings` (custom `UPCGSettings` + `IPCGElement`; pins + seed wiring done, `ExecuteInternal` math stubbed).
  - **UI:** `USIBWidgetBase` (delegate lifecycle), `UShipStatusWidget`, `UNavigationWidget`, `USectorOverviewWidget` — all English `NSLOCTEXT` labels: `[Hull Integrity: {n}%]`, `[Autopilot: Engaged/Disengaged]`, `[Destination: {Sector}]`, `[Initialize Jump Drive]`, `[Deploy Thermal Generator]`, `[Automated Mineral Export]`.
- **Constraint compliance verified by static check:** zero `Tick()`/`NativeTick` usage; every actor/component constructor disables tick; every header has `#pragma once` + correctly named last-position `.generated.h`; every UCLASS has a cpp; subsystem tunables live in `DefaultGame.ini`, everything else is `UPROPERTY(EditAnywhere)` or DataTable-driven.
- Stub bodies are marked `// Phase 3:` with exact intended behavior.

## 2. Known Bugs & Performance Bottlenecks

- **VERIFY-001 (OPEN):** No UE 5.5 toolchain exists in this workspace, so the module has not been compiled. First user-side compile (generate project files → build `SpaceIdleBotanistEditor`) is the open verification item. Most likely friction points, in order: (a) PCG API drift — `bUseSeed` member and `IPCGElement` inheritance in the two PCG settings classes; (b) `FFloatInterval` inline UPROPERTY defaults; everything else is textbook UE boilerplate.
- **RISK-001 (mitigated in code):** ledger and yields are `int64`; `MaxOfflineHours` clamp lives in `FMachineDef` and is enforced in `UResourceYieldComponent::ComputePendingYield`.
- **RISK-002 (mitigated in design, wiring pending):** seeds persist in `USIBSaveGame.SectorSeeds`; `USIBRandomLibrary::DeriveSeed` hashes content (CRC of salt string), never FName runtime indices. Phase 3 must wire `ASIBGameModeSector::InitializeSectorGeneration` to restore — never re-roll — the seed.
- **RISK-003 (mitigated in code):** negative timestamp deltas clamped to 0 in `ComputePendingYield`.
- **PERF-001 (open until Phase 3):** PCG `ExecuteInternal` must emit points consumed by Static Mesh Spawner in ISM/HISM mode; no per-asteroid actors.
- **PERF-002 (mitigated in code):** all widgets refresh only from delegate broadcasts; heartbeat is a config-driven 1 Hz timer.
- **PERF-003 (mitigated in code):** interaction detection is a 10 Hz configurable timer trace.

## 3. Next Immediate Steps

1. **[AWAITING USER APPROVAL] Phase 3a — logic completion:** offline-progress calculation + deploy persistence in `UIdleEconomySubsystem`, sector level open in `USectorTravelSubsystem`, seed restore in `ASIBGameModeSector`, PCG `ExecuteInternal` scatter math for both nodes.
2. **Phase 3b — content wiring:** DataTable CSVs (DT_Resources/DT_Machines/DT_Sectors), Enhanced Input assets (IMC_Hub, IA_*), Blueprint children (BP_SIBCharacter, BP_PilotConsole, BP_GeneLab, machines) and WBP_ widgets.
3. **Phase 3c — first compile & smoke test** on the user's machine; burn down VERIFY-001 and any PCG API friction.
