#!/usr/bin/env python3

import argparse
import subprocess
import tempfile
import xml.etree.ElementTree as ElementTree
from pathlib import Path


def writeFixture(fixtureDirectory: Path) -> Path:
    pipPath = fixtureDirectory / "Box2DPIPGeneratorTest.h"
    pipPath.write_text(
        """/*
  BEGIN_JUCE_PIP_METADATA

 name:             Box2DPIPGeneratorTest
 version:          1.0.0
 vendor:           JUCE
 website:          https://juce.com
 description:      Exercises direct C and C++ PIP companions.

 dependencies:     juce_core
 exporters:        vs2022

 type:             Console
 mainClass:        Box2DPIPGeneratorTest

 useLocalCopy:     1

  END_JUCE_PIP_METADATA
*/

#if 0
#include "box2d_companion.c"
#include "box2d_companion.cpp"
#include "box2d_resource.h"
#endif
""",
        encoding="utf-8",
        newline="\n",
    )

    (fixtureDirectory / "box2d_companion.c").write_text(
        "int getBox2DCValue (void) { return 1; }\n",
        encoding="utf-8",
        newline="\n",
    )
    (fixtureDirectory / "box2d_companion.cpp").write_text(
        "int getBox2DCppValue() { return 2; }\n",
        encoding="utf-8",
        newline="\n",
    )
    (fixtureDirectory / "box2d_resource.h").write_text(
        "#define BOX2D_RESOURCE_VALUE 3\n",
        encoding="utf-8",
        newline="\n",
    )
    return pipPath


def getCompileValues(jucerPath: Path) -> dict[str, str]:
    root = ElementTree.parse(jucerPath).getroot()
    return {
        element.attrib["name"]: element.attrib["compile"]
        for element in root.iter("FILE")
        if "name" in element.attrib and "compile" in element.attrib
    }


def runTest(projucerPath: Path, modulesPath: Path) -> None:
    with tempfile.TemporaryDirectory() as temporary:
        fixtureDirectory = Path(temporary) / "fixture"
        outputDirectory = Path(temporary) / "generated"
        fixtureDirectory.mkdir()
        outputDirectory.mkdir()
        pipPath = writeFixture(fixtureDirectory)

        result = subprocess.run(
            [
                str(projucerPath),
                "--create-project-from-pip",
                str(pipPath),
                str(outputDirectory),
                str(modulesPath),
            ],
            capture_output=True,
            text=True,
        )
        if result.returncode != 0:
            raise RuntimeError(result.stderr or result.stdout)

        jucerFiles = list(outputDirectory.rglob("*.jucer"))
        if len(jucerFiles) != 1:
            raise AssertionError(f"expected one generated project, found: {jucerFiles}")

        jucerPath = jucerFiles[0]
        compileValues = getCompileValues(jucerPath)
        expected = {
            "box2d_companion.c": "1",
            "box2d_companion.cpp": "1",
            "box2d_resource.h": "0",
        }
        if {name: compileValues.get(name) for name in expected} != expected:
            raise AssertionError(f"unexpected PIP compile values: {compileValues}")

        for name in expected:
            if not (jucerPath.parent / "Source" / name).is_file():
                raise AssertionError(f"PIP companion was not copied: {name}")


def parseArguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("projucer", type=Path)
    parser.add_argument("modules", type=Path)
    return parser.parse_args()


def main() -> None:
    arguments = parseArguments()
    runTest(arguments.projucer.resolve(), arguments.modules.resolve())
    print("PIP generator direct C/C++ companion test passed")


if __name__ == "__main__":
    main()
