# ADR-0002: Snapshot Cache Implementation Refinements

**Status:** Accepted
**Date:** 2026-02-02
**Authors:** Justin Lye, Claude (Sonnet 4.5)
**Amends:** [ADR-0001](0001-snapshot-cache-design.md)

---

## Context

During implementation of `snapshot_cache` (per ADR-0001), two design assumptions proved incorrect:

1. ADR-0001 specified `ahead` as a valid status for both `find(timestamp)` and `find(frame_index)`. Implementation revealed that timestamps fundamentally cannot distinguish "ahead of cache" from "matched in current frame".

2. ADR-0001 implicitly assumed `find(frame_index)` would use the same backward linear search as `find(timestamp)`. The frame == push order contract enables O(1) direct lookup instead.

---

## Decision

### 1. `ahead` is only meaningful for `find(frame_index)`

**ADR-0001 specified:**
```cpp
enum class cache_match_status : std::uint8_t {
    matched, ahead, evicted
};
```
Both `find` overloads were expected to return all three statuses.

**Refined:**
- `find(timestamp)` returns `matched` or `evicted` only
- `find(frame_index)` returns `matched`, `ahead`, or `evicted`

**Why `find(timestamp)` cannot return `ahead`:**

Snapshot[N] is captured at frame start. Any event occurring after Snapshot[N] but before Snapshot[N+1] has a timestamp greater than Snapshot[N].real_time. The backward search matches the first snapshot where `event_time >= snap.real_time`. When the event is in the current frame, this condition is satisfied by the newest snapshot on the first iteration — returning `matched`.

Returning `ahead` would require knowledge of Snapshot[N+1], which does not exist yet. Given only a timestamp, there is no way to distinguish "event matched to most recent snapshot" from "event in current frame ahead of cache".

**Why `find(frame_index)` can return `ahead`:**

Frame indices are discrete and monotonic. If the query requests frame 100 and the newest cached frame is 99, that is an unambiguous factual comparison. No future knowledge is required.

---

### 2. `find(frame_index)` uses O(1) direct lookup

**ADR-0001 assumed** both overloads would use backward linear search with modulo.

**Refined:** `find(frame_index)` performs boundary checks, then accesses the slot directly:

```cpp
[[nodiscard]] auto find(std::uint64_t frame_index)
    -> std::pair<TSnapshot, cache_match_status>;
```

**Contract (frame == push order):**

Frame indices are sequential starting from 0. Frame N is pushed as the Nth snapshot, landing at `buffer_[N % Capacity]`. Boundary checks for `ahead` and `evicted` are performed before the direct access.

**Trade-offs:**

| Aspect | Linear search | O(1) direct lookup |
|--------|--------------|-------------------|
| Complexity | O(Capacity) | O(1) |
| Branches | Capacity iterations | 2 boundary checks |
| Contract | None beyond cache_snapshot | Frame indices sequential from 0 |
| Robustness | Tolerates non-sequential frames | Breaks silently on skipped frames |

**Why O(1) was chosen:**

- Fewer branches in the hot path improves branch prediction
- The sequential frame index contract is already implied by the warm-up assumption in ADR-0001 (cache filled before queries)
- If frames are ever skipped, boundary checks (`ahead`/`evicted`) will catch out-of-range requests; only in-range non-sequential frames would silently mismatch

---

## Consequences

### Positive

1. **Clearer API contract:** Each `find` overload has well-defined status semantics
2. **Performance:** `find(frame_index)` is O(1) vs O(Capacity) linear search
3. **Honest semantics:** `ahead` is only returned when it can be factually determined

### Negative

1. **Asymmetric status semantics:** Callers must know which statuses each overload can return
2. **Implicit contract:** `find(frame_index)` correctness depends on sequential frame indices from 0

### Neutral

1. **`cache_match_status::ahead` exists in the enum but is never returned by `find(timestamp)`** — the enum is shared, and `ahead` remains valid for `find(frame_index)`

---

## References

- **Amends:** [ADR-0001: Snapshot Cache Design](0001-snapshot-cache-design.md)
- **Implementation:** `include/jage/time/internal/snapshot_cache.hpp`
- **Public API:** `include/jage/time/snapshot_cache.hpp`

---

## Revision History

| Date       | Author       | Changes                                      |
|------------|--------------|----------------------------------------------|
| 2026-02-02 | Justin Lye   | Initial ADR, amending ADR-0001               |
