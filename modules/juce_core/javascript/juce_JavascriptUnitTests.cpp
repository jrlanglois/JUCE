/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

class JavascriptTests  : public UnitTest
{
public:
    JavascriptTests() : UnitTest ("Javascript", "Scripting") {}

    void runTest() override
    {
        JavascriptEngine engine;

        beginTest ("Globals");

        expect (engine.evaluate ("exec").isUndefined());
        expect (static_cast<int> (engine.evaluate ("eval (10 * 10)")) == 100);
        expect (engine.evaluate ("trace").isUndefined());
        expect (engine.evaluate ("typeof").isUndefined());
        expect (engine.evaluate ("charToInt").isUndefined());
        expect (engine.evaluate ("parseInt").isUndefined());
        expect (engine.evaluate ("parseFloat").isUndefined());
        expect (engine.evaluate ("isNaN").isUndefined());
        expect (engine.evaluate ("isFinite").isUndefined());
        expect (engine.evaluate ("Infinity").isUndefined());
        expect (engine.evaluate ("NaN").isUndefined());

        beginTest ("ArrayBuffer");

        beginTest ("Array");

        beginTest ("Boolean");

        beginTest ("DataView");

        beginTest ("Date");

        beginTest ("JSON");

        beginTest ("Map");

        beginTest ("Math");

        beginTest ("Number");

        beginTest ("Object");

        beginTest ("RegExp");

        beginTest ("Set");

        beginTest ("String");

        beginTest ("WeakMap");

        beginTest ("WeakSet");

        beginTest ("Console");
    }
};

static JavascriptTests javascriptUnitTests;

} // namespace juce
