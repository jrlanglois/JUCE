# JUCE Box2D Integration

`juce_box2d` vendors Box2D and exposes its C API together with `juce::Box2DRenderer`. The upstream `b2*` API remains in the global namespace for C++ consumers; `juce::Box2DRenderer` and `juce::handleBox2DAssertion` are the JUCE additions. This document records the integration choices, local changes, supported configuration, and verification baseline so that the constraints do not need to be reconstructed from the generated projects or the vendored source.

For the Box2D API itself, use the [upstream documentation](https://box2d.org/documentation/).

## Why the Previous Fork Was Replaced

JUCE previously carried a Box2D 2.2.1 fork and exposed its C++ API. Box2D 3 is a ground-up rewrite with a C API, as described in the [upstream migration guide](https://github.com/erincatto/box2d/blob/956ce4e1e8acddd05a21102629e268ff9321ec50/docs/migration.md). This module therefore exposes Box2D 3 directly instead of maintaining a second compatibility API shaped like Box2D 2.

The vendored licence changes from Zlib to MIT. JUCE-authored module code remains available under JUCE's AGPLv3 or commercial terms.

## Upstream Pin and Refresh

The payload's public headers report Box2D 3.2.0. It is an untagged development snapshot whose nearest-tag description is `v3.1.1-55-g956ce4e`, so the integration pins the commit rather than naming it as a release. [`box2d/JUCE_UPSTREAM.txt`](box2d/JUCE_UPSTREAM.txt) is the provenance authority.

[`box2d/vendor.py`](box2d/vendor.py) is the only supported refresh path. Run it from the JUCE repository root:

```bash
python modules/juce_box2d/box2d/vendor.py
```

The importer clones the pinned commit, replaces the payload, reapplies the JUCE transformations, and fails if an expected source pattern or integration invariant has changed. A successful refresh still requires the builds and tests below.

## Compilation Model

Box2D is compiled as C in the module-root [`juce_box2d_upstream.c`](juce_box2d_upstream.c), while [`juce_box2d.cpp`](juce_box2d.cpp) contains only JUCE-authored C++. The separate units keep Box2D's C dialect, compiler pragmas, compatibility definitions, and warning suppressions out of JUCE C++ code.

The vendored files contain no `#include` directives. Public headers, private headers, and C sources are flattened into an explicit dependency order owned by the module header and C unit. This avoids consumer header-search paths and makes the integration behave like JUCE's other bundled C libraries. The importer verifies the public and private order, source order, macro leakage, cross-file static-name collisions, and required `#undef` boundaries.

The upstream build requests C17. The payload used here also compiles in the default MSVC C mode with the local `_Alignas` and `_Static_assert` compatibility definitions, and in strict C11 as emitted by Projucer's Xcode exporters.

CMake discovers the module-root C unit automatically. Projucer projects must be re-saved after adopting this version so that they gain an `include_juce_box2d_upstream.c` proxy. The module adds no framework, library, or include-path requirement.

## Changes to Vendored Files

The importer makes only these transformations:

- Removes vendored includes and moves their dependency order into JUCE's module header and C unit.
- Converts `recording_ops.inl` into the `B2_REC_MANIFEST` continuation macro required by the flattened unit.
- Changes `b2DefaultAssertFcn` in `core.c` to call the JUCE-authored `handleBox2DAssertion` hook.
- Uses the existing `posix_memalign` allocation branch on macOS and iOS because JUCE's supported deployment targets predate the guaranteed availability of `aligned_alloc`.

No Box2D algorithms or public declarations are otherwise changed. Refresh review should compare the imported payload with the pinned upstream commit and confirm that this list remains complete.

## Toolchain Baselines

The MSVC baseline for this module is Visual Studio 2022 17.14 or newer. Visual Studio 2019 remains a JUCE-wide exporter option, and Projucer still generates those projects, but `juce_box2d` does not support or verify that toolchain.

This module-specific floor does not require a paid Visual Studio edition for individual or open-source development: Visual Studio Community is available at no cost for those uses, and Microsoft's C++ standard library implementation is open source. The supported floor is the oldest MSVC toolset on which this payload was verified, rather than an estimate of which older compiler might accept it.

Clang-cl compiles the payload in its default C mode and in strict C11. Projucer's Xcode exporters select strict C11. Linux GCC and Clang, Apple SDK builds, Android, and non-x64 Windows architectures remain separate verification targets because they exercise different platform headers, SIMD paths, and floating-point pragma implementations.

## Configuration

These upstream options retain their Box2D meanings:

- `BOX2D_USER_CONFIG` selects an application-provided configuration header.
- `BOX2D_DOUBLE_PRECISION` changes public types and the ABI.
- `BOX2D_DISABLE_SIMD` disables Box2D's SIMD implementations.
- `BOX2D_VALIDATE` enables additional internal validation.
- `BOX2D_EXPORT` controls symbol visibility.
- `B2_ENABLE_ASSERT` enables Box2D assertions independently of JUCE's debug configuration.

Every option that affects declarations or ABI must have the same value in the C and C++ units. Set these as project- or target-wide definitions, not on one source file.

`BOX2D_PROFILE` requires Tracy's C headers on the include path and the Tracy client linked by the application. This integration does not exercise that option.

`BOX2D_AVX2` is unsupported because it requires AVX2 code generation for the complete target rather than only the Box2D C unit.

## Runtime Caveats

Box2D's default assertion handler forwards the original condition, file, and line to JUCE's assertion reporting and returns zero. This matches the continue-after-reporting behaviour of `jassert`; after an internal invariant fails, Box2D may no longer be in a usable state. Applications may replace the process-wide handler with `b2SetAssertFcn`, and can reinstall `juce::handleBox2DAssertion` explicitly.

`b2DefaultWorldDef()` is serial. A worker count above one without application task callbacks selects Box2D's built-in scheduler on supported native-thread platforms. On platforms using Box2D's synchronous fallback, including the BSDs, that configuration can hang; keep the worker count at one or supply task callbacks.

Box2D's world table, allocator, assertion handler, log handler, and length-unit setting are process-global. Separate plug-in instances in one process therefore share those limits and hooks.

World creation reports that deterministic simulation is unsupported when the calling thread flushes denormals, including inside `ScopedNoDenormals`.

The C unit disables floating-point contraction to match upstream's deterministic build settings. Public inline functions compile under the consuming target's floating-point settings.

`Box2DRenderer::render()` requires a valid, unlocked world, positive view dimensions, and a non-empty target area. It must not run while the world is stepping or concurrently through the same renderer. The renderer clips to the target area and restores the caller's graphics state.

## Warning Policy

Suppressions begin immediately before the vendored headers and sources and end immediately after them. JUCE-authored code is outside these scopes.

The C unit suppresses only diagnostics observed in the pinned payload: C4201 under MSVC; C4189 only while compiling `broad_phase.c` for ARM64EC; and sign conversion, switch-enum coverage, language-extension tokens, implicit integer conversion, implicit integer-to-float conversion, floating-point equality, and cast alignment under GCC-compatible warning controls. New warnings after a refresh must be investigated before extending the list.

## Tests and Demo

Link `juce_box2d` into UnitTestRunner and run:

```bash
UnitTestRunner --category=Box2D
```

The category covers fixed-step falling-body behaviour, serial determinism, built-in-scheduler determinism on native-thread platforms, renderer clipping and graphics-state restoration, and far-origin rendering when `BOX2D_DOUBLE_PRECISION` is enabled.

DemoRunner contains clean-room Box2D 3 scenes for Add Pair Stress Test, Apply Force, Dominoes, and Chain. The scenes use the C API, four simulation substeps, one RAII-owned world, and a persistent renderer. Apply Force uses motor joints for top-down friction and responds to `a`, `w`, and `d`.

The scene list exposes each scene name to accessibility clients. The render view exposes the active scene as its title, sends a title-changed event when selection changes, and no longer steals keyboard focus on every timer tick.

## Verification Status

Verified on Windows for the pinned commit:

- The importer completes and a second import reproduces the payload.
- An isolated C++17 consumer compiles with only `#include <juce_box2d/juce_box2d.h>`.
- The payload compiles with MSVC 19.44 and 19.51 in their default C modes.
- CMake Debug and Release builds of UnitTestRunner and DemoRunner complete without warnings under MSVC 19.51 and clang-cl 22.1.3.
- `UnitTestRunner --category=Box2D` passes in each of those configurations, including the native scheduler test.
- `BOX2D_DOUBLE_PRECISION`, `BOX2D_DISABLE_SIMD`, `BOX2D_VALIDATE`, and a custom `BOX2D_USER_CONFIG` build without warnings in Debug and pass the Box2D tests under MSVC 19.51. `B2_ENABLE_ASSERT` does the same in Release. Each definition is applied to both the C and C++ units.
- The generated Visual Studio 2022 solutions build UnitTestRunner and DemoRunner without warnings in Debug with the v143 14.44 toolset, and the generated Visual Studio 2026 solutions do the same with the v145 14.51 toolset. The Box2D tests pass through both generated UnitTestRunner projects.
- UnitTestRunner compiles without warnings for Windows Arm64 and ARM64EC. These targets were not executed on the x64 host.
- An MSVC AddressSanitizer Debug build passes the Box2D tests without a sanitizer report. It uses `/fsanitize=address` for both languages, `/DEBUG /INCREMENTAL:NO` at link, and the matching MSVC AddressSanitizer runtime on `PATH`.
- DemoRunner opens every Box2D scene, accepts the Apply Force keys, survives repeated scene changes and resizes, and exposes the scene rows and current title through Windows UI Automation.

Additional matrix result:

- The generated Android Debug APK builds for `armeabi-v7a`, `x86`, `arm64-v8a`, and `x86_64`. The installed command-line tools write a newer SDK XML schema than Android Gradle Plugin 8.13.2 understands, so Gradle reports CXX5304 while completing the build; a warning-free Android baseline remains unverified.

Unavailable or incomplete verification:

- Full Linux GCC and Clang builds were unavailable because the WSL image lacks the GNU C++ compiler, `pkg-config`, and the JUCE Linux development packages. GCC Arm64 determinism was also unavailable because the host has no Arm64 cross-compiler or emulator.
- macOS and iOS require macOS and Xcode and were unavailable on this Windows host.
- Android Box2D tests were not run because this checkout has no Android UnitTestRunner target or connected-device test harness.
- Spoken output in Narrator or another screen reader; Windows UI Automation exposes the intended names and title.

## Migration

The Box2D 2.x C++ API and the previous renderer overload have been removed without compatibility aliases. See the [`develop` entry in `BREAKING_CHANGES.md`](../../BREAKING_CHANGES.md#develop) for source migration steps.
