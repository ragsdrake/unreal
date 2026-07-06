# STATUS.md — Space Idle Botanist (UE5)

> Durable project memory. Updated and re-output at the start of every working session.
> Rule: no new C++ or Blueprint logic is generated without this context first.

**Last Updated:** 2026-07-06
**Phase:** 1 — Architecture Definition (COMPLETE). Awaiting user approval for Phase 2 (C++ Header Generation).

---

## 1. Current Architecture State

- **Engine target:** UE 5.4+ (PCG framework production-ready). Single C++ game module `SpaceIdleBotanist`, class prefix `SIB`.
- **Repo contents:** folder skeleton (`Source/`, `Config/`, `Content/`, `docs/`), UE-standard `.gitignore`, full class spec in `docs/ARCHITECTURE.md`. No `.uproject`, no C++ headers, no Blueprints yet — those are gated behind Phase 2 approval.
- **Locked design decisions:**
  - **Idle economy** lives in `UIdleEconomySubsystem` (UGameInstanceSubsystem). Accrual is computed from `FDateTime` UTC timestamp deltas, evaluated on demand (collect/load/UI query). A 1 Hz `FTimerManager` heartbeat broadcasts ledger updates for UI. No `Tick()` anywhere in the economy path.
  - **Hub-and-spoke:** Hub = persistent ship-interior level; Spokes = separately loaded sector levels (Earth Orbit, Mars Surface, Asteroid Belt), each owned by an `APlanetarySector` carrying a persisted `int32 GenerationSeed`.
  - **Procedural generation:** custom `UPCGSettings` nodes (`UPCGAsteroidClusterSettings`, `UPCGFloraScatterSettings`) consume the sector seed through `FRandomStream` — deterministic per save, unique per sector. Rendering via ISM/HISM spawners, never per-asteroid actors.
  - **Data-driven:** all gameplay tunables in DataTables (`FResourceDef`, `FMachineDef`, `FSectorDef` : `FTableRowBase`) or `UPROPERTY(EditAnywhere)`. Zero hardcoded gameplay values.
  - **C++/BP split:** C++ = data management, math, subsystems, interfaces, base actors. Blueprint subclasses = mesh/VFX assignment, timeline animations, UMG visual binding only.
  - **Localization:** all `FText` display strings, identifiers, and debug strings strictly English.

## 2. Known Bugs & Performance Bottlenecks

No code exists yet, so no live bugs. Design-time risk register:

- **RISK-001 (overflow):** week-long offline deltas × high-tier yields overflow `int32`. Mandate: resource quantities are `int64` from day one; per-machine `MaxOfflineHours` cap in `FMachineDef`.
- **RISK-002 (determinism):** PCG regeneration on sector load must reproduce identical geometry per seed, or deployed machine transforms desync from terrain. Mandate: `GenerationSeed` is persisted in `USIBSaveGame`, never re-rolled on load.
- **RISK-003 (clock tampering):** user moving the OS clock backward yields negative deltas. Mandate: clamp deltas to `[0, MaxOfflineHours]`.
- **PERF-001:** asteroid belts must render as ISM/HISM instances via the PCG Static Mesh Spawner path.
- **PERF-002:** UI resource counters bind to `FOnResourceLedgerUpdated` delegates; widgets never poll in `NativeTick`.
- **PERF-003:** interaction prompt detection uses a low-rate (10 Hz) timer trace on the character, not per-frame tracing. (Documented exception: movement/camera use the engine's normal input path.)

## 3. Next Immediate Steps

1. **[AWAITING USER APPROVAL] Phase 2a:** generate C++ headers for the Core + Data layer — `USIBGameInstance`, `USIBSaveGame`, `UIdleEconomySubsystem`, `USectorTravelSubsystem`, table row structs, `ISIBInteractable`.
2. **Phase 2b:** Hub classes — `ASIBCharacter`, `ASIBPlayerController`, `AInteractableBase`, `UShipStatusComponent`, `APilotConsole`, `AGeneLab`.
3. **Phase 2c:** Idle + Procedural classes — `APlanetarySector`, `AIdleMachineBase`, `UResourceYieldComponent`, `USIBRandomLibrary`, PCG settings nodes.
4. **Phase 3:** Blueprint subclass map, UMG widget tree, seed DataTable CSVs, `.uproject` + `Build.cs` + `Config/*.ini`.
