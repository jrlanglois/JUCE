#!/usr/bin/env python3

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[4]
BOX2D_DIR = REPO_ROOT / "modules" / "juce_box2d" / "box2d"
sys.path.insert(0, str(BOX2D_DIR))

from box2d_sample_import import (  # noqa: E402
    RegistrationEntry,
    apply_unified_patch,
    build_sample_translations_source,
    collect_registrations,
    collect_sample_translation_keys,
    compare_directory_trees,
    parse_registrations_from_text,
    scan_forbidden_references,
    validate_registration_parity,
    validate_spdx_headers,
)

UPSTREAM_ROOT = REPO_ROOT / "examples" / "Utilities" / "Box2D" / "Upstream"
MANIFEST_PATH = UPSTREAM_ROOT / "sample_registry.json"


class SampleImporterTests(unittest.TestCase):
    def test_parse_register_sample(self) -> None:
        text = 'static int x = RegisterSample( "Bodies", "Sleep", Sleep::Create );\n'
        entries = parse_registrations_from_text(text, "samples/sample_bodies.cpp")
        self.assertEqual(len(entries), 1)
        entry = entries[0]
        self.assertEqual(entry.kind, "sample")
        self.assertEqual(entry.category, "Bodies")
        self.assertEqual(entry.name, "Sleep")
        self.assertEqual(entry.create_symbol, "Sleep::Create")

    def test_parse_register_sample_with_capacity(self) -> None:
        text = (
            '\tRegisterSampleWithCapacity( "Benchmark", "Many Pyramids", '
            "BenchmarkManyPyramids::Create, BenchmarkManyPyramids::GetCapacity );\n"
        )
        entries = parse_registrations_from_text(text, "samples/sample_benchmark.cpp")
        self.assertEqual(len(entries), 1)
        self.assertEqual(entries[0].kind, "sampleWithCapacity")
        self.assertEqual(entries[0].capacity_symbol, "BenchmarkManyPyramids::GetCapacity")

    def test_parse_register_replay(self) -> None:
        text = 'static int r = RegisterReplay( "Replay", "Viewer", ReplayViewer::Create );\n'
        entries = parse_registrations_from_text(text, "samples/sample_replay.cpp")
        self.assertEqual(entries[0].kind, "replay")

    def test_manifest_matches_sources(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), "run vendor.py to generate Upstream content")
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        live_entries = collect_registrations(UPSTREAM_ROOT)
        manifest_entries = [
            RegistrationEntry(
                kind=item["kind"],
                category=item["category"],
                name=item["name"],
                create_symbol=item["createSymbol"],
                capacity_symbol=item.get("capacitySymbol"),
                source_file=item["sourceFile"],
                line_number=item["line"],
            )
            for item in manifest["registrations"]
        ]
        self.assertEqual(manifest_entries, live_entries)

    def test_manifest_has_unique_names(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        keys = [(item["category"], item["name"]) for item in manifest["registrations"]]
        self.assertEqual(len(keys), len(set(keys)))

    def test_translation_source_marks_every_static_label(self) -> None:
        entries = collect_registrations(UPSTREAM_ROOT)
        source = build_sample_translations_source(UPSTREAM_ROOT, entries)
        labels = collect_sample_translation_keys(UPSTREAM_ROOT, entries)
        self.assertEqual(source.count("NEEDS_TRANS ("), len(labels))

        for label in labels:
            self.assertIn(f"NEEDS_TRANS ({json.dumps(label)})", source)

        self.assertIn('NEEDS_TRANS ("Fire Bullets")', source)
        self.assertIn('NEEDS_TRANS ("match: sleep step %d")', source)
        self.assertIn('NEEDS_TRANS ("Multiple")', source)
        self.assertIn('NEEDS_TRANS ("Team 2")', source)
        self.assertNotIn('NEEDS_TRANS ("Team 2##1")', source)

    def test_forbidden_host_references_absent(self) -> None:
        findings = scan_forbidden_references(UPSTREAM_ROOT)
        self.assertEqual(findings, [])

    def test_source_files_have_spdx_headers(self) -> None:
        self.assertEqual(validate_spdx_headers(UPSTREAM_ROOT), [])

    def test_registration_parity_rejects_a_missing_sample(self) -> None:
        entries = collect_registrations(UPSTREAM_ROOT)
        with self.assertRaisesRegex(RuntimeError, "does not match"):
            validate_registration_parity(entries, entries[1:])

    def test_compare_directory_trees_detects_diff(self) -> None:
        left = UPSTREAM_ROOT / "samples"
        right = UPSTREAM_ROOT / "shared"
        mismatches = compare_directory_trees(left, right)
        self.assertTrue(any("missing file" in item or "unexpected file" in item for item in mismatches))

    def test_patch_applies_inside_parent_git_repository(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            repository = Path(temporary)
            subprocess.run(["git", "init", "--quiet"], cwd=repository, check=True)

            destination = repository / "nested" / "payload"
            destination.mkdir(parents=True)
            (destination / "value.txt").write_text("old\n", encoding="utf-8")

            patch_path = repository / "change.patch"
            patch_path.write_text(
                "--- a/value.txt\n"
                "+++ b/value.txt\n"
                "@@ -1 +1 @@\n"
                "-old\n"
                "+new\n",
                encoding="utf-8",
            )

            apply_unified_patch(destination, patch_path)
            self.assertEqual((destination / "value.txt").read_text(encoding="utf-8"), "new\n")


if __name__ == "__main__":
    unittest.main()
