#!/bin/bash

utfValues = (8, 16, 32)

for utfValues in "${utfValues[@]}"
do
    pushd buildForUTF${utfValues}
    make -C extras/UnitTestRunner/Builds/LinuxMakefile JUCE_STRING_UTF_TYPE=${utfValues}
    ./extras/UnitTestRunner/Builds/LinuxMakefile/build/UnitTestRunner
    make -C extras/UnitTestRunner/Builds/LinuxMakefile clean
    popd
done
