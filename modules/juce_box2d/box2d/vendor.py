#!/usr/bin/env python3
"""Vendors Box2D into the directory containing this script.

Run this script with no options to replace everything under box2d/ except
itself with the pinned upstream snapshot, strip vendored includes, apply JUCE
transformations, and validate the hand-written juce_box2d_upstream unit. Needs
git and Python 3.10+.
"""

from __future__ import annotations

import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

REPO = "https://github.com/erincatto/box2d.git"
COMMIT = "956ce4e1e8acddd05a21102629e268ff9321ec50"
DESCRIBED_VERSION = "v3.1.1-55-g956ce4e"

MODULE_PREFIX = "juce_box2d/box2d"
MODULE_ROOT = Path("modules/juce_box2d")
UPSTREAM_C = MODULE_ROOT / "juce_box2d_upstream.c"
UPSTREAM_H = MODULE_ROOT / "juce_box2d_upstream.h"

INCLUDE_RE = re.compile(r"^\s*#\s*include\s+(\"([^\"]+)\"|<([^>]+)>)", re.M)
MACRO_INCLUDE_RE = re.compile(
    r"#ifdef BOX2D_USER_CONFIG\s*\r?\n#include BOX2D_USER_CONFIG\s*\r?\n#endif\s*\r?\n",
    re.M,
)
REC_OP_RE = re.compile(r"^\s*B2_REC_OP\s*\(", re.M)
MANIFEST_OP_RE = re.compile(r"^\s{4}B2_REC_OP\s*\(", re.M)
STATIC_DEF_RE = re.compile(
    r"^\s*static\s+(?:inline\s+)?(?:const\s+)?(?:unsigned\s+)?(?:signed\s+)?"
    r"(?:\w[\w\s\*]*?)\s+(\w+)\s*[\(\[=;]",
    re.M,
)
MACRO_DEF_RE = re.compile(r"^\s*#\s*define\s+(\w+)", re.M)
UNDEF_RE = re.compile(r"^\s*#\s*undef\s+(\w+)", re.M)

BOX2D_SOURCE_FILES = [
    "aabb.c",
    "aabb.h",
    "arena_allocator.c",
    "arena_allocator.h",
    "bitset.c",
    "bitset.h",
    "body.c",
    "body.h",
    "broad_phase.c",
    "broad_phase.h",
    "constraint_graph.c",
    "constraint_graph.h",
    "contact_solver.c",
    "contact_solver.h",
    "contact.c",
    "contact.h",
    "container.h",
    "core.c",
    "core.h",
    "distance_joint.c",
    "distance.c",
    "dynamic_tree.c",
    "dynamic_tree.h",
    "geometry.c",
    "hull.c",
    "id_pool.c",
    "id_pool.h",
    "island.c",
    "island.h",
    "joint.c",
    "joint.h",
    "manifold.c",
    "math_functions.c",
    "motor_joint.c",
    "mover_joint.c",
    "mover.c",
    "parallel_for.c",
    "parallel_for.h",
    "physics_world.c",
    "physics_world.h",
    "platform.h",
    "pogo_joint.c",
    "prismatic_joint.c",
    "qsort.h",
    "recording_ops.inl",
    "recording.c",
    "recording.h",
    "replay.c",
    "replay.h",
    "revolute_joint.c",
    "scheduler.c",
    "scheduler.h",
    "sensor.c",
    "sensor.h",
    "shape.c",
    "shape.h",
    "simd.h",
    "snapshot.c",
    "snapshot.h",
    "solver_set.c",
    "solver_set.h",
    "solver.c",
    "solver.h",
    "table.c",
    "table.h",
    "timer.c",
    "types.c",
    "weld_joint.c",
    "wheel_joint.c",
]

PUBLIC_API_HEADERS = [
    f"{MODULE_PREFIX}/include/box2d/config.h",
    f"{MODULE_PREFIX}/include/box2d/base.h",
    f"{MODULE_PREFIX}/include/box2d/id.h",
    f"{MODULE_PREFIX}/include/box2d/math_types.h",
    f"{MODULE_PREFIX}/include/box2d/math_functions.h",
    f"{MODULE_PREFIX}/include/box2d/collision.h",
    f"{MODULE_PREFIX}/include/box2d/constants.h",
    f"{MODULE_PREFIX}/include/box2d/types.h",
    f"{MODULE_PREFIX}/include/box2d/box2d.h",
]

PRIVATE_HEADERS = [
    f"{MODULE_PREFIX}/src/aabb.h",
    f"{MODULE_PREFIX}/src/core.h",
    f"{MODULE_PREFIX}/src/container.h",
    f"{MODULE_PREFIX}/src/arena_allocator.h",
    f"{MODULE_PREFIX}/src/bitset.h",
    f"{MODULE_PREFIX}/src/dynamic_tree.h",
    f"{MODULE_PREFIX}/src/table.h",
    f"{MODULE_PREFIX}/src/broad_phase.h",
    f"{MODULE_PREFIX}/src/contact.h",
    f"{MODULE_PREFIX}/src/solver.h",
    f"{MODULE_PREFIX}/src/joint.h",
    f"{MODULE_PREFIX}/src/constraint_graph.h",
    f"{MODULE_PREFIX}/src/id_pool.h",
    f"{MODULE_PREFIX}/src/sensor.h",
    f"{MODULE_PREFIX}/src/shape.h",
    f"{MODULE_PREFIX}/src/island.h",
    f"{MODULE_PREFIX}/src/solver_set.h",
    f"{MODULE_PREFIX}/src/physics_world.h",
    f"{MODULE_PREFIX}/src/body.h",
    f"{MODULE_PREFIX}/src/contact_solver.h",
    f"{MODULE_PREFIX}/src/parallel_for.h",
    f"{MODULE_PREFIX}/src/platform.h",
    f"{MODULE_PREFIX}/src/recording.h",
    f"{MODULE_PREFIX}/src/replay.h",
    f"{MODULE_PREFIX}/src/scheduler.h",
    f"{MODULE_PREFIX}/src/simd.h",
    f"{MODULE_PREFIX}/src/snapshot.h",
]

SOURCE_C_FILES = [
    name
    for name in BOX2D_SOURCE_FILES
    if name.endswith(".c")
]

ASSERT_PATCH_OLD = """static int b2DefaultAssertFcn( const char* condition, const char* fileName, int lineNumber )
{
\tfprintf( stderr, "BOX2D ASSERTION: %s, %s, line %d\\n", condition, fileName, lineNumber );
\tfflush( stderr );

\t// return non-zero to break to debugger
\treturn 1;
}"""

ASSERT_PATCH_NEW = """static int b2DefaultAssertFcn( const char* condition, const char* fileName, int lineNumber )
{
\treturn handleBox2DAssertion( condition, fileName, lineNumber );
}"""

ALLOC_BRANCH_OLD = "#elif defined( B2_PLATFORM_ANDROID )"
ALLOC_BRANCH_NEW = (
    "#elif defined( B2_PLATFORM_ANDROID ) || defined( B2_PLATFORM_MACOS ) "
    "|| defined( B2_PLATFORM_IOS )"
)

UPSTREAM_NOTE = """Upstream: {repo}
Version:  {version}
Commit: {commit}

Vendored by vendor.py in this directory; rerun it to update. It copies the Box2D
library payload (public headers, src, shared determinism helpers, LICENSE, and
box2d.natvis), removes every vendored #include so the module owns include order,
converts recording_ops.inl into the B2_REC_MANIFEST continuation macro,
forwards assertions through handleBox2DAssertion in core.c, and routes Apple
platform allocation through posix_memalign like Android.
"""


def rel_to_py(*paths: str) -> Path:
    return Path(
        os.path.realpath(
            os.path.join(os.path.realpath(os.path.dirname(__file__)), *paths)
        )
    )


def repo_root() -> Path:
    return rel_to_py(".").parents[2]


def git(*args: str) -> str:
    return subprocess.run(
        ["git", *args], check=True, capture_output=True, text=True
    ).stdout.strip()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8", newline="\n")


def fail(message: str) -> None:
    print(f"vendor.py: {message}", file=sys.stderr)
    sys.exit(1)


def copy_payload(tree: Path, dest: Path) -> None:
    include_dir = tree / "include" / "box2d"
    src_dir = tree / "src"
    shared_dir = tree / "shared"

    (dest / "src").mkdir(parents=True, exist_ok=True)
    (dest / "shared").mkdir(parents=True, exist_ok=True)
    (dest / "include" / "box2d").mkdir(parents=True, exist_ok=True)

    shutil.copytree(include_dir, dest / "include" / "box2d", dirs_exist_ok=True)

    for name in BOX2D_SOURCE_FILES:
        source = src_dir / name
        assert source.is_file(), source
        shutil.copy2(source, dest / "src" / name)

    natvis = src_dir / "box2d.natvis"
    assert natvis.is_file()
    shutil.copy2(natvis, dest / "src" / "box2d.natvis")

    for name in ("determinism.c", "determinism.h"):
        shutil.copy2(shared_dir / name, dest / "shared" / name)

    shutil.copy2(tree / "LICENSE", dest / "LICENSE")


def convert_recording_ops(text: str) -> tuple[str, int]:
    if "#define B2_REC_MANIFEST" in text:
        return text, len(REC_OP_RE.findall(text))

    comment_lines: list[str] = []
    op_lines: list[str] = []
    pending: str | None = None
    depth = 0

    for line in text.splitlines():
        stripped = line.strip()
        if pending is None:
            if not stripped:
                continue
            if stripped.startswith("//"):
                comment_lines.append(stripped[2:].strip())
                continue
            if not stripped.startswith("B2_REC_OP"):
                continue
            pending = stripped
            depth = pending.count("(") - pending.count(")")
            if depth == 0:
                op_lines.append(re.sub(r"\s+", " ", pending))
                pending = None
            continue

        pending = f"{pending} {stripped}"
        depth += stripped.count("(") - stripped.count(")")
        if depth == 0:
            op_lines.append(re.sub(r"\s+", " ", pending))
            pending = None

    if pending is not None:
        fail("recording_ops.inl has an unfinished B2_REC_OP entry")

    if not op_lines:
        fail("recording_ops.inl contains no B2_REC_OP entries")

    comment_block = "/*\n" + "\n".join(comment_lines) + "\n*/\n\n" if comment_lines else ""
    macro_body = comment_block + "#define B2_REC_MANIFEST \\\n"
    for index, op in enumerate(op_lines):
        suffix = " \\" if index + 1 < len(op_lines) else ""
        macro_body += f"    {op}{suffix}\n"
    return macro_body, len(op_lines)


def strip_includes(text: str) -> str:
    return INCLUDE_RE.sub("", text)


def strip_user_config_include(base_text: str) -> str:
    stripped, count = MACRO_INCLUDE_RE.subn("", base_text)
    assert count == 1, "BOX2D_USER_CONFIG block in base.h"
    return stripped


def apply_patches(core_text: str) -> str:
    assert core_text.count(ASSERT_PATCH_OLD) == 1, "b2DefaultAssertFcn body"
    core_text = core_text.replace(ASSERT_PATCH_OLD, ASSERT_PATCH_NEW)

    assert core_text.count(ALLOC_BRANCH_OLD) == 1, "b2Alloc Apple branch"
    core_text = core_text.replace(ALLOC_BRANCH_OLD, ALLOC_BRANCH_NEW)

    return core_text


def replace_recording_manifest_includes(text: str) -> str:
    return text.replace('#include "recording_ops.inl"', "B2_REC_MANIFEST")


def collect_angle_includes(payload_root: Path) -> set[str]:
    found: set[str] = set()
    for path in payload_root.rglob("*"):
        if path.suffix not in {".c", ".h", ".inl"}:
            continue
        for match in INCLUDE_RE.finditer(read(path)):
            if match.group(3) is not None:
                found.add(f"<{match.group(3)}>")
    return found


def collect_quoted_includes(payload_root: Path) -> dict[Path, list[str]]:
    mapping: dict[Path, list[str]] = {}
    for path in payload_root.rglob("*"):
        if path.suffix not in {".c", ".h", ".inl"}:
            continue
        quoted = [match.group(2) for match in INCLUDE_RE.finditer(read(path)) if match.group(2)]
        if quoted:
            mapping[path] = quoted
    return mapping


def transform_payload(dest: Path) -> None:
    ops_path = dest / "src" / "recording_ops.inl"
    ops_text = read(ops_path)
    converted, op_count = convert_recording_ops(ops_text)
    write(ops_path, converted)

    for name in ("recording.c", "recording.h", "replay.c"):
        path = dest / "src" / name
        write(path, replace_recording_manifest_includes(read(path)))

    core_path = dest / "src" / "core.c"
    write(core_path, apply_patches(read(core_path)))

    angle_before = collect_angle_includes(dest)
    quoted_before = collect_quoted_includes(dest)
    validate_removed_includes(dest, angle_before, quoted_before)

    for path in list(dest.rglob("*")):
        if path.suffix not in {".c", ".h", ".inl"}:
            continue
        write(path, strip_includes(read(path)))

    base_path = dest / "include" / "box2d" / "base.h"
    write(base_path, strip_user_config_include(read(base_path)))

    for path in dest.rglob("*"):
        if path.suffix not in {".c", ".h", ".inl"}:
            continue
        if INCLUDE_RE.search(read(path)):
            fail(f"leftover #include in {path.relative_to(dest)}")

    converted_ops = read(ops_path)
    converted_count = len(MANIFEST_OP_RE.findall(converted_ops))
    if converted_count != op_count:
        fail("recording_ops.inl row count changed during manifest conversion")


def parse_unit_includes(unit_text: str) -> list[str]:
    includes: list[str] = []
    for line in unit_text.splitlines():
        match = re.match(r'^\s*#\s*include\s+"([^"]+)"', line)
        if match is not None:
            includes.append(match.group(1))
    return includes


def resolve_quoted_include(including: Path, name: str, payload_root: Path) -> Path | None:
    local = including.parent / name
    if local.is_file():
        return local.resolve()
    if name.startswith("box2d/"):
        candidate = payload_root / "include" / name
        if candidate.is_file():
            return candidate.resolve()
    candidate = payload_root / "src" / name
    if candidate.is_file():
        return candidate.resolve()
    candidate = payload_root / "shared" / name
    if candidate.is_file():
        return candidate.resolve()
    return None


def module_path_for(payload_root: Path, path: Path) -> str:
    return f"{MODULE_PREFIX}/{path.relative_to(payload_root).as_posix()}"


def validate_removed_includes(
    payload_root: Path,
    angle_includes: set[str],
    quoted_includes: dict[Path, list[str]],
) -> None:
    root = repo_root()
    unit_text = read(root / UPSTREAM_C)
    hoisted = extract_hoisted_angle_block(unit_text)

    for token in sorted(angle_includes):
        if token not in hoisted and f"#include {token}" not in unit_text:
            fail(
                f"juce_box2d_upstream.c hoisted block does not cover {token} "
                "removed from the payload"
            )

    header_text = read(root / UPSTREAM_H)
    combined_order = parse_unit_includes(header_text) + parse_unit_includes(unit_text)

    for include in combined_order:
        path = root / "modules" / include.replace("/", os.sep)
        if include.startswith(MODULE_PREFIX) and not path.is_file():
            fail(f"upstream unit includes missing file {include}")

    for payload_file, quoted_list in quoted_includes.items():
        rel_from_payload = payload_file.relative_to(payload_root)
        module_path = module_path_for(payload_root, payload_file)
        if module_path not in combined_order:
            fail(f"{rel_from_payload} is not included from juce_box2d_upstream.h or .c")
        position = combined_order.index(module_path)
        allowed = set(combined_order[:position])
        for quoted in quoted_list:
            target = resolve_quoted_include(payload_file, quoted, payload_root)
            if target is None:
                fail(
                    f"{rel_from_payload} quoted include {quoted!r} does not resolve in payload"
                )
            target_module = module_path_for(payload_root, target)
            if target_module not in allowed:
                fail(
                    f"{rel_from_payload} expects {quoted!r} before itself; "
                    f"unit order places {target_module} too late"
                )


def extract_hoisted_angle_block(unit_text: str) -> str:
    start = unit_text.find("/* Hoisted system headers */")
    end = unit_text.find("/* MSVC default C-mode compatibility */")
    if start < 0 or end < 0:
        fail("juce_box2d_upstream.c is missing hoisted header markers")
    return unit_text[start:end]


def validate_public_header(unit_text: str, header_text: str) -> None:
    namespace_position = header_text.find("namespace juce")
    handler_position = header_text.find("handleBox2DAssertion")
    if handler_position < 0:
        fail("juce_box2d_upstream.h must declare handleBox2DAssertion")
    if namespace_position < 0 or handler_position < namespace_position:
        fail("handleBox2DAssertion must be declared in namespace juce for C++")

    for header in PUBLIC_API_HEADERS:
        include = f'#include "{header}"'
        include_position = header_text.find(include)
        if include_position < 0:
            fail(f"juce_box2d_upstream.h must include {header}")
        if include_position > namespace_position:
            fail(f"juce_box2d_upstream.h must expose {header} in the global namespace")

    user_config_position = header_text.find("#ifdef BOX2D_USER_CONFIG")
    if user_config_position < 0:
        fail("juce_box2d_upstream.h must reproduce BOX2D_USER_CONFIG")
    if user_config_position > namespace_position:
        fail("BOX2D_USER_CONFIG must be included in the global namespace")

    expected = (
        PRIVATE_HEADERS
        + [f"{MODULE_PREFIX}/src/recording_ops.inl"]
        + [
            f"{MODULE_PREFIX}/src/qsort.h",
            *[
                f"{MODULE_PREFIX}/src/{name}"
                for name in SOURCE_C_FILES
            ],
        ]
    )

    unit_includes = parse_unit_includes(unit_text)
    for item in expected:
        if item not in unit_includes:
            fail(f"juce_box2d_upstream.c missing include {item}")

    qsort_index = unit_includes.index(f"{MODULE_PREFIX}/src/qsort.h")
    broad_phase_index = unit_includes.index(f"{MODULE_PREFIX}/src/broad_phase.c")
    if qsort_index + 1 != broad_phase_index:
        fail("qsort.h must immediately precede broad_phase.c in juce_box2d_upstream.c")

    determinism = f"{MODULE_PREFIX}/shared/determinism.c"
    if determinism not in unit_text:
        fail("juce_box2d_upstream.c must include determinism.c under JUCE_UNIT_TESTS")


def apply_macro_line(active_macros: set[str], line: str) -> None:
    for match in UNDEF_RE.finditer(line):
        active_macros.discard(match.group(1))
    for match in MACRO_DEF_RE.finditer(line):
        active_macros.add(match.group(1))


def validate_macro_leaks(unit_text: str, payload_root: Path) -> None:
    active_macros: set[str] = set()
    root = repo_root()

    for line in unit_text.splitlines():
        if UNDEF_RE.search(line):
            apply_macro_line(active_macros, line)

        include_match = re.match(r'^\s*#\s*include\s+"([^"]+\.c)"', line)
        if include_match is None:
            continue

        source_path = root / "modules" / include_match.group(1).replace("/", os.sep)
        source_text = read(source_path)

        for macro in sorted(active_macros):
            for source_line in source_text.splitlines():
                if source_line.lstrip().startswith("#"):
                    continue
                if re.search(rf"\b{re.escape(macro)}\b", source_line):
                    fail(
                        f"macro {macro} from an earlier source reaches "
                        f"{include_match.group(1)}; add #undef between sources"
                    )

        for source_line in source_text.splitlines():
            apply_macro_line(active_macros, source_line)


def validate_static_collisions(unit_text: str) -> None:
    seen_static: dict[str, str] = {}
    root = repo_root()

    for line in unit_text.splitlines():
        include_match = re.match(r'^\s*#\s*include\s+"([^"]+\.c)"', line)
        if include_match is None:
            continue
        source_path = root / "modules" / include_match.group(1).replace("/", os.sep)
        source_text = read(source_path)
        for match in STATIC_DEF_RE.finditer(source_text):
            name = match.group(1)
            previous = seen_static.get(name)
            if previous is not None and previous != include_match.group(1):
                fail(
                    f"static {name} in {include_match.group(1)} collides with "
                    f"{previous} in one translation unit"
                )
            seen_static[name] = include_match.group(1)


def validate_upstream_files(payload_root: Path) -> None:
    root = repo_root()
    unit_path = root / UPSTREAM_C
    header_path = root / UPSTREAM_H
    if not unit_path.is_file() or not header_path.is_file():
        fail("missing juce_box2d_upstream.c or juce_box2d_upstream.h beside the module")

    unit_text = read(unit_path)
    header_text = read(header_path)

    validate_public_header(unit_text, header_text)
    validate_macro_leaks(unit_text, payload_root)
    validate_static_collisions(unit_text)


def write_upstream_note(dest: Path) -> None:
    write(
        dest / "JUCE_UPSTREAM.txt",
        UPSTREAM_NOTE.format(repo=REPO, version=DESCRIBED_VERSION, commit=COMMIT),
    )


def main() -> None:
    dest = rel_to_py(".")
    root = repo_root()

    with tempfile.TemporaryDirectory() as temporary:
        tree = Path(temporary).resolve() / "box2d"
        git("clone", "--quiet", REPO, str(tree))
        git("-C", str(tree), "checkout", "--quiet", COMMIT)

        for entry in dest.iterdir():
            if entry.name != Path(__file__).name:
                shutil.rmtree(entry) if entry.is_dir() else entry.unlink()

        copy_payload(tree, dest)
        transform_payload(dest)
        write_upstream_note(dest)

    validate_upstream_files(dest)

    print(f"Vendored Box2D at {COMMIT}")


if __name__ == "__main__":
    main()
