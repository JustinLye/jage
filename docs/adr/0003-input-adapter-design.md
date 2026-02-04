# ADR-0003: Input Adapter Design

**Status:** Draft
**Date:** 2026-02-04
**Authors:** Justin Lye, Claude (Opus 4.5)

---

## Context

### Problem Statement

Games receive input from multiple sources (keyboard, mouse, gamepad, network, AI, replay files) that must be:
1. Captured with precise timestamps
2. Buffered for thread-safe handoff
3. Correlated with game frames via `snapshot_cache`
4. Abstracted for testability (mock vs real implementations)

### Requirements

**Functional:**
1. Capture raw input events from platform (GLFW, SDL, Win32, etc.)
2. Timestamp events with highest available precision
3. Buffer events for consumption by game loop
4. Flush events up to a given timestamp (for frame correlation)
5. Support multiple device types (keyboard, mouse, gamepad)

**Non-Functional:**
1. **Performance:** Lock-free SPSC, minimal latency
2. **Precision:** Nanosecond-capable timestamps
3. **Testability:** Mock adapters indistinguishable from real adapters
4. **Extensibility:** New input sources (network, AI, replay) addable without modifying existing code
5. **Zero allocation:** Fixed-capacity buffers, no heap in hot path

### Design Goals

| Goal | Implication |
|------|-------------|
| SOLID principles | Single responsibility per adapter, open for extension |
| Swappable I/O | Concept-based design, no inheritance hierarchy |
| Template composition | No virtual functions, zero-cost abstractions |
| Lock-free concurrency | SPSC queues, atomic operations |
| Platform independence | Adapters abstract platform-specific APIs |

---

## Decision

### Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                      Input Adapters                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │  Keyboard   │  │   Mouse     │  │      Cursor         │ │
│  │  Adapter    │  │   Button    │  │      Adapter        │ │
│  │             │  │   Adapter   │  │                     │ │
│  └──────┬──────┘  └──────┬──────┘  └──────────┬──────────┘ │
│         │                │                     │            │
│         ▼                ▼                     ▼            │
│  ┌─────────────────────────────────────────────────────────┐│
│  │              Timestamped Event Buffers                  ││
│  │                    (SPSC Queues)                        ││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
                              │
                              │ flush(timestamp, output_span)
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                       Game Loop                             │
│  ┌─────────────────┐    ┌────────────────────────────────┐ │
│  │ snapshot_cache  │◄───│  Correlate events to frames    │ │
│  │ .find(real_time)│    │  via event timestamps          │ │
│  └─────────────────┘    └────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Adapter Granularity

**Decision:** One adapter per device type.

| Adapter | Event Type | Rationale |
|---------|------------|-----------|
| `keyboard_adapter` | `key_event` | Discrete press/repeat/release |
| `mouse_button_adapter` | `mouse_button_event` | Discrete press/release |
| `cursor_adapter` | `cursor_event` | Absolute position stream |
| `raw_motion_adapter` | `raw_motion_event` | Relative delta stream |
| `scroll_adapter` | `scroll_event` | Scroll wheel offsets |

**Rationale:**
- Single responsibility per adapter
- Different data characteristics (discrete vs continuous)
- Different platform APIs (cursor vs raw motion are separate modes)
- Consumer picks only what they need

### The `input_adapter` Concept

```cpp
template <typename T, typename TEvent, typename TDuration>
concept input_adapter = requires(T adapter,
                                  TDuration timestamp,
                                  std::span<TEvent> output) {
    typename T::event_type;
    typename T::duration_type;

    requires std::same_as<typename T::event_type, TEvent>;
    requires std::same_as<typename T::duration_type, TDuration>;

    { adapter.flush(timestamp, output) } -> std::same_as<std::size_t>;
    { adapter.empty() } -> std::same_as<bool>;
    { adapter.capacity() } -> std::same_as<std::size_t>;
};
```

**Operations:**

| Operation | Signature | Description |
|-----------|-----------|-------------|
| `flush` | `flush(TDuration, span<TEvent>) → size_t` | Drain events with timestamp ≤ given timestamp into output span. Returns count written. |
| `empty` | `empty() → bool` | Check if any events are buffered. |
| `capacity` | `capacity() → size_t` | Return buffer capacity. Callers can size their buffers accordingly. |

**Design notes:**
- `flush` returns `size_t` (not iterator) for simplicity
- If output span is smaller than pending events, fills span and leaves rest buffered
- No `flush_all` in concept — use `flush(TDuration::max(), output)` if needed
- Construction not constrained by concept (platform-specific parameters)

### Event Structures

#### Keyboard Events

```cpp
enum class key_action : std::uint8_t {
    press,
    repeat,
    release
};

template <typename TDuration>
struct key_event {
    scan_code physical_key;   // Hardware scan code (layout-independent)
    key_code logical_key;     // Virtual key code (layout-dependent)
    key_action action;
    TDuration timestamp;
};
```

**Design decisions:**
- **Both scan code and key code:** Scan code for gameplay (WASD works on any layout), key code for text input and rebinding UI
- **Three-value action enum:** Includes `repeat` for consumers that need it (text input). Consumers can filter repeats if unwanted.
- **Repeats passed through:** Adapter captures raw facts, doesn't filter. Downstream state machine naturally ignores repeats (key already in "held" state).

#### Mouse Button Events

```cpp
template <typename TDuration>
struct mouse_button_event {
    mouse_button button;      // left, right, middle, extra1, extra2, ...
    button_action action;     // press, release (no repeat for buttons)
    TDuration timestamp;
};
```

#### Cursor Position Events

```cpp
template <typename TDuration>
struct cursor_event {
    double x;                 // Window coordinates
    double y;
    TDuration timestamp;
};
```

**Note:** Separate adapter from raw motion. Cursor position is absolute, clamped to window, affected by OS acceleration. Used for UI interaction.

#### Raw Mouse Motion Events

```cpp
template <typename TDuration>
struct raw_motion_event {
    double delta_x;           // Relative motion since last event
    double delta_y;
    TDuration timestamp;
};
```

**Note:** Separate adapter from cursor. Raw motion is relative, unbounded, unaffected by OS acceleration. Used for FPS camera, flight sims.

#### Scroll Events

```cpp
template <typename TDuration>
struct scroll_event {
    double offset_x;          // Horizontal scroll
    double offset_y;          // Vertical scroll
    TDuration timestamp;
};
```

**Note:** Floating-point for smooth scrolling and touchpad gestures.

### Timestamp Design

#### Source

**Decision:** Timestamp source is an implementation detail, not part of the concept.

| Adapter Type | Timestamp Source |
|--------------|------------------|
| Real (GLFW) | Platform timestamps (most accurate) |
| Real (other) | Best available platform source |
| Mock | Explicitly injected with events |
| Replay | Recorded timestamps from file |

**Rationale:**
- Real adapters should use most accurate available source
- Mock adapters don't need a clock — timestamps are test data
- No "ignored parameter" API smell

#### Precision

**Decision:** Store as `TDuration` (floating-point duration from `jage::time`).

- `jage::time::nanoseconds` = `std::chrono::duration<double, std::nano>`
- Platform providing microseconds → stored as nanoseconds (no precision loss)
- Platform providing nanoseconds → stored directly
- ~15-16 significant digits covers >100 days before sub-microsecond degradation

#### Epoch Normalization

**Decision:** Shared epoch passed at adapter construction.

**Problem:** Different platforms have different epoch (T=0):
- `glfwGetTime()` → seconds since `glfwInit()`
- `steady_clock` → arbitrary (often system boot)
- Future network source → server's epoch

**Solution:**
1. At input system initialization, capture platform time as epoch
2. Pass epoch to each adapter at construction
3. Adapters normalize: `TDuration{platform_time - epoch}`

**Result:** All adapters output timestamps relative to common game epoch, directly usable with `snapshot_cache.find(real_time)`.

**Note:** Epoch handling is implementation detail, not in concept. Different platforms may need different epoch types.

### Buffer Strategy

#### Threading Model

**Decision:** SPSC (single producer, single consumer).

- Producer: Platform thread (or callback context) pushes events
- Consumer: Game thread flushes events
- Existing `jage::containers::spsc::queue` fits this model

#### Capacity

**Decision:** Generous fixed-capacity buffers.

| Event Type | Frequency | Recommended Capacity |
|------------|-----------|---------------------|
| Keyboard (press/release) | Low (~20/sec) | 64 |
| Keyboard (with repeats) | Medium (~30 Hz per held key) | 64 |
| Mouse buttons | Low | 32 |
| Cursor position | High (up to 1000 Hz) | 256 |
| Raw motion | High (up to 1000 Hz) | 256 |
| Scroll | Burst | 32 |

**Rationale:**
- At 60 FPS, 16ms per frame
- 1000 Hz mouse = ~17 events/frame
- 256 capacity = ~15 frames of buffer margin
- Handles frame hitches without overflow

#### Overflow Handling

**Decision:** Buffer overwrites oldest (circular buffer semantics).

**Future extension:** Coalescing for continuous events (motion, cursor). Multiple deltas can be summed, multiple positions can use latest. Localizes change to adapter internals without affecting concept.

### Integration Point

#### Ownership

**Decision:** User or input controller owns adapters (not constrained by framework).

**Option A: Input controller owns adapters**
```cpp
input_controller<TKeyboard, TMouse, ...> {
    TKeyboard keyboard_;
    TMouse mouse_;

    void poll(TDuration timestamp) {
        keyboard_.flush(timestamp, kbd_buffer_);
        mouse_.flush(timestamp, mouse_buffer_);
    }
};

game<TWindow, TInputController> {
    TInputController input_;
};
```

**Option B: User composes manually**
```cpp
glfw_keyboard_adapter kbd{window, epoch};
glfw_mouse_adapter mouse{window, epoch};

while (running) {
    auto t = clock.real_time();
    kbd.flush(t, kbd_buffer);
    mouse.flush(t, mouse_buffer);
    // process...
}
```

Both are valid. Framework doesn't constrain this choice.

#### Flush Timing

**Decision:** Flush at start of frame.

```
┌─────────────────────────────────────────┐
│ Frame N                                 │
│ ┌─────────┐ ┌─────────┐ ┌────────────┐ │
│ │  Flush  │→│ Process │→│   Render   │ │
│ │  Input  │ │  Logic  │ │            │ │
│ └─────────┘ └─────────┘ └────────────┘ │
└─────────────────────────────────────────┘
```

Input available for entire frame's logic. Clear temporal boundary.

#### Event Routing

**Decision:** Direct processing (flush into span, iterate, process).

```cpp
auto count = kbd.flush(frame_time, buffer);
for (auto i = 0u; i < count; ++i) {
    handle_key_event(buffer[i]);
}
```

No event queue indirection. Simple and efficient.

#### Aggregation

**Decision:** Minimal — no required aggregation abstraction.

Users compose adapters as needed. Aggregation utilities can be added later if patterns emerge.

---

## Alternatives Considered

### 1. Single Unified Adapter vs Per-Device Adapters

**Alternative:** One adapter handling all input types with variant events.

```cpp
using input_event = std::variant<key_event, mouse_button_event, cursor_event, ...>;
input_adapter<input_event> adapter;
```

**Rejected because:**
- Violates single responsibility
- Runtime dispatch overhead
- Consumer pays for event types they don't use
- Different enable/disable semantics per device type

### 2. Clock Injection vs Implementation Detail

**Alternative:** Inject clock into adapter concept.

```cpp
template <typename T, typename TEvent, typename TDuration, typename TClock>
concept input_adapter = ...;
```

**Rejected because:**
- Real adapters should use platform timestamps (most accurate)
- Injecting clock that gets ignored is bad API design
- Mock adapters don't need clock — they inject timestamps directly
- Testing real adapters → mock the platform, not the clock

### 3. Iterator Return vs Size Return

**Alternative:** `flush` returns past-the-end iterator instead of count.

```cpp
{ adapter.flush(timestamp, output) } -> std::same_as<typename std::span<TEvent>::iterator>;
```

**Rejected because:**
- Count is more intuitive ("flushed 12 events")
- `count == buffer.size()` cleaner for "need to flush again" check
- Conversion between count and iterator is trivial if needed

### 4. `flush_all` in Concept

**Alternative:** Require `flush_all()` alongside `flush(timestamp)`.

**Rejected because:**
- `flush_all` can race with producer — "all" is ambiguous
- `flush(TDuration::max())` achieves same result explicitly
- Keeps concept minimal
- Can add as free function later if needed

### 5. Filter Repeats at Adapter Level

**Alternative:** Adapter filters repeat events, only passes press/release.

**Rejected because:**
- Adapter should capture raw facts, not make policy decisions
- Some consumers need repeats (text input)
- Downstream state machine naturally handles repeats (no state change)
- Consumer can easily filter: `if (e.action == repeat) continue;`

### 6. Combined Cursor + Raw Motion Adapter

**Alternative:** Single mouse adapter with both absolute and relative events.

**Rejected because:**
- Different platform APIs (separate callbacks/modes in GLFW)
- Different use cases (UI vs FPS camera)
- Different enable/disable semantics (raw motion must be explicitly enabled)
- Violates single responsibility

---

## Consequences

### Positive

1. **Testability:** Mock adapters satisfy same concept, indistinguishable from real
2. **Extensibility:** New input sources (network, AI, replay) implement concept without modifying existing code
3. **Performance:** Lock-free SPSC, zero allocation, minimal overhead
4. **Precision:** Nanosecond-capable timestamps, platform-optimal sources
5. **Flexibility:** User composes adapters as needed, no framework constraints
6. **Simplicity:** Minimal concept surface, clear responsibilities

### Negative

1. **Multiple adapters:** User must create/manage several adapters (keyboard, mouse, etc.)
2. **Platform-specific constructors:** Each real adapter has different construction signature
3. **Epoch coordination:** User must capture and pass shared epoch at initialization

### Neutral

1. **No aggregation abstraction:** Users compose manually (flexibility vs convenience tradeoff)
2. **Generous buffer sizes:** Uses more memory but provides safety margin

---

## Future Considerations

### Additional Adapters

| Adapter | Notes |
|---------|-------|
| Gamepad | Buttons + axes + triggers. May warrant separate button/axis adapters. |
| Touch | Multi-touch with touch IDs, pressure, etc. |
| Network | Events from remote players. Different epoch considerations. |
| AI | Synthetic input from game AI. |
| Replay | Playback from recorded file. |

### Coalescing Extension

For high-frequency continuous events, add optional coalescing:

```cpp
// Multiple motion events between discrete events get summed
motion(+1,+1) → motion(+2,+1) → button_press → motion(+1,0)
// Becomes:
motion(+3,+2) → button_press → motion(+1,0)
```

Localized to adapter internals, doesn't change concept.

### Action Mapping Layer

Higher-level abstraction mapping raw input to semantic actions:

```
key_event{scan_code::W, press} → action{move_forward, pressed}
```

Separate from adapter layer — adapters produce raw events, action mapper interprets.

### Input Recording/Playback

Record timestamped events for:
- Debugging (replay bug scenarios)
- Automated testing
- Game replays

Adapters produce events with precise timestamps, enabling accurate recording.

---

## Open Questions

### Enum Definitions

Need to define:
- `scan_code` — Platform-independent physical key codes
- `key_code` — Logical/virtual key codes
- `mouse_button` — Button identifiers
- `button_action` / `key_action` — Press/release/repeat

Options:
1. Define JAGE-specific enums, map from platform values
2. Use platform values directly (ties to specific platform)
3. Use existing standard (USB HID scan codes)

### Platform Handle Sharing

Multiple adapters may need same platform handle (GLFW window). Options:
1. Pass handle to each adapter constructor
2. Adapters share reference to common platform context
3. Platform context owns adapters

---

## References

- **Snapshot Cache:** ADR-0001, `include/jage/time/snapshot_cache.hpp`
- **Time System:** `include/jage/time/`
- **SPSC Queue:** `include/jage/containers/spsc/queue.hpp`
- **Double Buffer:** `include/jage/concurrency/double_buffer.hpp`

---

## Revision History

| Date       | Author       | Changes                                      |
|------------|--------------|----------------------------------------------|
| 2026-02-04 | Justin Lye   | Initial ADR from design discussion           |
