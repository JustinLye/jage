# ADR-0006: Entity Component System

**Status:** Draft
**Date:** 2026-04-03
**Authors:** Justin Lye

---

## Context

### Problem Statement

JAGE needs a composition model for gameplay and scene state that avoids rigid
inheritance hierarchies and supports efficient system-driven updates. The
engine also already contains specialized runtimes, most notably input, whose
implementation shape does not naturally fit "everything is stored directly in
ECS."

This creates two related design questions:

1. What role should ECS play inside JAGE?
2. What belongs to the engine's ECS machinery versus the game developer's
   component model?

### Current Architectural Pressure

The existing input runtime establishes an important precedent.

Per [ADR-0005](0005-input-adapter-implementation-iteration.md), input is
currently implemented as:

- a platform-facing GLFW callback adapter
- a context-level event sink handoff
- a unified timestamped event envelope
- downstream consumption from a ring buffer or other sink

Input is therefore already a specialized runtime with explicit capture and
handoff boundaries. It is not modeled as raw platform state stored directly in
ECS.

That matters because the ECS design should align with this direction rather
than fight it.

### Requirements

**Functional:**

1. Entities are lightweight identifiers
2. Components are plain data
3. Systems operate over component sets
4. Components can be added to and removed from entities
5. ECS can serve as the main gameplay and scene state model
6. ECS can integrate with specialized runtimes such as input, rendering, and
   physics

**Non-Functional:**

1. The design should favor cache-friendly iteration
2. The design should expose the real cost of structural changes
3. The design should preserve clear subsystem boundaries
4. The engine should remain extensible by the user
5. The engine should not prescribe game-specific domain models

---

## Decision

### 1. ECS is the gameplay and scene composition model

JAGE will use ECS as the primary model for:

- entity identity
- component composition
- queryable gameplay state
- system-driven updates over stable frame state

ECS is the backbone for world state and gameplay logic, not a universal storage
format for every engine subsystem.

### 2. The engine provides ECS machinery, not game-specific components

JAGE's responsibility is to provide the ECS model and its constraints, such as:

- entity lifecycle
- component storage rules
- query and iteration mechanisms
- structural change semantics
- deferred mutation mechanisms where needed
- scheduling and phase boundaries

The game developer remains responsible for defining most concrete components and
systems.

JAGE should not prescribe gameplay-specific components such as movement intent,
health, inventory state, or similar domain concepts. Those belong to the game
built on top of the engine.

### 3. Engine-defined components are the exception, not the default

The engine may define components only when they are necessary to support
engine-owned subsystem contracts.

Examples may eventually include data such as:

- transform or hierarchy state
- rendering handles or extraction state
- physics handles or sync state

These are integration types owned by engine/runtime boundaries, not examples of
the engine defining gameplay semantics on behalf of the user.

### 4. Specialized runtimes remain outside ECS and integrate through explicit boundaries

JAGE will treat subsystems such as input as specialized runtimes that are not
forced into ECS storage merely because they participate in frame execution.

The input runtime is the reference example:

- the input runtime owns platform callback registration, timestamping, and event
  buffering
- an explicit bridge or phase drains that runtime output
- ECS receives gameplay-facing state derived from that handoff, not raw GLFW
  integration details

This pattern should generalize to other specialized subsystems such as
rendering, physics, animation, and audio.

### 5. Archetype-oriented constraints should shape guidance to users

JAGE expects the ECS design to favor archetype-style iteration characteristics.
That means the engine should steer users toward patterns that minimize
structural churn.

At the design level, the guidance is:

- stable structure, mutable data
- frequent frame-to-frame changes should usually be field updates, not
  add/remove component operations
- component boundaries should be chosen around lifecycle and access patterns,
  not dogma about maximum granularity

This is guidance for users of the ECS, not a mandate that JAGE must define
their gameplay component model.

---

## Consequences

### Positive

1. JAGE gets a clear architectural role for ECS without pretending every
   subsystem must be stored in ECS form.
2. The engine can provide strong ECS machinery while leaving gameplay semantics
   to the developer.
3. Existing subsystem work, especially input, fits naturally into the ECS
   direction through explicit bridge phases.
4. The design stays honest about archetype-oriented tradeoffs by emphasizing
   structural stability.

### Negative

1. Some data will exist at subsystem boundaries rather than in one universal
   storage model.
2. Explicit bridge phases add architectural ceremony compared with directly
   mixing runtime concerns into ECS.
3. The engine must document boundaries clearly so developers know what belongs
   in ECS and what does not.

### Neutral

1. This ADR does not yet commit to a specific entity representation, storage
   implementation, or public ECS API shape.
2. This ADR intentionally leaves most gameplay component design to the engine
   user.

---

## Rejected Directions

### 1. The engine defines the game's component vocabulary

Rejected because components are primarily part of the game's domain model, not
the ECS machinery itself. Baking gameplay semantics into the engine would reduce
extensibility and make JAGE less useful as a general foundation.

### 2. Every participating subsystem must be represented directly inside ECS

Rejected because specialized runtimes have different execution and data-shaping
needs. Input already demonstrates that callback capture, timestamping, and event
buffering are better owned by a dedicated runtime with an explicit handoff into
gameplay state.

### 3. ECS should hide its performance model behind a backend-agnostic abstraction

Rejected because storage and execution tradeoffs materially affect how systems
and components should be designed. The engine should expose meaningful ECS
constraints rather than pretend all implementations are interchangeable.

---

## Open Questions

1. What entity representation should JAGE use?
2. What public ECS API should JAGE expose for world access, views, and deferred
   mutations?
3. What exact bridge model should connect input runtime output to ECS state?
4. Which engine-owned integration components, if any, should be part of a first
   ECS-capable runtime?
5. How explicitly should scheduling phases be modeled in the ECS API?

---

## References

- [ADR-0003: Input Adapter Design](0003-input-adapter-design.md)
- [ADR-0005: Input Adapter Implementation Iteration](0005-input-adapter-implementation-iteration.md)
- `libs/engine/include/jage/engine/input/adapters/glfw.hpp`
- `libs/engine/include/jage/engine/input/contexts/glfw.hpp`
- `libs/engine/include/jage/engine/input/event.hpp`

---

## Revision History

| Date       | Author       | Changes                                             |
|------------|--------------|-----------------------------------------------------|
| 2026-04-03 | Justin Lye   | Initial architecture ADR draft                      |
