# ADR-0003: Input Adapter Design

**Status:** Partially Implemented (Keyboard adapter callback phase complete, flush/buffering pending)
**Date:** 2026-02-04
**Updated:** 2026-02-14
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

| Adapter | Event Type | Implementation Status |
|---------|------------|---------------------|
| `keyboard_adapter` | `keyboard::event` | ✓ Implemented (callback registration only) |
| `mouse_button_adapter` | `mouse_button_event` | Not yet implemented |
| `cursor_adapter` | `cursor_event` | Not yet implemented |
| `raw_motion_adapter` | `raw_motion_event` | Not yet implemented |
| `scroll_adapter` | `scroll_event` | Not yet implemented |

**Rationale:**
- Single responsibility per adapter
- Different data characteristics (discrete vs continuous)
- Different platform APIs (cursor vs raw motion are separate modes)
- Consumer picks only what they need

**Current Implementation Note:**
The GLFW keyboard adapter (`jage::input::adapters::glfw<TPlatform>`) currently implements only the callback registration phase. It does not yet implement the full `input_adapter` concept (no `flush()`, `empty()`, or `capacity()` methods). The adapter:
1. Registers a GLFW key callback via platform abstraction
2. Maps GLFW key codes to `keyboard::key` (logical keys)
3. Maps OS scancodes to `keyboard::scancode` (physical keys)
4. Converts GLFW modifier flags to modifier bitset
5. Timestamps events using platform time source with epoch normalization
6. Pushes events to platform-abstracted context

The context abstraction (accessed via `TPlatform::context_type`) provides a `push()` method for event buffering. The adapter retrieves the context from the window's user pointer, enabling multiple windows to have independent event streams.

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
enum class action : std::uint8_t {
    press,
    repeat,
    release
};

enum class modifier {
    left_shift,
    right_shift,
    left_control,
    right_control,
    left_alt,
    right_alt,
    left_gui,
    right_gui,
};

constexpr std::size_t modifier_count = 8;

template <typename TDuration>
struct event {
    TDuration timestamp;
    key key;                          // Logical key (layout-dependent, e.g., key::a)
    scancode scancode;                // Physical key (layout-independent, hardware scancode)
    action action;
    std::bitset<modifier_count> modifiers;  // Active modifier keys
};
```

**Design decisions:**
- **Both scan code and key code:** Scan code for gameplay (WASD works on any layout), key code for text input and rebinding UI
- **Field naming:** Uses `key` and `scancode` instead of `logical_key` and `physical_key` for brevity
- **Timestamp first:** Places timestamp as first field for consistency with other event types
- **Three-value action enum:** Includes `repeat` for consumers that need it (text input). Consumers can filter repeats if unwanted.
- **Repeats passed through:** Adapter captures raw facts, doesn't filter. Downstream state machine naturally ignores repeats (key already in "held" state).
- **Modifier tracking:** Bitset tracks all active modifier keys at event time. GLFW doesn't distinguish left/right modifiers, so both bits are set for aggregated modifiers (e.g., GLFW_MOD_SHIFT sets both left_shift and right_shift bits).

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
key_event{scancode::W, press} → action{move_forward, pressed}
```

Separate from adapter layer — adapters produce raw events, action mapper interprets.

### Input Recording/Playback

Record timestamped events for:
- Debugging (replay bug scenarios)
- Automated testing
- Game replays

Adapters produce events with precise timestamps, enabling accurate recording.

---

## Deviations from Original Design

The actual implementation differs from the initial ADR in several ways:

### Event Structure Changes

1. **Field Order:** Events place `timestamp` first for consistency across event types
2. **Field Names:** Uses `key` and `scancode` instead of `logical_key` and `physical_key`
3. **Modifier Addition:** Added `std::bitset<modifier_count> modifiers` field not in original ADR
   - Rationale: Essential for handling keyboard shortcuts, text input, and UI interactions
   - GLFW provides aggregate modifier flags (can't distinguish left/right), so both bits are set

### Adapter Architecture

1. **Platform Abstraction Template:** Uses `TPlatform` template parameter for testability
   - Original ADR showed direct GLFW calls
   - Actual implementation abstracts all platform operations through `TPlatform` interface
   - Enables testing with mock platforms without preprocessor gymnastics

2. **Context-Based Buffering:** Events pushed to `TPlatform::context_type` rather than internal SPSC queue
   - Separates concerns: adapter handles translation, context handles buffering
   - Allows different contexts for different windows
   - Context retrieved from window user pointer set by platform

3. **Phased Implementation:** Callback registration phase complete, `input_adapter` concept phase pending
   - Current: Adapter sets up callbacks and translates events
   - Next: Implement `flush()`, `empty()`, `capacity()` to satisfy concept
   - Allows testing translation logic before buffering strategy finalized

### Type Naming

1. **Namespace Organization:** Types live in `jage::input::keyboard` namespace
   - `keyboard::event<TDuration>` instead of bare `key_event<TDuration>`
   - `keyboard::action` instead of `key_action`
   - `keyboard::key` instead of `key_code`
   - More consistent with C++ naming conventions

## Implementation Status

### Completed

1. **Keyboard event structure** (`include/jage/input/keyboard/event.hpp`)
   - Timestamp, key, scancode, action, modifiers fields
   - Template-based duration type

2. **Keyboard enums**
   - `keyboard::key` - Logical/virtual key codes (✓ Implemented)
   - `keyboard::scancode` - Physical key codes (✓ Implemented)
   - `keyboard::action` - Press/repeat/release (✓ Implemented)
   - `keyboard::modifier` - Modifier keys enum (✓ Implemented)

3. **GLFW keyboard adapter** (`include/jage/input/adapters/glfw.hpp`)
   - Platform abstraction via template parameter
   - Callback registration and key mapping
   - Scancode and logical key translation
   - Modifier state tracking
   - Epoch normalization
   - **Note:** Does not yet implement `input_adapter` concept

4. **Test infrastructure**
   - Mock platform for testing (`test/lib/include/jage/test/fakes/input/platforms/glfw.hpp`)
   - Mock context for event buffering (`test/lib/include/jage/test/fakes/input/context/glfw.hpp`)
   - Comprehensive adapter tests (`test/unit/jage/input/adapters/glfw_test.cpp`)

### Remaining Work

1. **Complete `input_adapter` concept implementation**
   - Add `flush(timestamp, output_span)` method
   - Add `empty()` method
   - Add `capacity()` method
   - Integrate with SPSC queue for thread-safe buffering

2. **Mouse/cursor adapters**
   - Mouse button adapter
   - Cursor position adapter
   - Raw motion adapter
   - Scroll adapter

3. **Integration with input controller**
   - Polling mechanism
   - Event routing to monitors

## Open Questions

### Mouse Event Structures

Need to define event structures for:
- `mouse_button` — Button identifiers
- Mouse button events
- Cursor position events
- Raw motion events
- Scroll events

### Platform Handle Sharing

**Decision:** Pass platform handle and abstraction to adapter constructor.

Multiple adapters may need same platform handle (GLFW window). The GLFW adapter implementation uses:
1. Template parameter `TPlatform` for platform abstraction
2. Constructor receives window handle and platform reference
3. Platform abstraction provides testable interface for GLFW operations

**Implementation (GLFW Keyboard Adapter):**
```cpp
template <class TPlatform>
class glfw {
  glfw(typename TPlatform::window_handler_pointer_type window,
       TPlatform &platform,
       const typename TPlatform::context_type::duration_type &epoch) {
    platform.set_seconds_since_init(time::cast<time::seconds>(epoch));
    load_logical_keys();
    load_physical_scancodes(platform);
    platform.set_key_callback(window, key_callback);
  }
};
```

**Platform Abstraction Requirements:**
- `window_handler_pointer_type` - Type for window handle (e.g., `GLFWwindow*`)
- `context_type` - Event context/buffer type
- `get_window_user_pointer(window)` - Retrieve associated context
- `get_seconds_since_init()` - Platform time source
- `set_seconds_since_init(duration)` - Set epoch for timestamp normalization
- `get_key_scancode(key)` - Map platform key to OS scancode
- `set_key_callback(window, callback)` - Register input callback

This abstraction enables testing with mock platforms while maintaining zero-cost for production use.

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
| 2026-02-14 | Justin Lye   | Updated to reflect actual GLFW adapter implementation: added modifier tracking, corrected event structure field names and order, documented platform abstraction pattern, added implementation status section |
