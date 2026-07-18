# Space Idle Botanist — Blueprint & Content Construction Guide

Phase 3b deliverable. Binary `.uasset` files cannot be generated outside the editor, so this
guide specifies every Blueprint, widget, input asset, DataTable, and level exactly — each
section is a checklist to execute in the UE 5.5 editor. All logic already lives in C++;
Blueprint work here is wiring, visuals, and layout only.

Naming: `BP_` actors, `WBP_` widgets, `IA_`/`IMC_` input, `DT_` tables, `GP_` gen params,
`L_` levels, `PCG_` graphs. All names and display text in English.

---

## 1. DataTables (Content/Data/)

Import the three CSVs from `SourceData/DataTables/` via **Content Browser → Import**, choosing
the matching row struct when prompted:

| CSV | Row struct | Asset name |
|---|---|---|
| DT_Resources.csv | `ResourceDef` | `DT_Resources` |
| DT_Machines.csv | `MachineDef` | `DT_Machines` |
| DT_Sectors.csv | `SectorDef` | `DT_Sectors` |

Re-import after editing a CSV; keep CSVs as the source of truth (no hand-edited rows).

## 2. Game Instance (Content/Core/)

1. Create `BP_SIBGameInstance` (parent: `SIBGameInstance`).
2. Set **Resource Table / Machine Table / Sector Table** to the three DataTables.
3. Project Settings → Maps & Modes → Game Instance Class → `BP_SIBGameInstance`
   (overrides the C++ default set in DefaultEngine.ini).

## 3. Enhanced Input (Content/Core/Input/)

Input Actions: `IA_Move` (Axis2D), `IA_Look` (Axis2D), `IA_Interact` (Digital),
`IA_ToggleCamera` (Digital).

`IMC_Hub` mappings: WASD → IA_Move (W: Swizzle YXZ; S: Swizzle YXZ + Negate; A: Negate; D: none),
Mouse XY → IA_Look (Y: Negate), E → IA_Interact, V → IA_ToggleCamera. Duplicate as `IMC_Sector`
(same bindings today; diverges later for piloting).

## 4. Player & Controller Blueprints (Content/Hub/Blueprints/)

- `BP_SIBCharacter` (parent: `SIBCharacter`): assign the four Input Action assets; add
  skeletal/static meshes; optionally re-implement camera toggle smoothing with a Timeline
  driving `CameraBoom.TargetArmLength` between the two exposed arm lengths.
- `BP_SIBPlayerController` (parent: `SIBPlayerController`): set **Hub/Sector Mapping Context**
  to the IMCs; set **Hud Widget Class** to `WBP_HUD`; on BeginPlay create + add the HUD widget.

## 5. Hub Equipment Blueprints (Content/Hub/Blueprints/)

All parents already implement `ISIBInteractable`; children add visuals and implement the
BlueprintImplementableEvents:

- `BP_PilotConsole` (parent: `PilotConsole`): set **Sector Table**; implement
  `OnOpenNavigationUI` → create `WBP_Navigation`, add to viewport, call controller
  `EnterUIMode`. Implement `OnHighlightBegin/End` (e.g. custom-depth outline).
- `BP_GeneLab` (parent: `GeneLab`): set **Splice Costs**; implement `OnGenomeSpliced` VFX.
- `BP_ShipCore` (parent: `Actor`): the ship's status owner — add a `ShipStatusComponent`;
  hub HUD finds it and calls `WBP_ShipStatus.InitializeFromShip`.

## 6. Machine Blueprints (Content/Sectors/Machines/)

One per DT_Machines row, parent `IdleMachineBase`, at the exact paths in the CSV
(`BP_ThermalGenerator`, `BP_MineralExtractor`, `BP_SilicateHarvester`, `BP_BotanyPod`):
add meshes under the C++ Root, implement `OnProductionStateChanged` (working VFX/timeline).
Do NOT set Machine Def Row on the class — deploy/restore injects it.

## 7. Widgets (Content/UI/)

Parents are the C++ bases; layout is free-form, but implement these events:

- `WBP_ShipStatus` (parent `ShipStatusWidget`): two TextBlocks; `OnShipStatusTextChanged`
  assigns the pre-formatted `[Hull Integrity: {n}%]` / `[Autopilot: ...]` texts.
- `WBP_Navigation` (parent `NavigationWidget`): set **Sector Table**; list view built from
  `GetDestinationRows` + `GetDestinationLabel`; a button labeled with
  `GetJumpDriveActionLabel()` calling `InitializeJumpDrive(SelectedRow)`;
  `OnJumpDriveStateChanged` drives charge feedback and closes the menu on `Jumping`.
- `WBP_SectorOverview` (parent `SectorOverviewWidget`): machine list + resource readouts
  rebuilt in `OnLedgerRefreshed` from `GetEconomySubsystem()` (never on tick); deploy buttons
  call `RequestDeployMachine(Row, Transform)` — use the pawn's location + forward offset as
  the spawn transform for now; toggle bound to `SetAutomatedMineralExport`.
- `WBP_InteractionPrompt` / `WBP_HUD` (parent `SIBWidgetBase`): HUD binds the pawn's
  `OnFocusedInteractableChanged`, shows `GetInteractionPrompt` text of the focused actor.

## 8. Sector Gen Params (per sector folder)

Create `GP_EarthOrbit`, `GP_MarsSurface`, `GP_AsteroidBelt`, `GP_LunarCrater`
(Misc → Data Asset → `SectorGenParams`) at the CSV paths. Fill MeshPalette/MeshWeights,
PointsPerHectare, scale/slope/altitude ranges per biome (Mars: slope 0–35°, flora-heavy;
Asteroid Belt params live mostly on the PCG node instead).

## 9. PCG Graphs (Content/PCG/)

- `PCG_AsteroidBelt`: **Asteroid Cluster Scatter** (custom node) → **Static Mesh Spawner**.
  Spawner mesh entries: asteroid meshes with weights; ISM/HISM descriptors (PERF-001:
  instanced rendering only). No surface input needed — the node's FallbackFieldExtents
  defines the belt volume.
- `PCG_Flora`: **Get Landscape Data** (or surface input) → **Flora Scatter** (custom node,
  set its GenParams to the sector's GP_ asset) → **Static Mesh Spawner** with weighted plant
  meshes mirroring the GP_ MeshPalette/MeshWeights.
- Graph seeds are irrelevant at edit time: `APlanetarySector.TriggerGeneration()` stamps each
  PCG component's seed (salted per actor) at runtime — never hardcode seeds in graphs.

## 10. Levels

- `L_ShipHub` (Content/Hub/): modular interior, `BP_ShipCore`, `BP_PilotConsole`, `BP_GeneLab`,
  Player Start. World Settings → GameMode Override → `SIBGameModeHub` (or a BP child).
- Sector levels (`L_EarthOrbit`, `L_MarsSurface`, `L_AsteroidBelt`, `L_LunarCrater`): GameMode
  Override → `SIBGameModeSector`; place one **PlanetarySector** actor with **Sector Row Name**
  set to its DT_Sectors row (e.g. `Sector.MarsSurface`) and **Managed PCG Actors** pointing at
  the level's PCG volume actors; place a PCG volume per graph; Player Start.
- Set Project Settings → Maps & Modes → Game/Editor Default Map → `L_ShipHub` once it exists.
- Return trip: the `Sector.Hub` row makes the hub a destination in `WBP_Navigation`.

## 11. Smoke-Test Script (after first compile)

1. PIE in `L_ShipHub`: WASD/mouse move, V toggles FP/TP, aiming at the consoles fires the
   interaction prompt via the 10 Hz scan (watch `OnFocusedInteractableChanged`).
2. Pilot console → `[Destination: Mars Surface]` → `[Initialize Jump Drive]`: charge state
   fires `OnJumpStateChanged`, level opens, PCG populates identically on every revisit
   (seed persisted in the save — RISK-002).
3. Deploy a machine, quit to desktop, relaunch after a few minutes: the ledger shows offline
   accrual (clamped by MaxOfflineHours) and the machine respawns at its recorded transform.
4. `stat unit` + `stat pcg` in the Asteroid Belt: instance counts high, actor count flat
   (PERF-001), no per-frame cost from idle logic (no Tick anywhere).
