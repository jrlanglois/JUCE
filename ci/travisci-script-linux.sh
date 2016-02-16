#!/bin/bash

make -C extras/UnitTestRunner/Builds/LinuxMakefile JUCE_STRING_UTF_TYPE=8
./extras/UnitTestRunner/Builds/LinuxMakefile/build/UnitTestRunner
make -C extras/UnitTestRunner/Builds/LinuxMakefile clean

make -C extras/UnitTestRunner/Builds/LinuxMakefile JUCE_STRING_UTF_TYPE=16
./extras/UnitTestRunner/Builds/LinuxMakefile/build/UnitTestRunner
make -C extras/UnitTestRunner/Builds/LinuxMakefile clean

make -C extras/UnitTestRunner/Builds/LinuxMakefile JUCE_STRING_UTF_TYPE=32
./extras/UnitTestRunner/Builds/LinuxMakefile/build/UnitTestRunner
make -C extras/UnitTestRunner/Builds/LinuxMakefile clean
