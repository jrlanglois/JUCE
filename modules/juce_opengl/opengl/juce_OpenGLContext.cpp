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

#include <juce_gui_basics/detail/juce_ComponentHelpers.h>

#if JUCE_MAC
 #include <juce_gui_basics/native/juce_PerScreenDisplayLinks_mac.h>
#endif

namespace juce
{

#if JUCE_IOS
struct AppInactivityCallback // NB: this is a duplicate of an internal declaration in juce_core
{
    virtual ~AppInactivityCallback() {}
    virtual void appBecomingInactive() = 0;
};

extern Array<AppInactivityCallback*> appBecomingInactiveCallbacks;

// On iOS, all GL calls will crash when the app is running in the background, so
// this prevents them from happening (which some messy locking behaviour)
struct iOSBackgroundProcessCheck final : public AppInactivityCallback
{
    iOSBackgroundProcessCheck()              { isBackgroundProcess(); appBecomingInactiveCallbacks.add (this); }
    ~iOSBackgroundProcessCheck() override    { appBecomingInactiveCallbacks.removeAllInstancesOf (this); }

    bool isBackgroundProcess()
    {
        const bool b = Process::isForegroundProcess();
        isForeground.set (b ? 1 : 0);
        return ! b;
    }

    void appBecomingInactive() override
    {
        int counter = 2000;

        while (--counter > 0 && isForeground.get() != 0)
            Thread::sleep (1);
    }

private:
    Atomic<int> isForeground;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (iOSBackgroundProcessCheck)
};

#endif

static auto tieVersion (const OpenGLVersion& v)
{
    return std::tuple (v.major, v.minor);
}

bool OpenGLVersion::operator== (const OpenGLVersion& other) const { return tieVersion (*this) == tieVersion (other); }
bool OpenGLVersion::operator!= (const OpenGLVersion& other) const { return tieVersion (*this) != tieVersion (other); }
bool OpenGLVersion::operator<  (const OpenGLVersion& other) const { return tieVersion (*this) <  tieVersion (other); }
bool OpenGLVersion::operator<= (const OpenGLVersion& other) const { return tieVersion (*this) <= tieVersion (other); }
bool OpenGLVersion::operator>  (const OpenGLVersion& other) const { return tieVersion (*this) >  tieVersion (other); }
bool OpenGLVersion::operator>= (const OpenGLVersion& other) const { return tieVersion (*this) >= tieVersion (other); }

static auto tieRequest (const OpenGLContextRequest& request)
{
    return std::tuple (request.api, request.version.major, request.version.minor, request.profile);
}

bool OpenGLContextRequest::operator== (const OpenGLContextRequest& other) const
{
    return tieRequest (*this) == tieRequest (other);
}

bool OpenGLContextRequest::operator!= (const OpenGLContextRequest& other) const
{
    return tieRequest (*this) != tieRequest (other);
}

static bool contextHasTextureNpotFeature()
{
    if (getOpenGLVersion() >= OpenGLVersion (2))
        return true;

    // If the version is < 2, we can't use the newer extension-checking API
    // so we have to use glGetString
    const auto* extensionsBegin = glGetString (GL_EXTENSIONS);

    if (extensionsBegin == nullptr)
        return false;

    const auto* extensionsEnd = findNullTerminator (extensionsBegin);
    const std::string extensionsString (extensionsBegin, extensionsEnd);
    const auto stringTokens = StringArray::fromTokens (extensionsString.c_str(), false);
    return stringTokens.contains ("GL_ARB_texture_non_power_of_two");
}

namespace detail
{
struct PlatformLimits
{
    bool canCreateOpenGL = false;
    bool canCreateOpenGLES = false;
    std::vector<OpenGLVersion> desktopCoreVersions;
    std::vector<OpenGLVersion> desktopCompatibilityVersions;
    std::vector<OpenGLVersion> openGLESVersions;
};

struct PreparedContextRequest
{
    OpenGLContextRequest requested;
    OpenGLContextRequest normalised;
    std::vector<OpenGLVersion> versions;
};

static bool isDefaultVersion (OpenGLVersion version)
{
    return version == OpenGLVersion{};
}

static PlatformLimits getPlatformLimits()
{
    const std::vector<OpenGLVersion> desktopCore
    {
        { 3, 2 }, { 3, 3 }, { 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 4, 6 }
    };

    const std::vector<OpenGLVersion> desktopCompatibility
    {
        { 2, 0 }, { 2, 1 }, { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 },
        { 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 4, 6 }
    };

    const std::vector<OpenGLVersion> openGLES
    {
        { 2, 0 }, { 3, 0 }, { 3, 1 }, { 3, 2 }
    };

   #if JUCE_MAC
    return { true, false, { { 3, 2 }, { 4, 1 } }, { { 2, 1 } }, {} };
   #elif JUCE_IOS
    return { false, true, {}, {}, { { 2, 0 }, { 3, 0 } } };
   #elif JUCE_OPENGL_ES
    return { false, true, {}, {}, openGLES };
   #elif JUCE_WINDOWS
    return { true, false, desktopCore, desktopCompatibility, {} };
   #elif JUCE_LINUX || JUCE_BSD
    return { true, true, desktopCore, desktopCompatibility, openGLES };
   #else
    return {};
   #endif
}

static bool canCreate (const PlatformLimits& limits, OpenGLAPI api)
{
    return api == OpenGLAPI::openGL ? limits.canCreateOpenGL
                                    : limits.canCreateOpenGLES;
}

static const std::vector<OpenGLVersion>& getAvailableVersions (const PlatformLimits& limits,
                                                               OpenGLAPI api,
                                                               OpenGLProfile profile)
{
    if (api == OpenGLAPI::openGLES)
        return limits.openGLESVersions;

    return profile == OpenGLProfile::core ? limits.desktopCoreVersions
                                          : limits.desktopCompatibilityVersions;
}

static OpenGLVersion snapAndClampVersion (OpenGLVersion requested,
                                          const std::vector<OpenGLVersion>& available)
{
    jassert (! available.empty());

    if (requested < available.front())
        return available.front();

    OpenGLVersion result = available.front();

    for (const auto candidate : available)
    {
        if (requested < candidate)
            break;

        result = candidate;
    }

    return result;
}

static std::vector<OpenGLVersion> getVersionList (const OpenGLContextRequest& request,
                                                  const PlatformLimits& limits)
{
    if (isDefaultVersion (request.version))
    {
        if (request.api == OpenGLAPI::openGL)
            return { {} };

        const auto& available = limits.openGLESVersions;
        std::vector<OpenGLVersion> result;

        for (const auto candidate : { OpenGLVersion { 3, 0 }, OpenGLVersion { 2, 0 } })
            if (std::find (available.begin(), available.end(), candidate) != available.end())
                result.push_back (candidate);

        return result;
    }

    const auto& available = getAvailableVersions (limits, request.api, request.profile);
    const auto first = snapAndClampVersion (request.version, available);
    std::vector<OpenGLVersion> result;

    for (auto iterator = available.rbegin(); iterator != available.rend(); ++iterator)
        if (*iterator <= first)
            result.push_back (*iterator);

    return result;
}

static std::optional<PreparedContextRequest> prepareContextRequest (OpenGLContextRequest requested,
                                                                    const PlatformLimits& limits)
{
    if (! canCreate (limits, requested.api))
        return {};

    auto normalised = requested;
    normalised.profile = normalised.api == OpenGLAPI::openGLES || ! isDefaultVersion (normalised.version)
                       ? normalised.profile
                       : OpenGLProfile::compatibility;

    if (normalised.api == OpenGLAPI::openGLES)
        normalised.profile = OpenGLProfile::core;

    const auto& available = getAvailableVersions (limits, normalised.api, normalised.profile);

    if (available.empty())
        return {};

    if (! isDefaultVersion (normalised.version))
        normalised.version = snapAndClampVersion (normalised.version, available);

    return PreparedContextRequest { requested, normalised, getVersionList (normalised, limits) };
}

static bool hasSamePreparedRequest (const PreparedContextRequest& a,
                                    const PreparedContextRequest& b)
{
    return a.normalised.api == b.normalised.api
        && a.normalised.profile == b.normalised.profile
        && a.versions == b.versions;
}

static std::vector<PreparedContextRequest> buildContextRequestChain (
    const std::vector<OpenGLContextRequest>& listed,
    OpenGLContextRequest preferred,
    bool hasRenderer,
    const PlatformLimits& limits)
{
    std::vector<OpenGLContextRequest> candidates;

    if (! listed.empty())
    {
        candidates = listed;
    }
    else
    {
        if (limits.canCreateOpenGL != limits.canCreateOpenGLES)
            preferred.api = limits.canCreateOpenGL ? OpenGLAPI::openGL : OpenGLAPI::openGLES;

        candidates.push_back (preferred);

        if (! isDefaultVersion (preferred.version))
        {
            candidates.push_back ({ preferred.api,
                                    {},
                                    preferred.api == OpenGLAPI::openGL ? OpenGLProfile::compatibility
                                                                       : OpenGLProfile::core });
        }

        if (! hasRenderer && limits.canCreateOpenGL && limits.canCreateOpenGLES)
        {
            const auto fallbackAPI = preferred.api == OpenGLAPI::openGL ? OpenGLAPI::openGLES
                                                                        : OpenGLAPI::openGL;
            candidates.push_back ({ fallbackAPI,
                                    {},
                                    fallbackAPI == OpenGLAPI::openGL ? OpenGLProfile::compatibility
                                                                      : OpenGLProfile::core });
        }
    }

    std::vector<PreparedContextRequest> result;

    for (const auto candidate : candidates)
    {
        const auto prepared = prepareContextRequest (candidate, limits);

        if (prepared == std::nullopt)
            continue;

        const auto duplicate = std::find_if (result.begin(), result.end(), [&] (const auto& existing)
        {
            return hasSamePreparedRequest (existing, *prepared);
        });

        if (duplicate == result.end())
            result.push_back (*prepared);
    }

    return result;
}

static bool contextMatchesRequest (const OpenGLContextRequest& request,
                                   OpenGLVersion acceptedVersion,
                                   OpenGLAPI detectedAPI,
                                   OpenGLVersion detectedVersion,
                                   OpenGLProfile detectedProfile)
{
    if (detectedAPI != request.api || detectedVersion < acceptedVersion)
        return false;

    return detectedAPI != OpenGLAPI::openGL
        || acceptedVersion == OpenGLVersion{}
        || detectedProfile == request.profile;
}
} // namespace detail

//==============================================================================
class OpenGLContext::CachedImage final : public CachedComponentImage,
                                         private AsyncUpdater
{
    template <typename T, typename U>
    static constexpr bool isFlagSet (const T& t, const U& u) { return (t & u) != 0; }

    struct AreaAndScale
    {
        Rectangle<int> area;
        double scale;

        auto tie() const { return std::tie (area, scale); }

        auto operator== (const AreaAndScale& other) const { return tie() == other.tie(); }
        auto operator!= (const AreaAndScale& other) const { return tie() != other.tie(); }
    };

    class LockedAreaAndScale
    {
    public:
        auto get() const
        {
            const ScopedLock lock (mutex);
            return data;
        }

        template <typename Fn>
        void set (const AreaAndScale& d, Fn&& ifDifferent)
        {
            const auto old = [&]
            {
                const ScopedLock lock (mutex);
                return std::exchange (data, d);
            }();

            if (old != d)
                ifDifferent();
        }

    private:
        CriticalSection mutex;
        AreaAndScale data { {}, 1.0 };
    };

public:
    CachedImage (OpenGLContext& c, Component& comp,
                 const OpenGLPixelFormat& pixFormat, void* contextToShare)
        : context (c),
          component (comp)
    {
        const auto preferred = ContextRequest { c.preferredAPI, c.preferredVersion, c.preferredProfile };
        const auto chain = detail::buildContextRequestChain (c.contextRequests,
                                                             preferred,
                                                             c.renderer != nullptr,
                                                             detail::getPlatformLimits());

        for (const auto& entry : chain)
        {
            nativeContext.reset();
            nativeContext.reset (new NativeContext (component,
                                                    pixFormat,
                                                    contextToShare,
                                                    c.useMultisampling,
                                                    entry.normalised.api,
                                                    entry.versions,
                                                    entry.normalised.profile,
                                                    entry.normalised.api == OpenGLAPI::openGL
                                                        && entry.normalised.version == Version{}));

            DBG ("OpenGL context entry: API " << (entry.normalised.api == OpenGLAPI::openGL ? "OpenGL" : "OpenGL ES")
                 << ", profile " << (entry.normalised.profile == OpenGLProfile::core ? "core" : "compatibility")
                 << (nativeContext->createdOk() ? " succeeded" : " failed"));

            if (! nativeContext->createdOk())
                continue;

            requestedContext = entry.requested;
            normalisedContext = entry.normalised;
            acceptedVersion = nativeContext->getAcceptedVersion();
            context.nativeContext = nativeContext.get();
            break;
        }

        if (context.nativeContext == nullptr)
            nativeContext.reset();

        refreshDisplayLinkConnection();
    }

    ~CachedImage() override
    {
        stop();
        cancelPendingUpdate();
    }

    //==============================================================================
    void start()
    {
        if (nativeContext != nullptr)
            resume();
    }

    void stop()
    {
        // make sure everything has finished executing
        state |= StateFlags::pendingDestruction;

        if (workQueue.size() > 0)
        {
            if (! renderThread->contains (this))
                resume();

            while (workQueue.size() != 0)
                Thread::sleep (20);
        }

        pause();
    }

    //==============================================================================
    void pause()
    {
        renderThread->remove (this);

        if ((state.fetch_and (~StateFlags::initialised) & StateFlags::initialised) == 0)
            return;

        ScopedContextActivator activator;
        activator.activate (context);

       #if JUCE_ANDROID
        context.nativeContextListeners.call ([] (auto& l) { l.contextWillPause(); });
       #endif

        if (context.renderer != nullptr)
            context.renderer->openGLContextClosing();

        {
            const SpinLock::ScopedLockType lock (context.contextInfoLock);
            context.contextInfo.reset();
        }

        associatedObjectNames.clear();
        associatedObjects.clear();
        cachedImageFrameBuffer.release();
        nativeContext->shutdownOnRenderThread();
    }

    void resume()
    {
        renderThread->add (this);
    }

    void handleAsyncUpdate() override
    {
        const auto info = context.getContextInfo();
        const auto callback = context.onCreated;

        if (info != std::nullopt && callback != nullptr)
            callback (*info);
    }

    //==============================================================================
    void paint (Graphics&) override
    {
        if (MessageManager::getInstance()->isThisTheMessageThread())
        {
            updateViewportSize();
        }
        else
        {
            // If you hit this assertion, it's because paint has been called from a thread other
            // than the message thread. This commonly happens when nesting OpenGL contexts, because
            // the 'outer' OpenGL renderer will attempt to call paint on the 'inner' context's
            // component from the OpenGL thread.
            // Nesting OpenGL contexts is not directly supported, however there is a workaround:
            // https://forum.juce.com/t/opengl-how-do-3d-with-custom-shaders-and-2d-with-juce-paint-methods-work-together/28026/7
            jassertfalse;
        }
    }

    bool invalidateAll() override
    {
        validArea.clear();
        triggerRepaint();
        return false;
    }

    bool invalidate (const Rectangle<int>& area) override
    {
        validArea.subtract (area.toFloat().transformedBy (transform).getSmallestIntegerContainer());
        triggerRepaint();
        return false;
    }

    void releaseResources() override
    {
        stop();
    }

    void triggerRepaint()
    {
        state |= (StateFlags::pendingRender | StateFlags::paintComponents);
        renderThread->triggerRepaint();
    }

    //==============================================================================
    bool ensureFrameBufferSize (Rectangle<int> viewportArea)
    {
        JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

        auto fbW = cachedImageFrameBuffer.getWidth();
        auto fbH = cachedImageFrameBuffer.getHeight();

        if (fbW != viewportArea.getWidth() || fbH != viewportArea.getHeight() || ! cachedImageFrameBuffer.isValid())
        {
            if (! cachedImageFrameBuffer.initialise (context, viewportArea.getWidth(), viewportArea.getHeight()))
                return false;

            validArea.clear();
            JUCE_CHECK_OPENGL_ERROR
        }

        return true;
    }

    void clearRegionInFrameBuffer (const RectangleList<int>& list)
    {
        glClearColor (0, 0, 0, 0);
        glEnable (GL_SCISSOR_TEST);

        auto previousFrameBufferTarget = OpenGLFrameBuffer::getCurrentFrameBufferTarget();
        cachedImageFrameBuffer.makeCurrentRenderingTarget();
        const auto textureH = cachedImageFrameBuffer.getTextureHeight();

        for (auto& r : list)
        {
            glScissor (r.getX(), textureH - r.getBottom(), r.getWidth(), r.getHeight());
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        glDisable (GL_SCISSOR_TEST);
        context.extensions.glBindFramebuffer (GL_FRAMEBUFFER, previousFrameBufferTarget);
        JUCE_CHECK_OPENGL_ERROR
    }

    struct ScopedContextActivator
    {
        bool activate (OpenGLContext& ctx)
        {
            if (! active)
                active = ctx.makeActive();

            return active;
        }

        ~ScopedContextActivator()
        {
            if (active)
                deactivateCurrentContext();
        }

    private:
        bool active = false;
    };

    enum class RenderStatus
    {
        nominal,
        messageThreadAborted,
        noWork,
    };

    RenderStatus renderFrame (MessageManager::Lock& mmLock)
    {
        ScopedContextActivator contextActivator;

        if (! isFlagSet (state, StateFlags::initialised))
        {
            switch (initialiseOnThread (contextActivator))
            {
                case InitResult::fatal:
                case InitResult::retry: return RenderStatus::noWork;
                case InitResult::success: break;
            }
        }

        state |= StateFlags::initialised;

       #if JUCE_IOS
        if (backgroundProcessCheck.isBackgroundProcess())
            return RenderStatus::noWork;
       #endif

        std::optional<MessageManager::Lock::ScopedTryLockType> scopedLock;

        const auto stateToUse = state.fetch_and (StateFlags::persistent);

       #if JUCE_MAC
        // On macOS, we use a display link callback to trigger repaints, rather than
        // letting them run at full throttle
        const auto noAutomaticRepaint = true;
       #else
        const auto noAutomaticRepaint = ! context.continuousRepaint;
       #endif

        if (! isFlagSet (stateToUse, StateFlags::pendingRender) && noAutomaticRepaint)
            return RenderStatus::noWork;

        const auto isUpdating = isFlagSet (stateToUse, StateFlags::paintComponents);

        if (context.renderComponents && isUpdating)
        {
            bool abortScope = false;
            // If we early-exit here, we need to restore these flags so that the render is
            // attempted again in the next time slice.
            const ScopeGuard scope { [&] { if (! abortScope) state |= stateToUse; } };

            // This avoids hogging the message thread when doing intensive rendering.
            std::this_thread::sleep_until (lastMMLockReleaseTime + std::chrono::milliseconds { 2 });

            if (renderThread->isListChanging())
                return RenderStatus::messageThreadAborted;

            doWorkWhileWaitingForLock (contextActivator);

            scopedLock.emplace (mmLock);

            // If we can't get the lock here, it's probably because a context has been removed
            // on the main thread.
            // We return, just in case this renderer needs to be removed from the rendering thread.
            // If another renderer is being removed instead, then we should be able to get the lock
            // next time round.
            if (! scopedLock->isLocked())
                return RenderStatus::messageThreadAborted;

            abortScope = true;
        }

        {
            NativeContext::Locker locker (*nativeContext);

            if (! contextActivator.activate (context))
                return RenderStatus::noWork;

            JUCE_CHECK_OPENGL_ERROR

            doWorkWhileWaitingForLock (contextActivator);

            const auto currentAreaAndScale = areaAndScale.get();
            const auto viewportArea = currentAreaAndScale.area;

            if (context.renderer != nullptr)
            {
                OpenGLRendering::SavedBinding<OpenGLRendering::TraitsVAO> vaoBinding;

                glViewport (0, 0, viewportArea.getWidth(), viewportArea.getHeight());
                context.currentRenderScale = currentAreaAndScale.scale;
                context.renderer->renderOpenGL();
                clearDebugGLError();
            }

            if (context.renderComponents)
            {
                if (isUpdating)
                {
                    paintComponent (currentAreaAndScale);

                    if (! isFlagSet (state, StateFlags::initialised))
                        return RenderStatus::noWork;

                    scopedLock.reset();
                    lastMMLockReleaseTime = std::chrono::steady_clock::now();
                }

                glViewport (0, 0, viewportArea.getWidth(), viewportArea.getHeight());
                drawComponentBuffer();
            }
        }

        bufferSwapper.swap();
        return RenderStatus::nominal;
    }

    void updateViewportSize()
    {
        JUCE_ASSERT_MESSAGE_THREAD

        if ([[maybe_unused]] auto* peer = component.getPeer())
        {
            const auto localBounds = component.getLocalBounds();
            const auto logicalArea = component.getScreenBounds();

           #if JUCE_MAC
            const auto globalArea = detail::ScalingHelpers::scaledScreenPosToUnscaled (component, logicalArea);

            updateScreen();

            const auto displayScale = std::invoke ([this]
            {
                if (auto* view = getCurrentView())
                {
                    if ([view respondsToSelector: @selector (backingScaleFactor)])
                        return [(id) view backingScaleFactor];

                    if (auto* window = [view window])
                        return [window backingScaleFactor];
                }

                return areaAndScale.get().scale;
            });

            const auto newArea = globalArea.withZeroOrigin() * displayScale;
           #elif JUCE_WINDOWS || JUCE_LINUX || JUCE_BSD
            const auto globalArea = detail::ScalingHelpers::scaledScreenPosToUnscaled (component, logicalArea);
            const auto newArea = (globalArea.toFloat() * peer->getPlatformScaleFactor()).withZeroOrigin().toNearestInt();
           #elif JUCE_IOS || JUCE_ANDROID
            auto& desktop = Desktop::getInstance();
            const auto& displays = desktop.getDisplays();
            const auto physicalTopLeft = displays.logicalToPhysical (logicalArea.getTopLeft().toFloat());
            const auto physicalBottomRight = displays.logicalToPhysical (logicalArea.getBottomRight().toFloat());
            const auto newArea = Rectangle { physicalTopLeft, physicalBottomRight }.withZeroOrigin().toNearestInt();
           #endif

            // On Windows some hosts (Pro Tools 2022.7) do not take the current DPI into account
            // when sizing plugin editor windows.
            //
            // Also in plugins on Windows, the plugin HWND's DPI settings generally don't reflect
            // the desktop scaling setting and Displays::Display::scale will return an incorrect 1.0
            // value. Our plugin wrappers will use a combination of querying the plugin HWND's
            // parent HWND (the host HWND), and utilising the scale factor reported by the host
            // through the plugin API. This scale is then added as a transformation to the
            // AudioProcessorEditor.
            //
            // Hence, instead of querying the OS for the DPI of the editor window,
            // we approximate based on the physical size of the window that was actually provided
            // for the context to draw into. This may break if the OpenGL context's component is
            // scaled differently in its width and height - but in this case, a single scale factor
            // isn't that helpful anyway.
            const auto newScale = (float) newArea.getWidth() / (float) localBounds.getWidth();

            areaAndScale.set ({ newArea, newScale }, [&]
            {
                // Transform is only accessed when the message manager is locked
                transform = AffineTransform::scale ((float) newArea.getWidth()  / (float) localBounds.getWidth(),
                                                    (float) newArea.getHeight() / (float) localBounds.getHeight());

                if (nativeContext != nullptr)
                    nativeContext->updateWindowPosition();

                invalidateAll();
            });
        }
    }

    void checkViewportBounds()
    {
        auto screenBounds = component.getTopLevelComponent()->getScreenBounds();

        if (lastScreenBounds != screenBounds)
        {
            updateViewportSize();
            lastScreenBounds = screenBounds;
        }
    }

    void paintComponent (const AreaAndScale& currentAreaAndScale)
    {
        JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

        // you mustn't set your own cached image object when attaching a GL context!
        jassert (get (component) == this);

        if (! ensureFrameBufferSize (currentAreaAndScale.area))
            return;

        RectangleList<int> invalid (currentAreaAndScale.area);
        invalid.subtract (validArea);
        validArea = currentAreaAndScale.area;

        if (! invalid.isEmpty())
        {
            clearRegionInFrameBuffer (invalid);

            {
                std::unique_ptr<LowLevelGraphicsContext> g (createOpenGLGraphicsContext (context, cachedImageFrameBuffer));
                g->clipToRectangleList (invalid);
                g->addTransform (transform);

                paintOwner (*g);
                JUCE_CHECK_OPENGL_ERROR
            }
        }

        JUCE_CHECK_OPENGL_ERROR
    }

    void drawComponentBuffer()
    {
        if (context.getProfile() == OpenGLProfile::compatibility)
            glEnable (GL_TEXTURE_2D);

        // some old drivers are missing this function, so try to at least avoid a crash here,
        // but if you hit this assertion you may want to have your own version check before using the
        // component rendering stuff on such old drivers.
        jassert (OpenGLHelpers::isFunctionAvailable (context.extensions.glActiveTexture));
        if (OpenGLHelpers::isFunctionAvailable (context.extensions.glActiveTexture))
            context.extensions.glActiveTexture (GL_TEXTURE0);

        glBindTexture (GL_TEXTURE_2D, cachedImageFrameBuffer.getTextureID());

        const Rectangle<int> cacheBounds (cachedImageFrameBuffer.getWidth(), cachedImageFrameBuffer.getHeight());
        const Rectangle<int> textureBounds (cachedImageFrameBuffer.getTextureWidth(), cachedImageFrameBuffer.getTextureHeight());
        context.copyTexture (cacheBounds, textureBounds, cacheBounds.getWidth(), cacheBounds.getHeight(), false);
        glBindTexture (GL_TEXTURE_2D, 0);
        JUCE_CHECK_OPENGL_ERROR
    }

    void paintOwner (LowLevelGraphicsContext& llgc)
    {
        Graphics g (llgc);

      #if JUCE_ENABLE_REPAINT_DEBUGGING
       #ifdef JUCE_IS_REPAINT_DEBUGGING_ACTIVE
        if (JUCE_IS_REPAINT_DEBUGGING_ACTIVE)
       #endif
        {
            g.saveState();
        }
       #endif

        JUCE_TRY
        {
            component.paintEntireComponent (g, false);
        }
        JUCE_CATCH_EXCEPTION

      #if JUCE_ENABLE_REPAINT_DEBUGGING
       #ifdef JUCE_IS_REPAINT_DEBUGGING_ACTIVE
        if (JUCE_IS_REPAINT_DEBUGGING_ACTIVE)
       #endif
        {
            // enabling this code will fill all areas that get repainted with a colour overlay, to show
            // clearly when things are being repainted.
            g.restoreState();

            static Random rng;
            g.fillAll (Colour ((uint8) rng.nextInt (255),
                               (uint8) rng.nextInt (255),
                               (uint8) rng.nextInt (255),
                               (uint8) 0x50));
        }
       #endif
    }

    void handleResize()
    {
        updateViewportSize();

       #if JUCE_MAC
        if (isFlagSet (state, StateFlags::initialised))
        {
            [nativeContext->view update];

            // We're already on the message thread, no need to lock it again.
            MessageManager::Lock mml;
            renderFrame (mml);
        }
       #endif
    }

    //==============================================================================
    InitResult initialiseOnThread (ScopedContextActivator& activator)
    {
        activator.activate (context);

        associatedObjectNames.clear();
        associatedObjects.clear();
        cachedImageFrameBuffer.release();

        if (const auto nativeResult = nativeContext->initialiseOnRenderThread (context); nativeResult != InitResult::success)
            return nativeResult;

       #if JUCE_ANDROID
        // On android the context may be created in initialiseOnRenderThread
        // and we therefore need to call makeActive again
        context.makeActive();
       #endif

        gl::loadFunctions();

       #if JUCE_DEBUG && ! JUCE_DISABLE_ASSERTIONS
        if (getOpenGLVersion() >= Version { 4, 3 } && OpenGLHelpers::isFunctionAvailable (glDebugMessageCallback))
        {
            glEnable (GL_DEBUG_OUTPUT);
            glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback ([] (GLenum, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*)
            {
                // This may reiterate issues that are also flagged by JUCE_CHECK_OPENGL_ERROR.
                // The advantage of this callback is that it will catch *all* errors, even if we
                // forget to check manually.
                DBG ("OpenGL DBG message: " << message);
                jassert (type != GL_DEBUG_TYPE_ERROR && severity != GL_DEBUG_SEVERITY_HIGH);
            }, nullptr);
        }
       #endif

        const auto currentViewportArea = areaAndScale.get().area;
        glViewport (0, 0, currentViewportArea.getWidth(), currentViewportArea.getHeight());

        nativeContext->setSwapInterval (1);

        acceptedVersion = nativeContext->getAcceptedVersion();

        const auto detectedAPI = OpenGLHelpers::isOpenGLES() ? OpenGLAPI::openGLES : OpenGLAPI::openGL;
        const auto detectedVersion = getOpenGLVersion();
        const auto detectedProfile = getOpenGLProfile();
        const auto matchesRequest = detail::contextMatchesRequest (normalisedContext,
                                                                   acceptedVersion,
                                                                   detectedAPI,
                                                                   detectedVersion,
                                                                   detectedProfile);
        const ContextInfo info { requestedContext,
                                 acceptedVersion,
                                 detectedAPI,
                                 detectedVersion,
                                 detectedProfile,
                                 matchesRequest };

        if (! matchesRequest)
        {
            DBG ("Created OpenGL context does not match its accepted request: API "
                 << (detectedAPI == OpenGLAPI::openGL ? "OpenGL" : "OpenGL ES")
                 << ", version " << detectedVersion.major << "." << detectedVersion.minor
                 << ", profile " << (detectedProfile == OpenGLProfile::core ? "core" : "compatibility"));
        }

        {
            const SpinLock::ScopedLockType lock (context.contextInfoLock);
            context.contextInfo = info;
        }

        if (detectedAPI == OpenGLAPI::openGL)
        {
            JUCE_CHECK_OPENGL_ERROR
            shadersAvailable = OpenGLShaderProgram::getLanguageVersion() > 0;
            OpenGLHelpers::resetErrorState();
        }
        else
        {
            shadersAvailable = true;
        }

        textureNpotSupported = contextHasTextureNpotFeature();

        const auto isES = detectedAPI == OpenGLAPI::openGLES;
        const auto pixelsAreBGRA = (GLenum) JUCE_RGBA_FORMAT == (GLenum) GL_BGRA_EXT;
        const auto mustAskForBGRA = isES && pixelsAreBGRA;
        const auto textureFormats = detail::chooseTextureFormats (
            isES,
            pixelsAreBGRA,
            mustAskForBGRA && OpenGLHelpers::isExtensionSupported ("GL_APPLE_texture_format_BGRA8888"),
            mustAskForBGRA && OpenGLHelpers::isExtensionSupported ("GL_EXT_texture_format_BGRA8888"),
            mustAskForBGRA && OpenGLHelpers::isExtensionSupported ("GL_EXT_read_format_bgra"));
        context.textureUploadInternalFormat = textureFormats.uploadInternalFormat;
        context.textureUploadFormat = textureFormats.uploadFormat;
        context.textureReadFormat = textureFormats.readFormat;
        context.textureUploadNeedsRedBlueSwap = textureFormats.swapRedAndBlueOnUpload;
        context.textureReadNeedsRedBlueSwap = textureFormats.swapRedAndBlueAfterRead;

        if (context.renderer != nullptr)
            context.renderer->newOpenGLContextCreated();

        triggerAsyncUpdate();

       #if JUCE_ANDROID
        context.nativeContextListeners.call ([] (auto& l) { l.contextDidResume(); });
       #endif

        return InitResult::success;
    }

    //==============================================================================
    struct BlockingWorker final : public AsyncWorker
    {
        BlockingWorker (Ptr && workerToUse)
            : originalWorker (std::move (workerToUse))
        {}

        void operator() (OpenGLContext& calleeContext) override
        {
            if (originalWorker != nullptr)
                (*originalWorker) (calleeContext);

            finishedSignal.signal();
        }

        void block() noexcept  { finishedSignal.wait(); }

        Ptr originalWorker;
        WaitableEvent finishedSignal;
    };

    void doWorkWhileWaitingForLock (ScopedContextActivator& contextActivator)
    {
        while (const auto work = workQueue.removeAndReturn (0))
        {
            if (renderThread->isListChanging() || ! contextActivator.activate (context))
                break;

            NativeContext::Locker locker (*nativeContext);

            (*work) (context);
            clearDebugGLError();
        }
    }

    void execute (AsyncWorker::Ptr workerToUse, bool shouldBlock)
    {
        if (! isFlagSet (state, StateFlags::pendingDestruction))
        {
            if (shouldBlock)
            {
                auto blocker = new BlockingWorker (std::move (workerToUse));
                AsyncWorker::Ptr worker (*blocker);
                workQueue.add (worker);

                renderThread->abortLock();
                context.triggerRepaint();

                blocker->block();
            }
            else
            {
                workQueue.add (std::move (workerToUse));

                renderThread->abortLock();
                context.triggerRepaint();
            }
        }
        else
        {
            jassertfalse; // you called execute AFTER you detached your OpenGLContext
        }
    }

    //==============================================================================
    static CachedImage* get (Component& c) noexcept
    {
        return dynamic_cast<CachedImage*> (c.getCachedComponentImage());
    }

    bool hasContextCreationFailed() const
    {
       #if JUCE_ANDROID
        return nativeContext != nullptr && nativeContext->hasContextCreationFailed();
       #else
        return false;
       #endif
    }

    class RenderThread
    {
    public:
        RenderThread() = default;

        ~RenderThread()
        {
            flags.setDestructing();
            thread.join();
        }

        void add (CachedImage* x)
        {
            const std::scoped_lock lock { listMutex };
            images.push_back (x);
        }

        void remove (CachedImage* x)
        {
            JUCE_ASSERT_MESSAGE_THREAD;

            flags.setSafe (false);
            abortLock();

            {
                const std::scoped_lock lock { callbackMutex, listMutex };
                images.remove (x);
            }

            flags.setSafe (true);
        }

        bool contains (CachedImage* x)
        {
            const std::scoped_lock lock { listMutex };
            return std::find (images.cbegin(), images.cend(), x) != images.cend();
        }

        void triggerRepaint()   { flags.setRenderRequested(); }

        void abortLock()        { messageManagerLock.abort(); }

        bool isListChanging()   { return ! flags.isSafe(); }

    private:
        RenderStatus renderAll()
        {
            auto result = RenderStatus::noWork;

            const std::scoped_lock lock { callbackMutex, listMutex };

            for (auto* x : images)
            {
                listMutex.unlock();
                const ScopeGuard scope { [&] { listMutex.lock(); } };

                const auto status = x->renderFrame (messageManagerLock);

                switch (status)
                {
                    case RenderStatus::noWork: break;
                    case RenderStatus::nominal: result = RenderStatus::nominal; break;
                    case RenderStatus::messageThreadAborted: return RenderStatus::messageThreadAborted;
                }
            }

            return result;
        }

        /*  Allows the main thread to communicate changes to the render thread.

            When the render thread needs to change in some way (asked to resume rendering,
            a renderer is added/removed, or the thread needs to stop prior to destruction),
            the main thread can set the appropriate flag on this structure. The render thread
            will call waitForWork() repeatedly, pausing when the render thread has no work to do,
            and resuming when requested by the main thread.
        */
        class Flags
        {
        public:
            void setDestructing()       { update ([] (auto& f) { f |= destructorCalled; }); }
            void setRenderRequested()   { update ([] (auto& f) { f |= renderRequested;  }); }

            void setSafe (const bool safe)
            {
                update ([safe] (auto& f)
                {
                    if (safe)
                        f |= listSafe;
                    else
                        f &= ~listSafe;
                });
            }

            bool isSafe()
            {
                const std::scoped_lock lock { mutex };
                return (flags & listSafe) != 0;
            }

            /*  Blocks until the 'safe' flag is set, and at least one other flag is set.
                After returning, the renderRequested flag will be unset.
                Returns true if rendering should continue.
            */
            bool waitForWork (bool requestRender)
            {
                std::unique_lock lock { mutex };
                flags |= (requestRender ? renderRequested : 0);
                condvar.wait (lock, [this] { return flags > listSafe; });
                flags &= ~renderRequested;
                return ((flags & destructorCalled) == 0);
            }

        private:
            template <typename Fn>
            void update (Fn fn)
            {
                {
                    const std::scoped_lock lock { mutex };
                    fn (flags);
                }

                condvar.notify_one();
            }

            enum
            {
                renderRequested  = 1 << 0,
                destructorCalled = 1 << 1,
                listSafe         = 1 << 2
            };

            std::mutex mutex;
            std::condition_variable condvar;
            int flags = listSafe;
        };

        MessageManager::Lock messageManagerLock;
        std::mutex listMutex, callbackMutex;
        std::list<CachedImage*> images;
        Flags flags;

        std::thread thread { [this]
        {
            Thread::setCurrentThreadName ("OpenGL Renderer");
            while (flags.waitForWork (renderAll() != RenderStatus::noWork)) {}
        } };
    };

    void refreshDisplayLinkConnection()
    {
       #if JUCE_MAC
        if (context.continuousRepaint)
        {
            connection.emplace (sharedDisplayLinks->registerFactory ([this] (CGDirectDisplayID display)
            {
                return [this, display] (double)
                {
                    if (display == lastDisplay)
                        triggerRepaint();
                };
            }));
        }
        else
        {
            connection.reset();
        }
       #endif
    }

    //==============================================================================
    class BufferSwapper final : private AsyncUpdater
    {
    public:
        explicit BufferSwapper (CachedImage& img)
            : image (img) {}

        ~BufferSwapper() override
        {
            cancelPendingUpdate();
        }

        void swap()
        {
            static const auto swapBuffersOnMainThread = []
            {
                const auto os = SystemStats::getOperatingSystemType();

                if ((os & SystemStats::MacOSX) != 0)
                    return (os != SystemStats::MacOSX && os < SystemStats::MacOSX_10_14);

                return false;
            }();

            if (swapBuffersOnMainThread && ! MessageManager::getInstance()->isThisTheMessageThread())
                triggerAsyncUpdate();
            else
                image.nativeContext->swapBuffers();
        }

    private:
        void handleAsyncUpdate() override
        {
            ScopedContextActivator activator;
            activator.activate (image.context);

            NativeContext::Locker locker (*image.nativeContext);
            image.nativeContext->swapBuffers();
        }

        CachedImage& image;
    };

    //==============================================================================
    friend class NativeContext;
    std::unique_ptr<NativeContext> nativeContext;
    ContextRequest requestedContext;
    ContextRequest normalisedContext;
    Version acceptedVersion;

    OpenGLContext& context;
    Component& component;

    SharedResourcePointer<RenderThread> renderThread;

    OpenGLFrameBuffer cachedImageFrameBuffer;
    RectangleList<int> validArea;
    Rectangle<int> lastScreenBounds;
    AffineTransform transform;
    LockedAreaAndScale areaAndScale;

    StringArray associatedObjectNames;
    ReferenceCountedArray<ReferenceCountedObject> associatedObjects;

    WaitableEvent canPaintNowFlag, finishedPaintingFlag;
    bool shadersAvailable = false;
    bool textureNpotSupported = false;
    std::chrono::steady_clock::time_point lastMMLockReleaseTime{};
    BufferSwapper bufferSwapper { *this };

   #if JUCE_MAC
    NSView* getCurrentView() const
    {
        JUCE_ASSERT_MESSAGE_THREAD;

        if (auto* peer = component.getPeer())
            return static_cast<NSView*> (peer->getNativeHandle());

        return nullptr;
    }

    NSWindow* getCurrentWindow() const
    {
        JUCE_ASSERT_MESSAGE_THREAD;

        if (auto* view = getCurrentView())
            return [view window];

        return nullptr;
    }

    NSScreen* getCurrentScreen() const
    {
        JUCE_ASSERT_MESSAGE_THREAD;

        if (auto* window = getCurrentWindow())
            return [window screen];

        return nullptr;
    }

    void updateScreen()
    {
        const auto screen = getCurrentScreen();
        const auto display = ScopedDisplayLink::getDisplayIdForScreen (screen);

        if (lastDisplay.exchange (display) == display)
            return;

        const auto newRefreshPeriod = sharedDisplayLinks->getNominalVideoRefreshPeriodSForScreen (display);

        if (newRefreshPeriod != 0.0 && ! approximatelyEqual (std::exchange (refreshPeriod, newRefreshPeriod), newRefreshPeriod))
            nativeContext->setNominalVideoRefreshPeriodS (newRefreshPeriod);

        updateColourSpace();
    }

    void updateColourSpace()
    {
        if (auto* view = nativeContext->getNSView())
            if (auto* window = [view window])
                [window setColorSpace: [NSColorSpace sRGBColorSpace]];
    }

    std::atomic<CGDirectDisplayID> lastDisplay { 0 };
    double refreshPeriod = 0.0;

    FunctionNotificationCenterObserver observer { NSWindowDidChangeScreenNotification,
                                                  getCurrentWindow(),
                                                  [this] { updateScreen(); } };

    // Note: the NSViewComponentPeer also has a SharedResourcePointer<PerScreenDisplayLinks> to
    // avoid unnecessarily duplicating display-link threads.
    SharedResourcePointer<PerScreenDisplayLinks> sharedDisplayLinks;

    // On macOS, rather than letting swapBuffers block as appropriate, we use a display link
    // callback to mark the view as needing to repaint.
    std::optional<PerScreenDisplayLinks::Connection> connection;
   #endif

    enum StateFlags
    {
        pendingRender           = 1 << 0,
        paintComponents         = 1 << 1,
        pendingDestruction      = 1 << 2,
        initialised             = 1 << 3,

        // Flags that should retain their state after each frame
        persistent              = initialised | pendingDestruction
    };

    std::atomic<int> state { 0 };
    ReferenceCountedArray<AsyncWorker, CriticalSection> workQueue;

   #if JUCE_IOS
    iOSBackgroundProcessCheck backgroundProcessCheck;
   #endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CachedImage)
};

//==============================================================================
class OpenGLContext::Attachment final : public ComponentMovementWatcher,
                                        private Timer,
                                        private AsyncUpdater
{
public:
    Attachment (OpenGLContext& c, Component& comp)
       : ComponentMovementWatcher (&comp), context (c)
    {
        if (canBeAttached (comp))
            attach();
    }

    ~Attachment() override
    {
        detach();
    }

    void detach()
    {
        cancelPendingUpdate();
        failureLatched = false;

        auto& comp = *getComponent();
        stop();
        detail::ComponentHelpers::releaseAllCachedImageResources (comp);
        comp.setCachedComponentImage (nullptr);
        context.clearNativeContext();
    }

    void componentMovedOrResized (bool /*wasMoved*/, bool /*wasResized*/) override
    {
        auto& comp = *getComponent();

        if (isAttached (comp) != canBeAttached (comp))
            componentVisibilityChanged();

        if (comp.getWidth() > 0 && comp.getHeight() > 0
             && context.nativeContext != nullptr)
        {
            if (auto* c = CachedImage::get (comp))
                c->handleResize();

            if (auto* native = context.nativeContext)
                native->updateWindowPosition();
        }
    }

    using ComponentMovementWatcher::componentMovedOrResized;

    void componentPeerChanged() override
    {
        detach();
        componentVisibilityChanged();
    }

    void componentVisibilityChanged() override
    {
        auto& comp = *getComponent();

        if (canBeAttached (comp))
        {
            if (isAttached (comp))
                comp.repaint(); // (needed when windows are un-minimised)
            else
                attach();
        }
        else
        {
            detach();
        }
    }

    using ComponentMovementWatcher::componentVisibilityChanged;

   #if JUCE_DEBUG || JUCE_LOG_ASSERTIONS
    void componentBeingDeleted (Component& c) override
    {
        /* You must call detach() or delete your OpenGLContext to remove it
           from a component BEFORE deleting the component that it is using!
        */
        jassertfalse;

        ComponentMovementWatcher::componentBeingDeleted (c);
    }
   #endif

private:
    OpenGLContext& context;

    bool canBeAttached (const Component& comp) noexcept
    {
        return (! context.overrideCanAttach) && comp.getWidth() > 0 && comp.getHeight() > 0 && isShowingOrMinimised (comp);
    }

    static bool isShowingOrMinimised (const Component& c)
    {
        if (! c.isVisible())
            return false;

        if (auto* p = c.getParentComponent())
            return isShowingOrMinimised (*p);

        return c.getPeer() != nullptr;
    }

    bool isAttached (const Component& comp) const noexcept
    {
        return failureLatched || comp.getCachedComponentImage() != nullptr;
    }

    void attach()
    {
        auto& comp = *getComponent();
        auto newCachedImage = std::make_unique<CachedImage> (context,
                                                            comp,
                                                            context.openGLPixelFormat,
                                                            context.contextToShareWith);

        if (context.nativeContext == nullptr)
        {
            failureLatched = true;
            triggerAsyncUpdate();
            return;
        }

        comp.setCachedComponentImage (newCachedImage.release());

        start();
    }

    void handleAsyncUpdate() override
    {
        const auto callback = context.onCreationFailed;

        if (callback != nullptr)
            callback();
    }

    void stop()
    {
        stopTimer();

        auto& comp = *getComponent();

       #if JUCE_MAC
        #if ! JUCE_MAC_API_VERSION_MIN_REQUIRED_AT_LEAST (15, 0)
        // According to a warning triggered on macOS 15 and above this doesn't do anything!
        [[(NSView*) comp.getWindowHandle() window] disableScreenUpdatesUntilFlush];
        #endif
       #endif

        if (auto* oldCachedImage = CachedImage::get (comp))
            oldCachedImage->stop(); // (must stop this before detaching it from the component)
    }

    void start()
    {
        auto& comp = *getComponent();

        if (auto* cachedImage = CachedImage::get (comp))
        {
            cachedImage->start(); // (must wait until this is attached before starting its thread)
            cachedImage->updateViewportSize();

            startTimer (400);
        }
    }

    bool failureLatched = false;

    void timerCallback() override
    {
        if (auto* cachedImage = CachedImage::get (*getComponent()))
        {
            if (cachedImage->hasContextCreationFailed())
            {
                handleContextCreationFailure();
                return;
            }

            cachedImage->checkViewportBounds();
        }
    }

    void handleContextCreationFailure()
    {
        auto& comp = *getComponent();
        stop();
        detail::ComponentHelpers::releaseAllCachedImageResources (comp);
        comp.setCachedComponentImage (nullptr);
        context.clearNativeContext();
        failureLatched = true;
        triggerAsyncUpdate();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Attachment)
};

//==============================================================================
OpenGLContext::OpenGLContext()
{
}

OpenGLContext::~OpenGLContext()
{
    detach();
}

void OpenGLContext::setRenderer (OpenGLRenderer* rendererToUse) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    renderer = rendererToUse;
}

void OpenGLContext::setComponentPaintingEnabled (bool shouldPaintComponent) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    renderComponents = shouldPaintComponent;
}

void OpenGLContext::setContinuousRepainting (bool shouldContinuouslyRepaint) noexcept
{
    continuousRepaint = shouldContinuouslyRepaint;

   #if JUCE_MAC
    if (auto* component = getTargetComponent())
    {
        detach();
        attachment.reset (new Attachment (*this, *component));
    }

    if (auto* cachedImage = getCachedImage())
        cachedImage->refreshDisplayLinkConnection();
   #endif

    triggerRepaint();
}

void OpenGLContext::setPixelFormat (const OpenGLPixelFormat& preferredPixelFormat) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    openGLPixelFormat = preferredPixelFormat;
}

void OpenGLContext::setTextureMagnificationFilter (OpenGLContext::TextureMagnificationFilter magFilterMode) noexcept
{
    texMagFilter = magFilterMode;
}

void OpenGLContext::setNativeSharedContext (void* nativeContextToShareWith) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    contextToShareWith = nativeContextToShareWith;
}

void OpenGLContext::setMultisamplingEnabled (bool b) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    useMultisampling = b;
}

void OpenGLContext::setOpenGLVersionRequired (OpenGLVersion v) noexcept
{
    setPreferredVersion (std::invoke ([&]() -> Version
    {
        switch (v)
        {
            case defaultGLVersion: return {};
            case openGL3_2: return { 3, 2 };
            case openGL4_1: return { 4, 1 };
            case openGL4_3: return { 4, 3 };
        }

        return {};
    }));

    setPreferredProfile (std::invoke ([&]() -> Profile
    {
        switch (v)
        {
            case defaultGLVersion: return OpenGLProfile::compatibility;
            case openGL3_2: return OpenGLProfile::core;
            case openGL4_1: return OpenGLProfile::core;
            case openGL4_3: return OpenGLProfile::core;
        }

        return OpenGLProfile::core;
    }));
}

void OpenGLContext::setPreferredVersion (const Version& v)
{
    preferredVersion = v;
}

auto OpenGLContext::getPreferredVersion() const -> Version
{
    return preferredVersion;
}

auto OpenGLContext::getVersion() const -> Version
{
    if (const auto info = getContextInfo())
        return info->version;

    return {};
}

void OpenGLContext::setPreferredAPI (API x)
{
    preferredAPI = x;
}

auto OpenGLContext::getPreferredAPI() const -> API
{
    return preferredAPI;
}

auto OpenGLContext::getAPI() const -> API
{
    if (const auto info = getContextInfo())
        return info->api;

    return API::openGL;
}

void OpenGLContext::setPreferredProfile (Profile x)
{
    preferredProfile = x;
}

auto OpenGLContext::getPreferredProfile() const -> Profile
{
    return preferredProfile;
}

auto OpenGLContext::getProfile() const -> Profile
{
    if (const auto info = getContextInfo())
        return info->profile;

    return Profile::compatibility;
}

void OpenGLContext::setContextRequests (std::vector<ContextRequest> requests)
{
    jassert (nativeContext == nullptr);
    contextRequests = std::move (requests);
}

std::vector<OpenGLContext::ContextRequest> OpenGLContext::getContextRequests() const
{
    return contextRequests;
}

std::optional<OpenGLContext::ContextInfo> OpenGLContext::getContextInfo() const
{
    const SpinLock::ScopedLockType lock (contextInfoLock);
    return contextInfo;
}

void OpenGLContext::attachTo (Component& component)
{
    component.repaint();

    if (getTargetComponent() != &component)
    {
        detach();
        attachment.reset (new Attachment (*this, component));
    }
}

void OpenGLContext::detach()
{
    if (auto* a = attachment.get())
    {
        a->detach(); // must detach before nulling our pointer
        attachment.reset();
    }

    clearNativeContext();
}

bool OpenGLContext::isAttached() const noexcept
{
    return nativeContext != nullptr;
}

Component* OpenGLContext::getTargetComponent() const noexcept
{
    return attachment != nullptr ? attachment->getComponent() : nullptr;
}

OpenGLContext* OpenGLContext::getContextAttachedTo (Component& c) noexcept
{
    if (auto* ci = CachedImage::get (c))
        return &(ci->context);

    return nullptr;
}

thread_local OpenGLContext* currentThreadActiveContext = nullptr;

OpenGLContext* OpenGLContext::getCurrentContext()
{
    return currentThreadActiveContext;
}

bool OpenGLContext::makeActive() const noexcept
{
    auto& current = currentThreadActiveContext;

    if (nativeContext != nullptr && nativeContext->makeActive())
    {
        current = const_cast<OpenGLContext*> (this);
        return true;
    }

    current = nullptr;
    return false;
}

bool OpenGLContext::isActive() const noexcept
{
    return nativeContext != nullptr && nativeContext->isActive();
}

void OpenGLContext::deactivateCurrentContext()
{
    NativeContext::deactivateCurrentContext();
    currentThreadActiveContext = nullptr;
}

void OpenGLContext::triggerRepaint()
{
    if (auto* cachedImage = getCachedImage())
        cachedImage->triggerRepaint();
}

void OpenGLContext::swapBuffers()
{
    if (nativeContext != nullptr)
        nativeContext->swapBuffers();
}

unsigned int OpenGLContext::getFrameBufferID() const noexcept
{
    return nativeContext != nullptr ? nativeContext->getFrameBufferID() : 0;
}

bool OpenGLContext::setSwapInterval (int numFramesPerSwap)
{
    return nativeContext != nullptr && nativeContext->setSwapInterval (numFramesPerSwap);
}

int OpenGLContext::getSwapInterval() const
{
    return nativeContext != nullptr ? nativeContext->getSwapInterval() : 0;
}

void* OpenGLContext::getRawContext() const noexcept
{
    return nativeContext != nullptr ? nativeContext->getRawContext() : nullptr;
}

bool OpenGLContext::isCoreProfile() const
{
    return getProfile() == OpenGLProfile::core;
}

OpenGLContext::CachedImage* OpenGLContext::getCachedImage() const noexcept
{
    if (auto* comp = getTargetComponent())
        return CachedImage::get (*comp);

    return nullptr;
}

bool OpenGLContext::areShadersAvailable() const
{
    auto* c = getCachedImage();
    return c != nullptr && c->shadersAvailable;
}

bool OpenGLContext::isTextureNpotSupported() const
{
    auto* c = getCachedImage();
    return c != nullptr && c->textureNpotSupported;
}

ReferenceCountedObject* OpenGLContext::getAssociatedObject (const char* name) const
{
    jassert (name != nullptr);

    auto* c = getCachedImage();

    // This method must only be called from an openGL rendering callback.
    jassert (c != nullptr && nativeContext != nullptr);
    jassert (getCurrentContext() != nullptr);

    auto index = c->associatedObjectNames.indexOf (name);
    return index >= 0 ? c->associatedObjects.getUnchecked (index).get() : nullptr;
}

void OpenGLContext::setAssociatedObject (const char* name, ReferenceCountedObject* newObject)
{
    jassert (name != nullptr);

    if (auto* c = getCachedImage())
    {
        // This method must only be called from an openGL rendering callback.
        jassert (nativeContext != nullptr);
        jassert (getCurrentContext() != nullptr);

        const int index = c->associatedObjectNames.indexOf (name);

        if (index >= 0)
        {
            if (newObject != nullptr)
            {
                c->associatedObjects.set (index, newObject);
            }
            else
            {
                c->associatedObjectNames.remove (index);
                c->associatedObjects.remove (index);
            }
        }
        else if (newObject != nullptr)
        {
            c->associatedObjectNames.add (name);
            c->associatedObjects.add (newObject);
        }
    }
}

void OpenGLContext::setImageCacheSize (size_t newSize) noexcept     { imageCacheMaxSize = newSize; }
size_t OpenGLContext::getImageCacheSize() const noexcept            { return imageCacheMaxSize; }

void OpenGLContext::execute (OpenGLContext::AsyncWorker::Ptr workerToUse, bool shouldBlock)
{
    if (auto* c = getCachedImage())
        c->execute (std::move (workerToUse), shouldBlock);
    else
        jassertfalse; // You must have attached the context to a component
}

//==============================================================================
struct DepthTestDisabler
{
    DepthTestDisabler() noexcept
    {
        glGetBooleanv (GL_DEPTH_TEST, &wasEnabled);

        if (wasEnabled)
            glDisable (GL_DEPTH_TEST);
    }

    ~DepthTestDisabler() noexcept
    {
        if (wasEnabled)
            glEnable (GL_DEPTH_TEST);
    }

    GLboolean wasEnabled;
};

//==============================================================================
void OpenGLContext::copyTexture (const Rectangle<int>& targetClipArea,
                                 const Rectangle<int>& anchorPosAndTextureSize,
                                 const int contextWidth, const int contextHeight,
                                 bool flippedVertically,
                                 bool blend)
{
    if (contextWidth <= 0 || contextHeight <= 0)
        return;

    JUCE_CHECK_OPENGL_ERROR
    if (blend)
    {
        glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glEnable (GL_BLEND);
    }
    else
    {
        glDisable (GL_BLEND);
    }

    DepthTestDisabler depthDisabler;

    if (areShadersAvailable())
    {
        OpenGLRendering::SavedBinding<OpenGLRendering::TraitsVAO> vaoBinding;

        struct OverlayShaderProgram final : public ReferenceCountedObject
        {
            explicit OverlayShaderProgram (OpenGLContext& context)
                : program (context), params (program)
            {}

            static const OverlayShaderProgram& select (OpenGLContext& context)
            {
                static const char programValueID[] = "juceGLComponentOverlayShader";
                OverlayShaderProgram* program = static_cast<OverlayShaderProgram*> (context.getAssociatedObject (programValueID));

                if (program == nullptr)
                {
                    program = new OverlayShaderProgram (context);
                    context.setAssociatedObject (programValueID, program);
                }

                program->program.use();
                return *program;
            }

            struct BuiltProgram final : public OpenGLShaderProgram
            {
                explicit BuiltProgram (OpenGLContext& ctx)
                    : OpenGLShaderProgram (ctx)
                {
                    addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (preprocessShaderPrecisionStatements (
                        "attribute #highp# vec2 position;"
                        "uniform #highp# vec2 screenSize;"
                        "uniform #highp# float textureBounds[4];"
                        "uniform #highp# vec2 vOffsetAndScale;"
                        "varying #highp# vec2 texturePos;"
                        "void main()"
                        "{"
                          "#highp# vec2 scaled = position / (0.5 * screenSize.xy);"
                          "gl_Position = vec4 (scaled.x - 1.0, 1.0 - scaled.y, 0, 1.0);"
                          "texturePos = (position - vec2 (textureBounds[0], textureBounds[1])) / vec2 (textureBounds[2], textureBounds[3]);"
                          "texturePos = vec2 (texturePos.x, vOffsetAndScale.x + vOffsetAndScale.y * texturePos.y);"
                        "}")));

                    addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (preprocessShaderPrecisionStatements (
                        "uniform sampler2D imageTexture;"
                        "varying #highp# vec2 texturePos;"
                        "void main()"
                        "{"
                          "gl_FragColor = texture2D (imageTexture, texturePos);"
                        "}")));

                    link();
                }
            };

            struct Params
            {
                explicit Params (OpenGLShaderProgram& prog)
                    : positionAttribute (prog, "position"),
                      screenSize (prog, "screenSize"),
                      imageTexture (prog, "imageTexture"),
                      textureBounds (prog, "textureBounds"),
                      vOffsetAndScale (prog, "vOffsetAndScale")
                {}

                void set (const float targetWidth, const float targetHeight, const Rectangle<float>& bounds, bool flipVertically) const
                {
                    const GLfloat m[] = { bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight() };
                    textureBounds.set (m, 4);
                    imageTexture.set (0);
                    screenSize.set (targetWidth, targetHeight);

                    vOffsetAndScale.set (flipVertically ? 0.0f : 1.0f,
                                         flipVertically ? 1.0f : -1.0f);
                }

                OpenGLShaderProgram::Attribute positionAttribute;
                OpenGLShaderProgram::Uniform screenSize, imageTexture, textureBounds, vOffsetAndScale;
            };

            BuiltProgram program;
            Params params;
        };

        auto left   = (GLshort) targetClipArea.getX();
        auto top    = (GLshort) targetClipArea.getY();
        auto right  = (GLshort) targetClipArea.getRight();
        auto bottom = (GLshort) targetClipArea.getBottom();
        const GLshort vertices[] = { left, bottom, right, bottom, left, top, right, top };

        GLint oldProgram{};
        glGetIntegerv (GL_CURRENT_PROGRAM, &oldProgram);

        const ScopeGuard bindPreviousProgram { [&] { extensions.glUseProgram ((GLuint) oldProgram); } };

        auto& program = OverlayShaderProgram::select (*this);
        program.params.set ((float) contextWidth, (float) contextHeight, anchorPosAndTextureSize.toFloat(), flippedVertically);

        OpenGLRendering::SavedBinding<OpenGLRendering::TraitsArrayBuffer> savedArrayBuffer;
        extensions.glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

        auto index = (GLuint) program.params.positionAttribute.attributeID;
        extensions.glVertexAttribPointer (index, 2, GL_SHORT, GL_FALSE, 4, nullptr);
        extensions.glEnableVertexAttribArray (index);
        JUCE_CHECK_OPENGL_ERROR

        if (extensions.glCheckFramebufferStatus (GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        {
            glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
            extensions.glDisableVertexAttribArray (index);
        }
        else
        {
            OpenGLHelpers::resetErrorState();
        }
    }
    else
    {
        jassert (attachment == nullptr); // Running on an old graphics card!
    }

    JUCE_CHECK_OPENGL_ERROR
}

void OpenGLContext::clearNativeContext()
{
    nativeContextListeners.call ([] (auto& l) { l.contextWillBeDestroyed(); });
    nativeContextListeners.clear();
    nativeContext = nullptr;

    const SpinLock::ScopedLockType lock (contextInfoLock);
    contextInfo.reset();
}

#if JUCE_ANDROID

void OpenGLContext::NativeContext::surfaceCreated (LocalRef<jobject> holder)
{
    {
        const std::lock_guard lock { nativeHandleMutex };

        jassert (hasInitialised);

        // has the context already attached?
        jassert (surface.get() == EGL_NO_SURFACE && context.get() == EGL_NO_CONTEXT);

        const auto window = getNativeWindowFromSurfaceHolder (holder);

        if (window == nullptr)
        {
            contextCreationFailed = true;
            DBG ("Failed to get an Android native window for the OpenGL surface");
        }
        else
        {
            // Reset the surface (only one window surface may be alive at a time)
            context.reset();
            surface.reset();
            acceptedVersion = {};
            contextCreationFailed = false;

            // Create the surface
            surface = { eglCreateWindowSurface (display, config, window.get(), nullptr), display };

            if (surface == nullptr)
            {
                contextCreationFailed = true;
                DBG ("eglCreateWindowSurface failed on Android, error " << eglGetError());
            }
            else
            {
                context = EGLHelpers::initEGLContext (api,
                                                      versions,
                                                      profile,
                                                      display,
                                                      config,
                                                      EGL_NO_CONTEXT,
                                                      true,
                                                      acceptedVersion);
                contextCreationFailed = context == nullptr;
            }
        }
    }

    if (auto* cached = CachedImage::get (component))
    {
        cached->resume();
        cached->triggerRepaint();
    }
}

void OpenGLContext::NativeContext::surfaceDestroyed (LocalRef<jobject>)
{
    if (auto* cached = CachedImage::get (component))
        cached->pause();

    {
        const std::lock_guard lock { nativeHandleMutex };

        context.reset();
        surface.reset();
        contextCreationFailed = false;
    }
}

#endif

} // namespace juce
