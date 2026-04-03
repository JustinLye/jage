# ADR-0005: Input Adapter Implementation Iteration

**Status:** Accepted
**Date:** 2026-04-03
**Authors:** Codex (GPT-5.3), Justin Lye
**Amends:** [ADR-0003](0003-input-adapter-design.md)

---

## Context

ADR-0003 described a target architecture where each input device type would have its own adapter implementing a shared `input_adapter` concept with `flush`, `empty`, and `capacity` operations. The implemented design differs in several important ways:

1. GLFW input is implemented as a single adapter that registers and handles keyboard, mouse button, cursor, and scroll callbacks in one type.
2. Buffered handoff is delegated to a context-level event sink (`contexts::glfw`) instead of a per-adapter `flush` API.
3. The event model has converged on a single timestamped event envelope with a `std::variant` payload across supported input event types.
4. Cursor behavior includes implementation-specific logic for deduplication and mode-aware mapping (absolute position when cursor is normal, relative motion when cursor is disabled).

These are not partial placeholders; they are real, tested implementation behavior and should be the documented design baseline.

---

## Decision

### 1. Adopt a unified GLFW callback adapter as the canonical implementation shape

The GLFW adapter remains a single adapter type responsible for callback registration and callback-to-event translation for:

- keyboard key events
- mouse button click events
- cursor position and cursor motion events
- vertical and horizontal scroll events

This supersedes ADR-0003's "one adapter per device type" direction for the GLFW backend.

### 2. Treat buffering as a context/event-sink responsibility, not an adapter concept contract

The implementation routes emitted events through `contexts::glfw`, which forwards to an injected event sink via `push`.

Accordingly, the system no longer treats `flush/empty/capacity` as a required adapter-level interface for the current GLFW path. Consumers that need draining policy are expected to implement it on top of the sink/bus they choose.

### 3. Standardize on a unified timestamped event envelope

The effective design is:

- `input::event<TDuration>` as the public event type
- timestamp carried at envelope level
- payload represented as `std::variant` over supported event payload structs

This envelope model is the documented integration point for input pipelines and tests.

### 4. Document callback semantics now encoded in implementation

The GLFW adapter behavior now explicitly includes:

- modifier mapping from GLFW flags to the engine modifier bitset
- logical key mapping and physical scancode mapping
- cursor event deduplication when position has not materially changed
- cursor mode split: disabled cursor emits motion deltas; normal cursor emits absolute position
- scroll axis split: non-zero `yoffset` emits vertical scroll, otherwise horizontal scroll

---

## Consequences

### Positive

1. Documentation now matches tested implementation across keyboard, mouse button, cursor, and scroll paths.
2. The architecture is simpler to reason about for the GLFW backend: one adapter for callback wiring and event translation, one context for sink handoff.
3. The unified event envelope reduces API surface area and avoids parallel per-device adapter contracts that are not currently used.

### Negative

1. This creates intentional divergence from the more granular adapter decomposition proposed in ADR-0003.
2. There is no generic adapter-level flush contract at present, so buffering/drain behavior lives at sink/bus integration points.

### Neutral

1. Future backends may still choose different decomposition boundaries, but GLFW's implemented baseline is now explicit.

---

## References

- **Amends:** [ADR-0003: Input Adapter Design](0003-input-adapter-design.md)
- **Unified GLFW adapter:** `libs/engine/include/jage/engine/input/adapters/glfw.hpp`
- **GLFW context sink forwarding:** `libs/engine/include/jage/engine/input/contexts/glfw.hpp`
- **Unified input event envelope:** `libs/engine/include/jage/engine/input/event.hpp`, `libs/engine/include/jage/engine/input/internal/event.hpp`

---

## Revision History

| Date       | Author            | Changes                                      |
|------------|-------------------|----------------------------------------------|
| 2026-04-03 | Codex (GPT-5.3)   | Initial ADR, amending ADR-0003              |
