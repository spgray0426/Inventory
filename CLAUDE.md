# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Claude Code Role and Expertise

When assisting with this project, Claude Code operates as a **10+ year experienced Unreal Engine C++ expert and senior code reviewer**. Claude is expected to:

- Apply **Epic Games official coding standards** comprehensively
- Follow **Unreal Engine 5.5 best practices** in all recommendations
- Provide **senior-level code review** with deep technical analysis
- Maintain expertise in modern C++ patterns, UE5 performance optimization, and multiplayer architecture
- Ensure all code contributions meet production-quality standards

## Project Overview

This is an Unreal Engine 5.5 project implementing a spatial grid-based inventory system as a plugin. The project uses the Third Person template and includes a fully-featured inventory plugin called "Inventory".

## Architecture

### Plugin-Based Structure

The inventory system is implemented as a standalone plugin at `Plugins/Inventory/`, allowing it to be reusable across projects. The plugin follows Unreal's modular architecture pattern.

### Core Systems

**Item Fragment System** (`Items/Fragments/`)
- Uses a composition-based design pattern where items are composed of fragments
- `FInv_ItemFragment`: Base fragment struct with GameplayTag identification
- Specialized fragments:
  - `FInv_GridFragment`: Defines grid size and padding for spatial inventory
  - `FInv_ImageFragment`: Visual representation (icon, dimensions)
  - `FInv_StackableFragment`: Stack count and max stack size
- Item manifests (`FInv_ItemManifest`) contain arrays of `TInstancedStruct<FInv_ItemFragment>` to define item behavior
- Fragment retrieval uses templated functions with GameplayTag matching for type-safe access

**Inventory Replication** (`InventoryManagement/FastArray/`)
- Uses Unreal's Fast Array Serialization for efficient network replication
- `FInv_InventoryFastArray`: Custom fast array implementation for inventory items
- `FInv_InventoryEntry`: Individual replicated inventory entry
- Automatically handles delta serialization for multiplayer

**Component Architecture**
- `UInv_InventoryComponent`: Actor component for managing inventory data
  - Server-authoritative inventory operations
  - Delegates for item add/remove events (`OnItemAdded`, `OnItemRemoved`, `NoRoomInInventory`)
  - Replicates inventory state using Fast Array
- `UInv_ItemComponent`: Actor component attached to pickable items in the world
  - Contains `FInv_ItemManifest` defining the item's properties
  - Replicated for multiplayer item pickup

**Widget System** (`Widgets/`)
- `UInv_InventoryBase`: Base class for inventory UI (abstract)
- `UInv_SpatialInventory`: Implements tabbed inventory with category switching (Equippables/Consumables/Craftables)
- `UInv_InventoryGrid`: Grid-based inventory visualization
  - Manages grid slots (`UInv_GridSlot`) and slotted items (`UInv_SlottedItem`)
  - Category filtering via `EInv_ItemCategory`
  - Room availability checking for spatial item placement
- `UInv_HUDWidget`: Main HUD widget
- `UInv_InfoMessage`: HUD message display

**Item Management**
- Items in the inventory are `UInv_InventoryItem` objects (UObject, not Actor)
- Items support replication via `IsSupportedForNetworking()`
- `GetFragment<FragmentType>()` global template function for safe fragment access
- Item manifests use `TInstancedStruct` for type-safe heterogeneous fragment storage

**Category System**
- Items categorized via `EInv_ItemCategory` enum: Equippable, Consumable, Craftable, None
- Grid widgets filter items by category
- Each category has a dedicated grid in the spatial inventory UI

### Utility Systems

**Grid Mathematics** (`InventoryManagement/Utils/`, `Widgets/Utils/`)
- `UInv_InventoryStatics::ForEach2D()`: Template function for 2D grid iteration
  - Converts linear indices to 2D coordinates and vice versa
  - Used for setting grid slot states across item footprints
- `UInv_WidgetUtils`: Position/index conversion utilities for grid layouts

**Slot Availability**
- `FInv_SlotAvailability`: Represents a single slot's capacity
- `FInv_SlotAvailabilityResult`: Complete result of room checking, including stacking support
  - Tracks total room available, remainder, and per-slot details
  - Handles both stackable and non-stackable items

**Interaction System** (`Interaction/`)
- `IInv_Highlightable`: Interface for highlightable objects
- `AInv_HighlightableStaticMesh`: Implementation for static mesh actors

## Development Workflow

### Building the Project

This is an Unreal Engine project. Build it using:
- **Editor**: Open `InventoryProject.uproject` in Unreal Editor 5.5
- **Visual Studio**: Right-click `InventoryProject.uproject` → "Generate Visual Studio project files", then build the solution
- **Rider**: Open the `.uproject` file directly

### Plugin Module

The Inventory plugin is a Runtime module defined in:
- Plugin descriptor: `Plugins/Inventory/Inventory.uplugin`
- Build configuration: `Plugins/Inventory/Source/Inventory/Inventory.Build.cs`

### Code Organization

- **Public headers** in `Plugins/Inventory/Source/Inventory/Public/` - exposed API
- **Private implementations** in `Plugins/Inventory/Source/Inventory/Private/` - internal details
- Naming convention: All inventory classes/structs prefixed with `Inv_`

### Key Patterns to Follow

**Fragment Pattern**
When adding new item behaviors, create new fragment structs derived from `FInv_ItemFragment`:
1. Define the fragment struct in `Items/Fragments/Inv_ItemFragment.h`
2. Add a GameplayTag in `Items/Fragments/Inv_FragmentTags.h` (declare with `UE_DECLARE_GAMEPLAY_TAG_EXTERN`)
3. Define the tag in the corresponding `.cpp` file
4. Add accessors following the existing pattern (getter methods, private UPROPERTY members)

**Grid Iteration**
When working with spatial grid operations, use `UInv_InventoryStatics::ForEach2D()` for iterating over 2D ranges within a linear array.

**Replication**
The inventory system is designed for multiplayer:
- All item operations should be server-authoritative
- Use `Server_` prefixed functions with `UFUNCTION(Server, Reliable)` for client-to-server RPCs
- The Fast Array automatically handles item replication

## Testing

Currently no automated tests are configured. Test in the Unreal Editor with Play-in-Editor (PIE) mode. For multiplayer testing, use PIE with multiple players (Edit → Editor Preferences → Play → Multiplayer Options).

## Important Notes

- This codebase uses Korean comments in some files (e.g., grid types, item manifests)
- The project is on branch `5-item-fragments` - the main branch is `main`
- Fragments use `TInstancedStruct` which requires `#include "StructUtils/InstancedStruct.h"`
- GameplayTags must be registered in the project's GameplayTag settings before use
