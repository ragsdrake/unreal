# Space Idle Botanist — UE5 Architecture Specification

Phase 1 deliverable. Defines the project folder structure and every required C++ base class
for the Hub (3D active) and Spoke (idle factory) systems. No implementation code lives here;
headers are generated in Phase 2 against this contract.

---

## 1. Project Folder Structure

```
/ (repo root)
├── SpaceIdleBotanist.uproject          # Phase 2 — enables plugins: PCG, EnhancedInput
├── STATUS.md                           # Durable project memory (architecture, bugs, next steps)
├── docs/
│   └── ARCHITECTURE.md                 # This document
├── Config/
│   ├── DefaultEngine.ini               # Phase 2 — default maps, PCG settings
│   ├── DefaultGame.ini                 # Phase 2 — GameInstance class, save slot names
│   └── DefaultInput.ini                # Phase 2 — Enhanced Input runtime settings
├── Source/
│   └── SpaceIdleBotanist/              # Single game module, class prefix "SIB"
│       ├── SpaceIdleBotanist.Build.cs  # Phase 2 — deps: Core, CoreUObject, Engine,
│       │                               #   EnhancedInput, PCG, UMG, GameplayTags
│       ├── Public/
│       │   ├── Core/                   # GameInstance, SaveGame, subsystems, game modes
│       │   ├── Data/                   # FTableRowBase structs, shared enums, delegates
│       │   ├── Hub/                    # Character, controller, ship equipment actors
│       │   ├── Idle/                   # Sectors, machines, yield accrual
│       │   ├── Interfaces/             # UInterface contracts
│       │   ├── Procedural/             # Seeded math library, PCG settings nodes
│       │   └── UI/                     # UUserWidget C++ bases (logic only, no visuals)
│       └── Private/                    # Mirrors Public/ one-to-one
│           ├── Core/  ├── Data/  ├── Hub/  ├── Idle/
│           ├── Interfaces/  ├── Procedural/  └── UI/
└── Content/
    ├── Core/                           # BP_SIBGameInstance child (if needed), input assets
    │   └── Input/                      # IMC_Hub, IMC_Sector, IA_* Enhanced Input assets
    ├── Data/                           # DT_Resources, DT_Machines, DT_Sectors DataTables
    ├── Hub/                            # Ship interior: BP children of equipment bases
    │   ├── Blueprints/                 # BP_GeneLab, BP_PilotConsole, BP_SIBCharacter
    │   └── Environment/                # Modular ship interior meshes/materials
    ├── PCG/                            # PCG_AsteroidBelt, PCG_MarsFlora, PCG_OrbitDebris graphs
    ├── Sectors/
    │   ├── EarthOrbit/                 # L_EarthOrbit level + sector-specific assets
    │   ├── MarsSurface/                # L_MarsSurface level + sector-specific assets
    │   └── AsteroidBelt/               # L_AsteroidBelt level + sector-specific assets
    └── UI/                             # WBP_ children of C++ widget bases
        ├── HUD/                        # WBP_ShipStatus, WBP_InteractionPrompt
        └── Menus/                      # WBP_Navigation, WBP_SectorOverview, WBP_GeneLab
```

**Excluded from version control** (`.gitignore`): `Binaries/`, `Intermediate/`, `Saved/`,
`DerivedDataCache/`, IDE folders.

---

## 2. Required C++ Base Classes

### 2.1 Core — `Source/SpaceIdleBotanist/Public/Core/`

| Class | Parent | Responsibility |
|---|---|---|
| `USIBGameInstance` | `UGameInstance` | Session root. Orchestrates save/load, stamps `LastSessionUtc` (`FDateTime::UtcNow()`) into the save on shutdown/travel so offline progress can be reconstructed. |
| `USIBSaveGame` | `USaveGame` | Serialized state: resource ledger (`TMap<FName, int64>`), array of `FDeployedMachineRecord` (machine row, sector row, transform, `DeployUtc`, `LastCollectUtc`), per-sector `GenerationSeed`, ship status, `LastSessionUtc`. |
| `UIdleEconomySubsystem` | `UGameInstanceSubsystem` | The idle engine. Owns the runtime resource ledger. `RecalculateOfflineProgress()` on load computes `clamp(UtcNow − LastCollectUtc, 0, MaxOfflineHours) × YieldPerSecond` per deployed machine. Runs a 1 Hz `FTimerManager` heartbeat that broadcasts `FOnResourceLedgerUpdated` for UI. **No Tick.** |
| `USectorTravelSubsystem` | `UGameInstanceSubsystem` | Hub↔Spoke transitions. Drives the `[Initialize Jump Drive]` flow as an event-driven state machine (Idle → Charging → Jumping → Arrived) using timers/delegates; loads sector levels from `FSectorDef.LevelRef`. |
| `ASIBGameModeHub` | `AGameModeBase` | Wires hub pawn/controller/HUD classes for the ship-interior map. Thin. |
| `ASIBGameModeSector` | `AGameModeBase` | Wires sector pawn/controller/HUD; notifies `APlanetarySector` to run PCG generation with the persisted seed on map start. Thin. |
| `ASIBPlayerController` | `APlayerController` | Input-mode switching (game vs. UI), routes Enhanced Input contexts (`IMC_Hub` / `IMC_Sector`), owns HUD widget lifetimes. |

### 2.2 Interfaces — `Source/SpaceIdleBotanist/Public/Interfaces/`

| Class | Parent | Responsibility |
|---|---|---|
| `ISIBInteractable` | `UInterface` (Blueprintable) | Contract for anything usable in 3D: `Interact(APawn* Instigator)`, `GetInteractionPrompt() → FText`, `CanInteract(const APawn*) → bool`. Implemented by all lab equipment and deployed machines. |

### 2.3 Hub (3D Active) — `Source/SpaceIdleBotanist/Public/Hub/`

| Class | Parent | Responsibility |
|---|---|---|
| `ASIBCharacter` | `ACharacter` | Player avatar. Enhanced Input bindings, first/third-person camera toggle (spring arm length 0 ↔ N, both `EditAnywhere`). Interaction detection via a 10 Hz timer-driven line trace (`SetTimer`), not per-frame. |
| `AInteractableBase` | `AActor` | Base for all ship equipment. Implements `ISIBInteractable`. Exposes `PromptText` (`FText`, `EditAnywhere`) and `FOnInteracted` delegate. Blueprint children supply meshes, highlight VFX, timeline animations. |
| `APilotConsole` | `AInteractableBase` | Navigation station. Presents destinations from `DT_Sectors` (`[Destination: Lunar Crater Alpha]`), triggers `USectorTravelSubsystem` on `[Initialize Jump Drive]`. |
| `AGeneLab` | `AInteractableBase` | `[Splice Genome]` station. Consumes ledger resources to unlock/upgrade flora yield multipliers defined in DataTable rows. |
| `UShipStatusComponent` | `UActorComponent` | Hull integrity (0–100), autopilot state. All values `EditAnywhere`; changes broadcast `FOnHullChanged` / `FOnAutopilotChanged` for the `[Hull Integrity: 85%]` / `[Autopilot: Disengaged]` HUD readouts. Event-driven only. |

### 2.4 Idle (Spokes) — `Source/SpaceIdleBotanist/Public/Idle/`

| Class | Parent | Responsibility |
|---|---|---|
| `APlanetarySector` | `AInfo` | One per sector level. Holds `SectorRowName` (`FName`) and persisted `int32 GenerationSeed`; registers deployed machines with the economy subsystem; hands the seed to the level's PCG component on generation. |
| `AIdleMachineBase` | `AActor` | Deployable producer (`[Deploy Thermal Generator]`, `[Automated Mineral Export]`). Resolves stats from its `FMachineDef` row handle at `BeginPlay`. Yield accrues purely from timestamp deltas via `UResourceYieldComponent`; collection happens through `ISIBInteractable` or remote UI. Blueprint children add meshes and "working" VFX/timelines. |
| `UResourceYieldComponent` | `UActorComponent` | Reusable accrual math: stores `LastCollectUtc`, computes pending yield on demand, clamps negative/over-cap deltas (RISK-003). Shared by machines and harvestable plants. |

### 2.5 Procedural — `Source/SpaceIdleBotanist/Public/Procedural/`

| Class | Parent | Responsibility |
|---|---|---|
| `USIBRandomLibrary` | `UBlueprintFunctionLibrary` | Deterministic helpers over `FRandomStream`: seed derivation (sector base seed + salt hash), jittered grid scatter, range/curve sampling. Pure functions, callable from C++, BP, and PCG. |
| `USectorGenParams` | `UPrimaryDataAsset` | Per-sector generation tuning: density curves, mesh palettes, scale/rotation ranges, slope/altitude filters. Referenced by `FSectorDef`; everything `EditAnywhere`. |
| `UPCGAsteroidClusterSettings` | `UPCGSettings` (+ `FPCGElement`) | Custom PCG node: seeded asteroid cluster scatter (cluster centers → per-cluster point clouds with size falloff). Seed input wired from `APlanetarySector`; output feeds Static Mesh Spawner in ISM/HISM mode (PERF-001). |
| `UPCGFloraScatterSettings` | `UPCGSettings` (+ `FPCGElement`) | Custom PCG node: seeded plant placement on terrain with slope/altitude rejection and species selection from `USectorGenParams`. Unique per seed — no identical generations. |

### 2.6 Data — `Source/SpaceIdleBotanist/Public/Data/`

| Type | Parent | Responsibility |
|---|---|---|
| `FResourceDef` | `FTableRowBase` | `DisplayName` (`FText`), icon (soft ref), `bIsExportable`. Rows keyed by `FName` (e.g. `Res.Mineral.Iron`). |
| `FMachineDef` | `FTableRowBase` | `DisplayName`, yield resource row name, `YieldPerSecond` (`double`), `BuildCosts` (`TMap<FName, int64>`), `UpgradeCurve` (soft `UCurveFloat`), `MaxOfflineHours` (`float`). |
| `FSectorDef` | `FTableRowBase` | `DisplayName` (`FText`, e.g. "Lunar Crater Alpha"), `LevelRef` (`TSoftObjectPtr<UWorld>`), `GenParams` (soft `USectorGenParams`), `BaseSeed` (`int32`), allowed machine rows. |
| `FDeployedMachineRecord` | `USTRUCT` (plain) | Save-file record: machine row, sector row, `FTransform`, `DeployUtc`, `LastCollectUtc` (both `FDateTime` as ticks). |
| `SIBDelegates.h` | — | Central multicast delegate declarations: `FOnResourceLedgerUpdated`, `FOnHullChanged`, `FOnAutopilotChanged`, `FOnJumpStateChanged`, `FOnMachineDeployed`. |

### 2.7 UI — `Source/SpaceIdleBotanist/Public/UI/`

C++ bases hold logic and delegate wiring; `WBP_` Blueprint children hold all layout,
styling, and animation. All display strings are `FText` in English.

| Class | Parent | Responsibility |
|---|---|---|
| `USIBWidgetBase` | `UUserWidget` | Cached subsystem accessors, helpers to bind/unbind economy and ship delegates safely. No `NativeTick` logic (PERF-002). |
| `UShipStatusWidget` | `USIBWidgetBase` | Formats `[Hull Integrity: 85%]`, `[Autopilot: Disengaged]` from `UShipStatusComponent` delegates. |
| `UNavigationWidget` | `USIBWidgetBase` | Destination list from `DT_Sectors` → `[Destination: <Name>]`; `[Initialize Jump Drive]` action routed to `USectorTravelSubsystem`. |
| `USectorOverviewWidget` | `USIBWidgetBase` | Deployed-machine list, pending yields, `[Automated Mineral Export]` toggle, `[Deploy Thermal Generator]` action. Refreshes on `FOnResourceLedgerUpdated`, never polls. |

---

## 3. Enforcement of Banned Outcomes

| Ban | Architectural enforcement |
|---|---|
| No Event Tick abuse | Economy = timestamp deltas + 1 Hz `FTimerManager` heartbeat; interaction trace = 10 Hz timer; UI = delegate-driven. `PrimaryActorTick.bCanEverTick = false` on all gameplay base classes in Phase 2. |
| No hardcoded values | Every tunable is a `UPROPERTY(EditAnywhere)` or a DataTable row (`DT_Resources`, `DT_Machines`, `DT_Sectors`) / `USectorGenParams` asset. |
| No identical clones | All PCG nodes and scatter math consume `FRandomStream` seeded from `APlanetarySector.GenerationSeed` (persisted per save, unique per sector, salted per node). |
