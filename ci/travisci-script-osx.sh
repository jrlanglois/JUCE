#!/bin/bash

#MacOSX build
xcodebuild -project "extras/UnitTestRunner/Builds/MacOSX/UnitTestRunner.xcodeproj" GCC_PREPROCESSOR_DEFINITIONS='$GCC_PREPROCESSOR_DEFINITIONS JUCE_STRING_UTF_TYPE=8'
xcodebuild -project "extras/UnitTestRunner/Builds/MacOSX/UnitTestRunner.xcodeproj" GCC_PREPROCESSOR_DEFINITIONS='$GCC_PREPROCESSOR_DEFINITIONS JUCE_STRING_UTF_TYPE=16'
xcodebuild -project "extras/UnitTestRunner/Builds/MacOSX/UnitTestRunner.xcodeproj" GCC_PREPROCESSOR_DEFINITIONS='$GCC_PREPROCESSOR_DEFINITIONS JUCE_STRING_UTF_TYPE=32'

#iOS build
xcodebuild -project "extras/UnitTestRunner/Builds/iOS/UnitTestRunner.xcodeproj" GCC_PREPROCESSOR_DEFINITIONS='$GCC_PREPROCESSOR_DEFINITIONS JUCE_STRING_UTF_TYPE=8'
xcodebuild -project "extras/UnitTestRunner/Builds/iOS/UnitTestRunner.xcodeproj" GCC_PREPROCESSOR_DEFINITIONS='$GCC_PREPROCESSOR_DEFINITIONS JUCE_STRING_UTF_TYPE=16'
xcodebuild -project "extras/UnitTestRunner/Builds/iOS/UnitTestRunner.xcodeproj" GCC_PREPROCESSOR_DEFINITIONS='$GCC_PREPROCESSOR_DEFINITIONS JUCE_STRING_UTF_TYPE=32'
