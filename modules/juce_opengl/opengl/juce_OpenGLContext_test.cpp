/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-9-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

class OpenGLContextUnitTest final : public UnitTest
{
public:
    OpenGLContextUnitTest()
        : UnitTest ("OpenGLContext", UnitTestCategories::graphics)
    {
    }

    void runTest() override
    {
        testDefaultChains();
        testListedChains();
        testNormalisationAndVersionLists();
        testEGLConfigSelection();
        testVersionParser();
        testProfileDetection();
        testGLSLVersionStrings();
        testRequestMatching();
        testTextureFormats();
        testRedBlueSwap();
        testPrecisionPlaceholders();
    }

private:
    using Request = OpenGLContextRequest;
    using Version = OpenGLVersion;
    using Limits = detail::PlatformLimits;
    using Prepared = detail::PreparedContextRequest;

    static Limits getWindowsLimits()
    {
        return { true,
                 false,
                 { { 3, 2 }, { 3, 3 }, { 4, 0 }, { 4, 1 }, { 4, 2 },
                   { 4, 3 }, { 4, 4 }, { 4, 5 }, { 4, 6 } },
                 { { 2, 0 }, { 2, 1 }, { 3, 0 }, { 3, 1 }, { 3, 2 },
                   { 3, 3 }, { 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 },
                   { 4, 4 }, { 4, 5 }, { 4, 6 } },
                 {} };
    }

    static Limits getMacLimits()
    {
        return { true, false, { { 3, 2 }, { 4, 1 } }, { { 2, 1 } }, {} };
    }

    static Limits getLinuxLimits()
    {
        auto result = getWindowsLimits();
        result.canCreateOpenGLES = true;
        result.openGLESVersions = { { 2, 0 }, { 3, 0 }, { 3, 1 }, { 3, 2 } };
        return result;
    }

    static Limits getIOSLimits()
    {
        return { false, true, {}, {}, { { 2, 0 }, { 3, 0 } } };
    }

    static Limits getAndroidLimits()
    {
        return { false, true, {}, {}, { { 2, 0 }, { 3, 0 }, { 3, 1 }, { 3, 2 } } };
    }

    static std::vector<Prepared> build (std::vector<Request> listed,
                                        Request preferred,
                                        bool hasRenderer,
                                        const Limits& limits)
    {
        return detail::buildContextRequestChain (listed, preferred, hasRenderer, limits);
    }

    void expectVersions (const Prepared& prepared,
                         std::initializer_list<Version> expected,
                         const String& failureMessage)
    {
        expect (prepared.versions == std::vector<Version> (expected), failureMessage);
    }

    void testDefaultChains()
    {
        beginTest ("Default chains apply the platform fallbacks");

        for (const auto hasRenderer : { false, true })
        {
            const auto windows = build ({},
                                        { OpenGLAPI::openGL, { 4, 6 }, OpenGLProfile::core },
                                        hasRenderer,
                                        getWindowsLimits());
            expectEquals ((int) windows.size(), 2);
            expectVersions (windows[0],
                            { { 4, 6 }, { 4, 5 }, { 4, 4 }, { 4, 3 }, { 4, 2 },
                              { 4, 1 }, { 4, 0 }, { 3, 3 }, { 3, 2 } },
                            "Windows must step down through every core version");
            expectVersions (windows[1], { {} }, "Windows rule 2 must use legacy creation");

            const auto mac = build ({},
                                    { OpenGLAPI::openGL, { 4, 6 }, OpenGLProfile::core },
                                    hasRenderer,
                                    getMacLimits());
            expectEquals ((int) mac.size(), 2);
            expectVersions (mac[0], { { 4, 1 }, { 3, 2 } }, "macOS must clamp to 4.1");
            expectVersions (mac[1], { {} }, "macOS rule 2 must use its legacy context");

            const auto ios = build ({},
                                    { OpenGLAPI::openGL, { 2, 0 }, OpenGLProfile::compatibility },
                                    hasRenderer,
                                    getIOSLimits());
            expectEquals ((int) ios.size(), 2);
            expect (ios[0].normalised.api == OpenGLAPI::openGLES);
            expectVersions (ios[0], { { 2, 0 } }, "iOS must use the supported API");
            expectVersions (ios[1], { { 3, 0 }, { 2, 0 } }, "iOS rule 2 must use its ES default");

            const auto android = build ({},
                                        { OpenGLAPI::openGL, { 3, 2 }, OpenGLProfile::compatibility },
                                        hasRenderer,
                                        getAndroidLimits());
            expectEquals ((int) android.size(), 2);
            expect (android[0].normalised.api == OpenGLAPI::openGLES);
            expectVersions (android[0],
                            { { 3, 2 }, { 3, 1 }, { 3, 0 }, { 2, 0 } },
                            "Android must step down through every ES version");
            expectVersions (android[1],
                            { { 3, 0 }, { 2, 0 } },
                            "Android rule 2 must use its ES default");
        }

        const auto linuxWithRenderer = build ({},
                                              { OpenGLAPI::openGL, { 4, 6 }, OpenGLProfile::core },
                                              true,
                                              getLinuxLimits());
        expectEquals ((int) linuxWithRenderer.size(), 2);

        const auto linuxWithoutRenderer = build ({},
                                                 { OpenGLAPI::openGL, { 4, 6 }, OpenGLProfile::core },
                                                 false,
                                                 getLinuxLimits());
        expectEquals ((int) linuxWithoutRenderer.size(), 3);
        expect (linuxWithoutRenderer[2].normalised.api == OpenGLAPI::openGLES);
        expectVersions (linuxWithoutRenderer[2],
                        { { 3, 0 }, { 2, 0 } },
                        "Linux rule 3 must cross to the other API without a renderer");

        const auto linuxStartingWithES = build ({},
                                                { OpenGLAPI::openGLES, { 3, 2 }, OpenGLProfile::core },
                                                false,
                                                getLinuxLimits());
        expectEquals ((int) linuxStartingWithES.size(), 3);
        expect (linuxStartingWithES[2].normalised.api == OpenGLAPI::openGL);
        expectVersions (linuxStartingWithES[2],
                        { {} },
                        "Linux rule 3 must work in both directions");
    }

    void testListedChains()
    {
        beginTest ("Listed chains preserve order and add no fallbacks");

        const std::vector<Request> listed
        {
            { OpenGLAPI::openGLES, { 3, 0 }, OpenGLProfile::compatibility },
            { OpenGLAPI::openGL, { 3, 2 }, OpenGLProfile::core },
            { OpenGLAPI::openGL, {}, OpenGLProfile::core }
        };

        const auto linux = build (listed, {}, false, getLinuxLimits());
        expectEquals ((int) linux.size(), 3);
        expect (linux[0].normalised.api == OpenGLAPI::openGLES);
        expect (linux[1].normalised.version == Version { 3, 2 });
        expect (linux[2].normalised.profile == OpenGLProfile::compatibility);

        const auto windows = build (listed, {}, false, getWindowsLimits());
        expectEquals ((int) windows.size(), 2);
        expect (windows[0].normalised.version == Version { 3, 2 });
        expect (windows[1].normalised.version == Version{});

        const auto ios = build (listed, {}, false, getIOSLimits());
        expectEquals ((int) ios.size(), 1);
        expect (ios[0].normalised.api == OpenGLAPI::openGLES);

        const auto empty = build ({ { OpenGLAPI::openGLES, { 3, 0 }, OpenGLProfile::core } },
                                  {},
                                  false,
                                  getWindowsLimits());
        expect (empty.empty());
    }

    void testNormalisationAndVersionLists()
    {
        beginTest ("Requests are normalised, snapped, clamped and deduplicated");

        const auto defaultDesktop = build ({ { OpenGLAPI::openGL, {}, OpenGLProfile::core } },
                                           {},
                                           true,
                                           getMacLimits());
        expectEquals ((int) defaultDesktop.size(), 1);
        expect (defaultDesktop[0].normalised.profile == OpenGLProfile::compatibility);
        expectVersions (defaultDesktop[0], { {} }, "A desktop default keeps an unversioned step");

        const auto esCompatibility = build (
            { { OpenGLAPI::openGLES, { 3, 0 }, OpenGLProfile::compatibility } },
            {},
            true,
            getAndroidLimits());
        expectEquals ((int) esCompatibility.size(), 1);
        expect (esCompatibility[0].normalised.profile == OpenGLProfile::core);

        const auto snapped = build ({ { OpenGLAPI::openGL, { 3, 5 }, OpenGLProfile::core } },
                                    {},
                                    true,
                                    getWindowsLimits());
        expect (snapped[0].normalised.version == Version { 3, 3 });
        expectVersions (snapped[0],
                        { { 3, 3 }, { 3, 2 } },
                        "An unreleased desktop version must snap down");

        const auto desktopFloor = build ({ { OpenGLAPI::openGL, { 1, 0 }, OpenGLProfile::compatibility } },
                                         {},
                                         true,
                                         getWindowsLimits());
        expect (desktopFloor[0].normalised.version == Version { 2, 0 });

        const auto desktopCeiling = build ({ { OpenGLAPI::openGL, { 9, 0 }, OpenGLProfile::core } },
                                           {},
                                           true,
                                           getWindowsLimits());
        expect (desktopCeiling[0].normalised.version == Version { 4, 6 });

        const auto macCeiling = build ({ { OpenGLAPI::openGL, { 4, 6 }, OpenGLProfile::core } },
                                       {},
                                       true,
                                       getMacLimits());
        expect (macCeiling[0].normalised.version == Version { 4, 1 });
        expectVersions (macCeiling[0],
                        { { 4, 1 }, { 3, 2 } },
                        "macOS must use only its expressible core profiles");

        const auto iosCeiling = build ({ { OpenGLAPI::openGLES, { 3, 2 }, OpenGLProfile::core } },
                                       {},
                                       true,
                                       getIOSLimits());
        expect (iosCeiling[0].normalised.version == Version { 3, 0 });
        expectVersions (iosCeiling[0],
                        { { 3, 0 }, { 2, 0 } },
                        "iOS must clamp to ES 3.0");

        const auto androidSnap = build ({ { OpenGLAPI::openGLES, { 3, 5 }, OpenGLProfile::core } },
                                        {},
                                        true,
                                        getAndroidLimits());
        expect (androidSnap[0].normalised.version == Version { 3, 2 });
        expectVersions (androidSnap[0],
                        { { 3, 2 }, { 3, 1 }, { 3, 0 }, { 2, 0 } },
                        "Android must snap to ES 3.2");

        const auto deduplicatedMac = build (
            { { OpenGLAPI::openGL, { 4, 6 }, OpenGLProfile::core },
              { OpenGLAPI::openGL, { 9, 0 }, OpenGLProfile::core } },
            {},
            true,
            getMacLimits());
        expectEquals ((int) deduplicatedMac.size(), 1);

        const auto deduplicatedIOS = build (
            { { OpenGLAPI::openGLES, { 3, 0 }, OpenGLProfile::core },
              { OpenGLAPI::openGLES, {}, OpenGLProfile::core } },
            {},
            true,
            getIOSLimits());
        expectEquals ((int) deduplicatedIOS.size(), 1);
    }

    void testEGLConfigSelection()
    {
        beginTest ("EGL version attempts respect the config renderable type");

        expect (detail::canTryEGLVersion (OpenGLAPI::openGL, { 4, 6 }, false, false));
        expect (! detail::canTryEGLVersion (OpenGLAPI::openGLES, { 3, 2 }, false, true));
        expect (detail::canTryEGLVersion (OpenGLAPI::openGLES, { 3, 2 }, true, false));
        expect (! detail::canTryEGLVersion (OpenGLAPI::openGLES, { 2, 0 }, true, false));
        expect (detail::canTryEGLVersion (OpenGLAPI::openGLES, { 2, 0 }, false, true));
        expect (! detail::canTryEGLVersion (OpenGLAPI::openGLES, { 1, 1 }, true, true));
    }

    void testVersionParser()
    {
        beginTest ("OpenGL version strings follow the specification grammar");

        struct TestCase
        {
            const char* input;
            Version expected;
            const char* source;
        };

        const TestCase testCases[]
        {
            // Desktop GL_VERSION starts with major.minor:
            // https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf
            { "4.6.0 NVIDIA 555.42.02", { 4, 6 }, "OpenGL 4.6 specification" },

            // Mesa appends its package and Git description after the specified prefix:
            // https://gitlab.freedesktop.org/mesa/mesa/-/blob/main/src/mesa/main/version.c
            { "3.3 (Core Profile) Mesa 24.2.8", { 3, 3 }, "Mesa version.c" },

            // ES GL_VERSION begins with "OpenGL ES" followed by major.minor:
            // https://registry.khronos.org/OpenGL/specs/es/3.2/es_spec_3.2.pdf
            { "OpenGL ES 3.2 Mesa 24.2.8", { 3, 2 }, "OpenGL ES 3.2 specification" },

            // The ES 1.x grammar permits an implementation profile suffix:
            // https://registry.khronos.org/OpenGL/specs/es/1.1/es_full_spec_1.1.pdf
            { "OpenGL ES-CM 1.1 Apple", { 1, 1 }, "OpenGL ES 1.1 specification" }
        };

        for (const auto& test : testCases)
        {
            const auto parsed = detail::parseOpenGLVersionString (test.input);
            expect (parsed.has_value(), test.source);

            if (parsed.has_value())
                expect (parsed->toOpenGLVersion() == test.expected, test.source);
        }

        const auto languageVersion = detail::parseOpenGLVersionString ("4.60 NVIDIA");
        expect (languageVersion.has_value());

        if (languageVersion.has_value())
            expectWithinAbsoluteError (languageVersion->toDouble(), 4.6, 0.00001);

        for (const auto* invalid : { "", "OpenGL 4.6", "OpenGL ES", "OpenGL ES 3", "3." })
            expect (! detail::parseOpenGLVersionString (invalid).has_value());
    }

    void testProfileDetection()
    {
        beginTest ("Profile detection follows API and version rules");

        expect (detail::determineOpenGLProfile (OpenGLAPI::openGLES,
                                                { 2, 0 },
                                                false,
                                                false,
                                                false)
                == OpenGLProfile::core);
        expect (detail::determineOpenGLProfile (OpenGLAPI::openGL,
                                                { 2, 1 },
                                                true,
                                                false,
                                                true)
                == OpenGLProfile::compatibility);
        expect (detail::determineOpenGLProfile (OpenGLAPI::openGL,
                                                { 3, 0 },
                                                false,
                                                false,
                                                false)
                == OpenGLProfile::compatibility);
        expect (detail::determineOpenGLProfile (OpenGLAPI::openGL,
                                                { 3, 0 },
                                                true,
                                                false,
                                                false)
                == OpenGLProfile::core);
        expect (detail::determineOpenGLProfile (OpenGLAPI::openGL,
                                                { 3, 1 },
                                                false,
                                                true,
                                                false)
                == OpenGLProfile::compatibility);
        expect (detail::determineOpenGLProfile (OpenGLAPI::openGL,
                                                { 3, 1 },
                                                false,
                                                false,
                                                false)
                == OpenGLProfile::core);
        expect (detail::determineOpenGLProfile (OpenGLAPI::openGL,
                                                { 3, 2 },
                                                false,
                                                false,
                                                true)
                == OpenGLProfile::core);
        expect (detail::determineOpenGLProfile (OpenGLAPI::openGL,
                                                { 3, 2 },
                                                false,
                                                false,
                                                false)
                == OpenGLProfile::compatibility);
    }

    void testGLSLVersionStrings()
    {
        beginTest ("GLSL prefixes match the active API");

        expectEquals (detail::getGLSLVersionString (OpenGLAPI::openGL, { 2, 0 }),
                      String ("#version 110"));
        expectEquals (detail::getGLSLVersionString (OpenGLAPI::openGL, { 3, 2 }),
                      String ("#version 150"));
        expectEquals (detail::getGLSLVersionString (OpenGLAPI::openGLES, { 2, 0 }),
                      String ("#version 100"));
        expectEquals (detail::getGLSLVersionString (OpenGLAPI::openGLES, { 3, 0 }),
                      String ("#version 300 es"));
    }

    void testRequestMatching()
    {
        beginTest ("Created contexts are checked against the accepted request");

        const Request desktopCore { OpenGLAPI::openGL, { 4, 1 }, OpenGLProfile::core };
        expect (detail::contextMatchesRequest (desktopCore,
                                               { 4, 1 },
                                               OpenGLAPI::openGL,
                                               { 4, 6 },
                                               OpenGLProfile::core));
        expect (! detail::contextMatchesRequest (desktopCore,
                                                 { 4, 1 },
                                                 OpenGLAPI::openGL,
                                                 { 4, 0 },
                                                 OpenGLProfile::core));
        expect (! detail::contextMatchesRequest (desktopCore,
                                                 { 4, 1 },
                                                 OpenGLAPI::openGL,
                                                 { 4, 6 },
                                                 OpenGLProfile::compatibility));
        expect (! detail::contextMatchesRequest (desktopCore,
                                                 { 4, 1 },
                                                 OpenGLAPI::openGLES,
                                                 { 4, 6 },
                                                 OpenGLProfile::core));

        const Request desktopDefault { OpenGLAPI::openGL, {}, OpenGLProfile::compatibility };
        expect (detail::contextMatchesRequest (desktopDefault,
                                               {},
                                               OpenGLAPI::openGL,
                                               { 4, 6 },
                                               OpenGLProfile::core));

        const Request es { OpenGLAPI::openGLES, { 3, 0 }, OpenGLProfile::core };
        expect (detail::contextMatchesRequest (es,
                                               { 3, 0 },
                                               OpenGLAPI::openGLES,
                                               { 3, 2 },
                                               OpenGLProfile::compatibility));
    }

    void testTextureFormats()
    {
        beginTest ("Texture formats account for each ES BGRA extension");

        const auto rgba = detail::chooseTextureFormats (true, false, false, false, false);
        expectEquals ((int) rgba.uploadInternalFormat, (int) GL_RGBA);
        expectEquals ((int) rgba.uploadFormat, (int) GL_RGBA);
        expectEquals ((int) rgba.readFormat, (int) GL_RGBA);
        expect (! rgba.swapRedAndBlueOnUpload);
        expect (! rgba.swapRedAndBlueAfterRead);

        const auto desktop = detail::chooseTextureFormats (false, true, false, false, false);
        expectEquals ((int) desktop.uploadInternalFormat, (int) GL_RGBA);
        expectEquals ((int) desktop.uploadFormat, (int) GL_BGRA_EXT);
        expectEquals ((int) desktop.readFormat, (int) GL_BGRA_EXT);
        expect (! desktop.swapRedAndBlueOnUpload);
        expect (! desktop.swapRedAndBlueAfterRead);

        const auto apple = detail::chooseTextureFormats (true, true, true, false, true);
        expectEquals ((int) apple.uploadInternalFormat, (int) GL_RGBA);
        expectEquals ((int) apple.uploadFormat, (int) GL_BGRA_EXT);
        expectEquals ((int) apple.readFormat, (int) GL_BGRA_EXT);
        expect (! apple.swapRedAndBlueOnUpload);
        expect (! apple.swapRedAndBlueAfterRead);

        const auto ext = detail::chooseTextureFormats (true, true, false, true, false);
        expectEquals ((int) ext.uploadInternalFormat, (int) GL_BGRA_EXT);
        expectEquals ((int) ext.uploadFormat, (int) GL_BGRA_EXT);
        expectEquals ((int) ext.readFormat, (int) GL_RGBA);
        expect (! ext.swapRedAndBlueOnUpload);
        expect (ext.swapRedAndBlueAfterRead);

        const auto noExtensions = detail::chooseTextureFormats (true, true, false, false, false);
        expectEquals ((int) noExtensions.uploadInternalFormat, (int) GL_RGBA);
        expectEquals ((int) noExtensions.uploadFormat, (int) GL_RGBA);
        expectEquals ((int) noExtensions.readFormat, (int) GL_RGBA);
        expect (noExtensions.swapRedAndBlueOnUpload);
        expect (noExtensions.swapRedAndBlueAfterRead);

        const auto readOnly = detail::chooseTextureFormats (true, true, false, false, true);
        expect (readOnly.swapRedAndBlueOnUpload);
        expect (! readOnly.swapRedAndBlueAfterRead);
    }

    void testRedBlueSwap()
    {
        beginTest ("Red and blue channels are swapped without changing alpha or green");

        PixelARGB pixels[]
        {
            { 0x11, 0x22, 0x33, 0x44 },
            { 0x55, 0x66, 0x77, 0x88 }
        };

        detail::swapRedAndBlue (pixels);

        expectEquals (pixels[0].getAlpha(), (uint8) 0x11);
        expectEquals (pixels[0].getRed(),   (uint8) 0x44);
        expectEquals (pixels[0].getGreen(), (uint8) 0x33);
        expectEquals (pixels[0].getBlue(),  (uint8) 0x22);
        expectEquals (pixels[1].getAlpha(), (uint8) 0x55);
        expectEquals (pixels[1].getRed(),   (uint8) 0x88);
        expectEquals (pixels[1].getGreen(), (uint8) 0x77);
        expectEquals (pixels[1].getBlue(),  (uint8) 0x66);
    }

    void testPrecisionPlaceholders()
    {
        beginTest ("Precision translation replaces only JUCE placeholders");

        const String source { "#lowp# a; #mediump# b; #highp# c; lowp d;" };
        expectEquals (preprocessShaderPrecisionStatements (source, true),
                      String { "lowp a; mediump b; highp c; lowp d;" });
        expectEquals (preprocessShaderPrecisionStatements (source, false),
                      String { " a;  b;  c; lowp d;" });
    }
};

static OpenGLContextUnitTest openGLContextUnitTest;

} // namespace juce
