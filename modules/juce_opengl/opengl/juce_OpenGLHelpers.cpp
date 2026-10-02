/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
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

template <typename Char>
static auto* findNullTerminator (const Char* ptr)
{
    while (*ptr != 0)
        ++ptr;

    return ptr;
}

static String preprocessShaderPrecisionStatements (String shaderSource, bool isES)
{
    return shaderSource.replace ("#lowp#",    isES ? "lowp" : "")
                       .replace ("#mediump#", isES ? "mediump" : "")
                       .replace ("#highp#",   isES ? "highp" : "");
}

static String preprocessShaderPrecisionStatements (String shaderSource)
{
    return preprocessShaderPrecisionStatements (std::move (shaderSource), OpenGLHelpers::isOpenGLES());
}

namespace detail
{
struct ParsedOpenGLVersion
{
    int major = 0;
    int minor = 0;
    int numMinorDigits = 0;

    OpenGLVersion toOpenGLVersion() const
    {
        return { major, minor };
    }

    double toDouble() const
    {
        auto divisor = 1.0;

        for (auto i = 0; i < numMinorDigits; ++i)
            divisor *= 10.0;

        return (double) major + ((double) minor / divisor);
    }
};

static std::optional<ParsedOpenGLVersion> parseOpenGLVersionString (const String& input)
{
    auto index = 0;

    if (input.startsWith ("OpenGL ES"))
    {
        while (index < input.length() && ! CharacterFunctions::isDigit (input[index]))
            ++index;
    }

    if (index >= input.length() || ! CharacterFunctions::isDigit (input[index]))
        return {};

    const auto majorBegin = index;

    while (index < input.length() && CharacterFunctions::isDigit (input[index]))
        ++index;

    if (index >= input.length() || input[index] != '.')
        return {};

    const auto major = input.substring (majorBegin, index).getIntValue();
    const auto minorBegin = ++index;

    while (index < input.length() && CharacterFunctions::isDigit (input[index]))
        ++index;

    if (minorBegin == index)
        return {};

    return ParsedOpenGLVersion { major,
                                 input.substring (minorBegin, index).getIntValue(),
                                 index - minorBegin };
}

static OpenGLProfile determineOpenGLProfile (OpenGLAPI api,
                                             OpenGLVersion version,
                                             bool isForwardCompatible,
                                             bool hasCompatibilityExtension,
                                             bool hasCoreProfile)
{
    if (api == OpenGLAPI::openGLES)
        return OpenGLProfile::core;

    if (version <= OpenGLVersion { 2, 1 })
        return OpenGLProfile::compatibility;

    if (version <= OpenGLVersion { 3, 0 })
        return isForwardCompatible ? OpenGLProfile::core : OpenGLProfile::compatibility;

    if (version <= OpenGLVersion { 3, 1 })
        return hasCompatibilityExtension ? OpenGLProfile::compatibility : OpenGLProfile::core;

    return hasCoreProfile ? OpenGLProfile::core : OpenGLProfile::compatibility;
}

static String getGLSLVersionString (OpenGLAPI api, OpenGLVersion version)
{
    if (api == OpenGLAPI::openGLES)
        return version >= OpenGLVersion { 3, 0 } ? "#version 300 es"
                                                : "#version 100";

    return version >= OpenGLVersion { 3, 2 } ? "#version 150"
                                             : "#version 110";
}

#if JUCE_ANDROID || JUCE_LINUX || JUCE_BSD || JUCE_UNIT_TESTS
static bool canTryEGLVersion (OpenGLAPI api,
                              OpenGLVersion version,
                              bool configSupportsES3,
                              bool configSupportsES2)
{
    if (api == OpenGLAPI::openGL || version == OpenGLVersion{})
        return true;

    if (version.major >= 3)
        return configSupportsES3;

    return version.major == 2 && configSupportsES2;
}
#endif
} // namespace detail

static OpenGLVersion getOpenGLVersion()
{
    if (auto* context = OpenGLContext::getCurrentContext())
        if (const auto info = context->getContextInfo())
            return info->version;

    const auto* versionString = glGetString (GL_VERSION);

    if (versionString == nullptr)
        return {};

    const auto parsed = detail::parseOpenGLVersionString (String::fromUTF8 ((const char*) versionString));

    if (parsed == std::nullopt)
        return {};

    const auto stringVersion = parsed->toOpenGLVersion();

    if (stringVersion >= OpenGLVersion { 3, 0 })
    {
        GLint major = 0, minor = 0;
        glGetIntegerv (GL_MAJOR_VERSION, &major);
        glGetIntegerv (GL_MINOR_VERSION, &minor);

        if (major != 0)
        {
            const OpenGLVersion integerVersion { major, minor };

            if (integerVersion != stringVersion)
                DBG ("OpenGL version queries disagree: string " << stringVersion.major << "." << stringVersion.minor
                     << ", integers " << integerVersion.major << "." << integerVersion.minor);

            return integerVersion;
        }
    }

    return stringVersion;
}

static OpenGLProfile getOpenGLProfile()
{
    if (auto* context = OpenGLContext::getCurrentContext())
        if (const auto info = context->getContextInfo())
            return info->profile;

    const auto version = getOpenGLVersion();

   #if ! JUCE_OPENGL_ES
    const auto isES = OpenGLHelpers::isOpenGLES();
    GLint flags = 0, profileMask = 0;

    if (! isES && version > OpenGLVersion { 2, 1 } && version <= OpenGLVersion { 3, 0 })
        glGetIntegerv (GL_CONTEXT_FLAGS, &flags);

    if (! isES && version > OpenGLVersion { 3, 1 })
        glGetIntegerv (GL_CONTEXT_PROFILE_MASK, &profileMask);

    return detail::determineOpenGLProfile (
        isES ? OpenGLAPI::openGLES : OpenGLAPI::openGL,
        version,
        (flags & (int) GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) != 0,
        ! isES && version > OpenGLVersion { 3, 0 } && version <= OpenGLVersion { 3, 1 }
            && OpenGLHelpers::isExtensionSupported ("GL_ARB_compatibility"),
        (profileMask & (int) GL_CONTEXT_CORE_PROFILE_BIT) != 0);
   #else
    return detail::determineOpenGLProfile (OpenGLAPI::openGLES, version, false, false, true);
   #endif
}

#if JUCE_ANDROID || JUCE_LINUX || JUCE_BSD

struct EGLHelpers
{
    EGLHelpers() = delete;

    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wzero-as-null-pointer-constant")
    static constexpr EGLContext nullContext = EGL_NO_CONTEXT;
    static constexpr EGLDisplay nullDisplay = EGL_NO_DISPLAY;
    static constexpr EGLSurface nullSurface = EGL_NO_SURFACE;
    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

    template <typename Traits>
    class ScopedEGLObject
    {
    public:
        using Type = typename Traits::Type;

        ScopedEGLObject() = default;

        ScopedEGLObject (Type obj, EGLDisplay d)
            : object (obj), display (d) {}

        ScopedEGLObject (ScopedEGLObject&& other) noexcept
            : object  (std::exchange (other.object, Type{})),
              display (std::exchange (other.display, nullDisplay)) {}

        ScopedEGLObject& operator= (ScopedEGLObject&& other) noexcept
        {
            ScopedEGLObject { std::move (other) }.swap (*this);
            return *this;
        }

        ~ScopedEGLObject() noexcept
        {
            if (object != Traits::null)
            {
                jassert (display != nullDisplay);
                Traits::destroy(display, object);
            }
        }

        Type get() const { return object; }

        void reset() noexcept
        {
            *this = ScopedEGLObject();
        }

        void swap (ScopedEGLObject& other) noexcept
        {
            std::swap (other.object,  object);
            std::swap (other.display, display);
        }

        bool operator== (std::nullptr_t) const
        {
            return object == Traits::null;
        }

        bool operator!= (std::nullptr_t other) const
        {
            return ! operator== (other);
        }

    private:
        Type object = Traits::null;
        EGLDisplay display = nullDisplay;
    };

    struct TraitsEGLContext
    {
        using Type = EGLContext;
        static constexpr auto null = nullContext;

        static void destroy (EGLDisplay display, Type t)
        {
            eglDestroyContext (display, t);
        }
    };

    struct TraitsEGLSurface
    {
        using Type = EGLSurface;
        static constexpr auto null = nullSurface;

        static void destroy (EGLDisplay display, Type t)
        {
            eglDestroySurface (display, t);
        }
    };

    using PtrEGLContext = ScopedEGLObject<TraitsEGLContext>;
    using PtrEGLSurface = ScopedEGLObject<TraitsEGLSurface>;

    static PtrEGLContext initEGLContext (OpenGLAPI api,
                                         Span<const OpenGLVersion> versionsToRequest,
                                         [[maybe_unused]] OpenGLProfile profile,
                                         EGLDisplay display,
                                         EGLConfig config,
                                         EGLContext contextToShareWith,
                                         bool mayFallBackToDefault,
                                         OpenGLVersion& acceptedVersion)
    {
        [[maybe_unused]] const auto didBind = eglBindAPI (api == OpenGLAPI::openGL ? EGL_OPENGL_API : EGL_OPENGL_ES_API);
        // Failed to bind the requested OpenGL API
        jassert (didBind);

        if (! didBind)
        {
            DBG ("eglBindAPI failed for "
                 << (api == OpenGLAPI::openGL ? "OpenGL" : "OpenGL ES")
                 << ", error " << eglGetError());
            return {};
        }

        EGLint renderableType{};
        [[maybe_unused]] const auto didGet = eglGetConfigAttrib (display, config, EGL_RENDERABLE_TYPE, &renderableType);
        // Failed to query the supported renderable types
        jassert (didGet);

        if (! didGet)
        {
            DBG ("eglGetConfigAttrib failed for EGL_RENDERABLE_TYPE, error " << eglGetError());
            return {};
        }

        constexpr bool addDebugOption[]
        {
           #if JUCE_DEBUG
            true,
           #endif
            false,
        };

        for (const auto& versionToTry : versionsToRequest)
        {
            if (versionToTry == OpenGLVersion{})
                continue;

            if (! detail::canTryEGLVersion (api,
                                            versionToTry,
                                            (renderableType & EGL_OPENGL_ES3_BIT) != 0,
                                            (renderableType & EGL_OPENGL_ES2_BIT) != 0))
            {
                DBG ("EGL config does not support "
                     << (api == OpenGLAPI::openGL ? "OpenGL " : "OpenGL ES ")
                     << versionToTry.major << "." << versionToTry.minor);
                continue;
            }

            for (const auto& shouldDebug : addDebugOption)
            {
                std::vector<EGLint> attribs;

                if (versionToTry != OpenGLVersion{})
                {
                    attribs.insert (attribs.end(),
                    {
                        EGL_CONTEXT_MAJOR_VERSION, versionToTry.major,
                        EGL_CONTEXT_MINOR_VERSION, versionToTry.minor,
                    });
                }

                if (shouldDebug)
                {
                    attribs.insert (attribs.end(), { EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE });
                }

               #if ! JUCE_OPENGL_ES
                if (api == OpenGLAPI::openGL)
                {
                   #if JUCE_DEBUG
                    constexpr EGLint contextFlags = EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
                   #else
                    constexpr EGLint contextFlags = 0;
                   #endif

                    attribs.insert (attribs.end(),
                    {
                        EGL_CONTEXT_FLAGS_KHR, contextFlags,
                        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,
                        profile == OpenGLProfile::core ? EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR
                                                       : EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR,
                    });
                }
               #endif

                attribs.push_back (EGL_NONE);

                if (PtrEGLContext renderContext { eglCreateContext (display, config, contextToShareWith, attribs.data()), display };
                    renderContext != nullptr)
                {
                    DBG ("eglCreateContext succeeded for "
                         << (api == OpenGLAPI::openGL ? "OpenGL " : "OpenGL ES ")
                         << versionToTry.major << "." << versionToTry.minor
                         << (shouldDebug ? " with debug attributes" : ""));
                    acceptedVersion = versionToTry;
                    return renderContext;
                }

                DBG ("eglCreateContext failed for "
                     << (api == OpenGLAPI::openGL ? "OpenGL " : "OpenGL ES ")
                     << versionToTry.major << "." << versionToTry.minor
                     << (shouldDebug ? " with debug attributes" : "")
                     << ", error " << eglGetError());
            }
        }

        if (mayFallBackToDefault)
        {
            PtrEGLContext renderContext { eglCreateContext (display, config, contextToShareWith, nullptr), display };

            if (renderContext != nullptr)
            {
                DBG ("eglCreateContext succeeded for the default OpenGL context");
                acceptedVersion = {};
            }
            else
            {
                DBG ("eglCreateContext failed for the default OpenGL context, error " << eglGetError());
            }

            return renderContext;
        }

        return {};
    }
};

#endif

void OpenGLHelpers::resetErrorState()
{
    // GL implementations retain one flag per error category, so the total number of errors
    // will be small, but we need to guard against infinite loops in some situations like
    // when the context has been lost.
    for (int i = 0; i < 16; ++i)
        if (glGetError() == GL_NO_ERROR)
            break;
}

void* OpenGLHelpers::getExtensionFunction (const char* functionName)
{
   #if JUCE_WINDOWS
    return (void*) wglGetProcAddress (functionName);
   #elif JUCE_LINUX || JUCE_BSD
    if (auto* function = (void*) eglGetProcAddress (functionName))
        return function;

    static DynamicLibrary desktopLibrary { "libGL.so.1" };
    static DynamicLibrary esLibrary { "libGLESv2.so.2" };
    return (isOpenGLES() ? esLibrary : desktopLibrary).getFunction (functionName);
   #else
    static void* handle = dlopen (nullptr, RTLD_LAZY);
    return dlsym (handle, functionName);
   #endif
}

bool OpenGLHelpers::isExtensionSupported (const char* const extensionName)
{
    jassert (extensionName != nullptr); // you must supply a genuine string for this
    jassert (isContextActive()); // an OpenGL context will need to be active before calling this

    if (getOpenGLVersion().major >= 3)
    {
        using GetStringi = const GLubyte* (*) (GLenum, GLuint);

        if (auto* thisGlGetStringi = reinterpret_cast<GetStringi> (getExtensionFunction ("glGetStringi")))
        {
            GLint n = 0;
            glGetIntegerv (GL_NUM_EXTENSIONS, &n);

            for (auto i = (decltype (n)) 0; i < n; ++i)
                if (StringRef (extensionName) == StringRef ((const char*) thisGlGetStringi (GL_EXTENSIONS, (GLuint) i)))
                    return true;

            return false;
        }
    }

    const char* extensions = (const char*) glGetString (GL_EXTENSIONS);
    jassert (extensions != nullptr); // Perhaps you didn't activate an OpenGL context before calling this?

    for (;;)
    {
        const char* found = strstr (extensions, extensionName);

        if (found == nullptr)
            break;

        extensions = found + strlen (extensionName);

        if (extensions[0] == ' ' || extensions[0] == 0)
            return true;
    }

    return false;
}

void OpenGLHelpers::clear (Colour colour)
{
    glClearColor (colour.getFloatRed(), colour.getFloatGreen(),
                  colour.getFloatBlue(), colour.getFloatAlpha());

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLHelpers::enableScissorTest (Rectangle<int> clip)
{
    glEnable (GL_SCISSOR_TEST);
    glScissor (clip.getX(), clip.getY(), clip.getWidth(), clip.getHeight());
}

String OpenGLHelpers::getGLSLVersionString()
{
    return detail::getGLSLVersionString (isOpenGLES() ? OpenGLAPI::openGLES : OpenGLAPI::openGL,
                                         getOpenGLVersion());
}

String OpenGLHelpers::translateVertexShaderToV3 (const String& code)
{
    if (getOpenGLVersion() >= OpenGLVersion (3, 2))
    {
        String output;

        if (isOpenGLES())
        {
            int numAttributes = 0;

            for (int p = code.indexOf (0, "attribute "); p >= 0; p = code.indexOf (p + 1, "attribute "))
                numAttributes++;

            int last = 0;

            for (int p = code.indexOf (0, "attribute "); p >= 0; p = code.indexOf (p + 1, "attribute "))
            {
                output += code.substring (last, p) + "layout(location=" + String (--numAttributes) + ") in ";

                last = p + 10;
            }

            output += code.substring (last);
        }
        else
        {
            output = code.replace ("attribute", "in");
        }

        return getGLSLVersionString() + "\n" + output.replace ("varying", "out");
    }

    return code;
}

String OpenGLHelpers::translateFragmentShaderToV3 (const String& code)
{
    if (getOpenGLVersion() >= OpenGLVersion (3, 2))
        return getGLSLVersionString() + "\n"
               "out mediump vec4 fragColor;\n"
                + code.replace ("varying", "in")
                      .replace ("texture2D", "texture")
                      .replace ("gl_FragColor", "fragColor");

    return code;
}

String OpenGLHelpers::translatePrecisionPlaceholders (const String& code)
{
    return preprocessShaderPrecisionStatements (code);
}

} // namespace juce
