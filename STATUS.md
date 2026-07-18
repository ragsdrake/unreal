# STATUS.md — Space Idle Botanist (UE5)

> Durable project memory. Updated and re-output at the start of every working session.
> Rule: no new C++ or Blueprint logic is generated without this context first.

**Last Updated:** 2026-07-07
**Phase:** 3 — Logic Implementation + Content Spec (CODE COMPLETE, UE 5.5). Next gate: editor-side verification (Phase 3c) on the user's machine.

---

## 1. Current Architecture State

- **Engine target:** UE 5.5, module `SpaceIdleBotanist` (prefix `SIB`). Full layer breakdown lives in `docs/ARCHITECTURE.md`; editor construction steps in `docs/BLUEPRINT_GUIDE.md`; DataTable sources in `SourceData/DataTables/*.csv`.
- **All former `// Phase 3:` stubs are now implemented:**
  - **Offline progress:** `UIdleEconomySubsystem::RecalculateOfflineProgress` loads the save ledger + deploy records, credits `ComputeYieldFromTicks` per machine (clamped, int64), advances timestamps. `FlushToSave` round-trips state on every `USIBGameInstance::WriteSaveGame`.
  - **Deploy persistence:** `APlanetarySector::TryDeployMachine` (spend → record → deferred spawn → row + record-index injection), `RestoreDeployedMachines` respawns from records on sector load. Deployed machines use their economy record as the **single accrual authority**; the local `UResourceYieldComponent` only serves hand-placed actors (no double-crediting).
  - **Travel:** `USectorTravelSubsystem::HandleChargeComplete` saves, resolves `FSectorDef.LevelRef`, opens the level; `ASIBGameModeSector::StartPlay` calls `NotifyArrivedInSector` to complete the state machine.
  - **Seeds:** sector game mode restores the persisted seed or first-rolls one (time-entropy, then persisted forever — RISK-002); `APlanetarySector::TriggerGeneration` stamps every managed `UPCGComponent` with a per-actor-salted seed and regenerates.
  - **PCG:** both elements implemented — asteroid cluster scatter (radial-falloff packing, per-point deterministic seeds) and flora scatter (density-derived jittered grid, `SamplePoint` projection, slope/altitude rejection from `USectorGenParams`). Species selection intentionally delegated to the Static Mesh Spawner's weighted entries.
- **Data added:** `FMachineDef.MachineClass` (soft class for deploy spawning). CSV seed content: 5 resources, 4 machines, 5 sectors (incl. `Sector.Hub` for the return trip and `Lunar Crater Alpha`).
- **Content spec:** `docs/BLUEPRINT_GUIDE.md` §1–10 defines every BP/WBP/IMC/GP/PCG/level asset and its wiring; §11 is the smoke-test script.

## 2. Known Bugs & Performance Bottlenecks

- **VERIFY-001 (OPEN, now the critical path):** still no compile — the toolchain lives on the user's machine. Watch-list, in order of likelihood: `Context->GetSeed()` / `bUseSeed` / `CanExecuteOnlyOnMainThread` signatures in the PCG elements, `UPCGComponent::Seed` public access in `APlanetarySector::TriggerGeneration`, `FFloatInterval` inline defaults. Everything else is standard UE API.
- **VERIFY-002 (OPEN):** DataTable CSV import syntax for `TMap<FName,int64>` (BuildCosts) and `TArray<FName>` should be confirmed on first import; adjust the CSVs, not the structs, if the parser complains.
- **KNOWN-001 (accepted for now):** record indices into `UIdleEconomySubsystem::DeployedMachines` are stable only because the array is append-only; machine removal/refunding (not yet a feature) must switch to stable IDs (GUID per record) before any delete path is added.
- **RISK-001 / RISK-003:** mitigated in code (int64 + clamped deltas). **RISK-002:** mitigated end-to-end (restore-never-reroll now wired). **PERF-001:** C++ emits points only; ISM/HISM enforcement is a content rule in the guide (§9). **PERF-002/003:** mitigated in code (delegate-driven UI, 10 Hz scan).

## 3. Next Immediate Steps

1. **Phase 3c (user's machine):** generate project files, build `SpaceIdleBotanistEditor`, report compile errors (PCG watch-list first) for immediate fixes.
2. Import CSVs and build the content per `docs/BLUEPRINT_GUIDE.md`; run the §11 smoke-test script (movement/interaction, jump drive, offline accrual, PCG determinism, `stat pcg`).
3. **Phase 4 candidates (need user prioritization):** machine upgrade flow (UpgradeCurve consumption), `[Automated Mineral Export]` timer logic, gene-splice yield modifier stack, hull damage events during jumps, machine removal (requires KNOWN-001 stable IDs), placement-preview UX for deployment.
