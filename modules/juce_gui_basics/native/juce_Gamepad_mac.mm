<<<<<<< HEAD:modules/juce_gui_basics/native/juce_Gamepad_mac.mm
/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

/**
    GamepadInputSource gis (0);

    if (gis.isConnected())
    {
        Logger::writeToLog (String ("ID: ")                     + String (gis.getID()));
        Logger::writeToLog (String ("Battery Type: ")           + String ((int) gis.getBatteryType()));
        Logger::writeToLog (String ("Battery Level: ")          + String (gis.getBatteryLevel()));
        Logger::writeToLog (String ("Type: ")                   + String ((int) gis.getType()));
        Logger::writeToLog (String ("Subtype: ")                + String ((int) gis.getSubtype()));
        Logger::writeToLog (String ("Is wireless? ")            + booleanToString (gis.isWireless(), true));
        Logger::writeToLog (String ("Is virtual? ")             + booleanToString (gis.isVirtual(), true));
        Logger::writeToLog (String ("Supports orientation? ")   + booleanToString (gis.supportsOrientation(), true));
        Logger::writeToLog (String ("Supports motion? ")        + booleanToString (gis.supportsMotion(), true));
        Logger::writeToLog (String ("Has light? ")              + booleanToString (gis.hasLight(), true));
        Logger::writeToLog (String ("Has accelerometer? ")      + booleanToString (gis.hasAccelerometer(), true));
        Logger::writeToLog (String ("Has gyroscope? ")          + booleanToString (gis.hasGyroscope(), true));
    }

    while (gis.isConnected())
    {
        auto addVibesIfPossible = [&] (Point<double> axis, Point<double> deadzone, bool isLeft)
        {
            const auto mag = getMagnitude (axis);

            if (mag > getMagnitude (deadzone))
            {
                if (isLeft)
                  gis.setLeftMotorVibration (1.0 - mag);
                else
                  gis.setRightMotorVibration (1.0 - mag);
            }
        };

        addVibesIfPossible (gis.getLeftAxis(), gis.getLeftAxisDeadZone(), true);
        addVibesIfPossible (gis.getRightAxis(), gis.getRightAxisDeadZone(), false);

        using DirectionalPadButton = GamepadInputSource::DirectionalPadButton;
        auto logIfDirectionalPadButtonIsDown = [&] (DirectionalPadButton b, const String& name)
        {
            if (gis.isDirectionalPadButtonDown (b))
                Logger::writeToLog ("DirectionalPad: " + name);
        };

        logIfDirectionalPadButtonIsDown (DirectionalPadButton::up,      "Up");
        logIfDirectionalPadButtonIsDown (DirectionalPadButton::right,   "Right");
        logIfDirectionalPadButtonIsDown (DirectionalPadButton::down,    "Down");
        logIfDirectionalPadButtonIsDown (DirectionalPadButton::left,    "Left");

        using ActionButton = GamepadInputSource::ActionButton;
        auto logIfActionButtonIsDown = [&] (ActionButton b, const String& name)
        {
            if (gis.isActionButtonDown (b))
                Logger::writeToLog ("ActionButton: " + name);
        };

        logIfActionButtonIsDown (ActionButton::up,      "Up");
        logIfActionButtonIsDown (ActionButton::right,   "Right");
        logIfActionButtonIsDown (ActionButton::down,    "Down");
        logIfActionButtonIsDown (ActionButton::left,    "Left");

        using ShoulderButton = GamepadInputSource::ShoulderButton;
        auto logIfShoulderButtonIsDown = [&] (ShoulderButton b, const String& name)
        {
            if (gis.isShoulderButtonDown (b))
                Logger::writeToLog ("ShoulderButton: " + name);
        };

        logIfShoulderButtonIsDown (ShoulderButton::leftTrigger,     "leftTrigger");
        logIfShoulderButtonIsDown (ShoulderButton::leftBumper,      "leftBumper");
        logIfShoulderButtonIsDown (ShoulderButton::leftAnalog,      "leftAnalog");
        logIfShoulderButtonIsDown (ShoulderButton::rightTrigger,    "rightTrigger");
        logIfShoulderButtonIsDown (ShoulderButton::rightBumper,     "rightBumper");
        logIfShoulderButtonIsDown (ShoulderButton::rightAnalog,     "rightAnalog");

        using SpecialButton = GamepadInputSource::SpecialButton;
        auto logIfSpecialButtonIsDown = [&] (SpecialButton b, const String& name)
        {
            if (gis.isSpecialButtonDown (b))
                Logger::writeToLog ("SpecialButton: " + name);
        };

        logIfSpecialButtonIsDown (SpecialButton::start,         "start");
        logIfSpecialButtonIsDown (SpecialButton::select,        "select");
        logIfSpecialButtonIsDown (SpecialButton::back,          "back");
        logIfSpecialButtonIsDown (SpecialButton::centre,        "centre");
        logIfSpecialButtonIsDown (SpecialButton::guide,         "guide");
        logIfSpecialButtonIsDown (SpecialButton::blue,          "blue");
        logIfSpecialButtonIsDown (SpecialButton::red,           "red");
        logIfSpecialButtonIsDown (SpecialButton::green,         "green");
        logIfSpecialButtonIsDown (SpecialButton::yellow,        "yellow");
        logIfSpecialButtonIsDown (SpecialButton::volumeUp,      "volumeUp");
        logIfSpecialButtonIsDown (SpecialButton::volumeDown,    "volumeDown");
        logIfSpecialButtonIsDown (SpecialButton::volumeMute,    "volumeMute");
        logIfSpecialButtonIsDown (SpecialButton::channelUp,     "channelUp");
        logIfSpecialButtonIsDown (SpecialButton::channelDown,   "channelDown");
        logIfSpecialButtonIsDown (SpecialButton::playPause,     "playPause");
        logIfSpecialButtonIsDown (SpecialButton::stop,          "stop");
        logIfSpecialButtonIsDown (SpecialButton::rewind,        "rewind");
        logIfSpecialButtonIsDown (SpecialButton::fastForward,   "fastForward");
        logIfSpecialButtonIsDown (SpecialButton::previousTrack, "previousTrack");
        logIfSpecialButtonIsDown (SpecialButton::nextTrack,     "nextTrack");

        Thread::sleep (100);
    }
*/

class GamepadInputSource::Pimpl final
{
public:
    Pimpl (int indexOrId) :
        playerIndex (static_cast<GCControllerPlayerIndex> (indexOrId))
    {
        updateState();

#if 0
static id connectObserver = nil;
static id disconnectObserver = nil;

static void poopIn()
{
    jassertfalse;
}
static void poopOut()
{
    jassertfalse;
}

        NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
        connectObserver = [center addObserverForName:GCControllerDidConnectNotification
                                              object:nil
                                               queue:nil
                                          usingBlock:^(NSNotification *note) {
                                              GCController* controller = note.object;
                                              poopIn();
                                          }];

        disconnectObserver = [center addObserverForName:GCControllerDidDisconnectNotification
                                                 object:nil
                                                  queue:nil
                                             usingBlock:^(NSNotification *note) {
                                                 GCController* controller = note.object;
                                                 poopOut();
                                             }];
#endif
    }

    ~Pimpl()
    {
    }

    //==============================================================================
    constexpr int getID() const                                     { return static_cast<int> (playerIndex); }
    String getName() const                                          { return {}; }
    constexpr bool isVirtual() const                                { return false; }
    constexpr bool supportsOrientation() const                      { return false; }
    constexpr bool supportsMotion() const                           { return false; }
    constexpr bool supportsVoice() const                            { return false; }
    constexpr bool hasLight() const                                 { return false; }
    Colour getLightColour() const                                   { return {}; }
    constexpr bool hasAccelerometer() const                         { return false; }
    constexpr bool hasGyroscope() const                             { return false; }
    constexpr double getAccelerometerValue (ThreeAxisUnit) const    { return 0.0; }
    constexpr double getGyroscopeValue (ThreeAxisUnit) const        { return 0.0; }
    bool isConnected() const                                        { return updateState(); }

    //==============================================================================
    Type getType() const
    {
        if (! isConnected())
            return Type::none;

        const auto desc = String (controller.description.UTF8String);

        if (desc.equalsIgnoreCase ("Pro Controller"))       return Type::nintendo;
        else if (desc.containsIgnoreCase ("Xbox"))          return Type::xbox;
        else if (desc.containsIgnoreCase ("playstation"))   return Type::playstation;
        else if (desc.containsIgnoreCase ("Sony"))          return Type::playstation;
        // else {} // TODO: Siri remote & Apple remote

        return Type::unknown;
    }

    Subtype getSubtype() const
    {
        switch (getType())
        {
            case Type::nintendo:
            case Type::xbox:
            case Type::playstation:
                return Subtype::gamepad;

            case Type::siriRemote:
            case Type::appleRemote:
                return Subtype::remote;

            default:
            break;
        };

        return Subtype::none;
    }

    //==============================================================================
    BatteryType getBatteryType() const
    {
        return BatteryType::none;
    }

    bool isWireless() const
    {
        return getBatteryType() != BatteryType::wired;
    }

    double getBatteryLevel() const
    {
        return 0.0;
    }

    //==============================================================================
    bool isDirectionalPadButtonDown (DirectionalPadButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case DirectionalPadButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.up.pressed == TRUE;
                break;

                case DirectionalPadButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.right.pressed == TRUE;
                break;

                case DirectionalPadButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.down.pressed == TRUE;
                break;

                case DirectionalPadButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.left.pressed == TRUE;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return false;
    }

    double getDirectionalPadButtonDepression (DirectionalPadButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case DirectionalPadButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.up.value;
                break;

                case DirectionalPadButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.right.value;
                break;

                case DirectionalPadButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.down.value;
                break;

                case DirectionalPadButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.left.value;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return 0.0;
    }

    //==============================================================================
    bool isActionButtonDown (ActionButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case ActionButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonY.pressed == TRUE;
                break;

                case ActionButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonB.pressed == TRUE;
                break;

                case ActionButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonA.pressed == TRUE;
                break;

                case ActionButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonX.pressed == TRUE;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return false;
    }

    double getActionButtonDepression (ActionButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case ActionButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonY.value;
                break;

                case ActionButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonB.value;
                break;

                case ActionButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonA.value;
                break;

                case ActionButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonX.value;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return 0.0;
    }

    //==============================================================================
    Point<double> getAxis (GCControllerDirectionPad* button) const
    {
        if (button == nullptr)
            return {};

        return
        {
            (double) button.xAxis.value,
            (double) button.yAxis.value
        };
    }

    Point<double> getAxis (bool isLeft) const
    {
        if (updateState() && extendedGamepad != nullptr)
            return isLeft
                    ? getAxis (extendedGamepad.leftThumbstick)
                    : getAxis (extendedGamepad.rightThumbstick);

        return {};
    }

    Point<double> getLeftAxis (bool) const                      { return getAxis (true); }
    Point<double> getRightAxis (bool) const                     { return getAxis (false); }
    constexpr Point<double> getDeadZone (double value) const    { return {}; }
    constexpr Point<double> getLeftAxisDeadZone() const         { return {}; }
    constexpr Point<double> getRightAxisDeadZone() const        { return {}; }
    void setLeftAxisDeadZone (Point<double>)                    {}
    void setRightAxisDeadZone (Point<double>)                   {}

    //==============================================================================
    void setLeftMotorVibration (double v)
    {
    }

    void setRightMotorVibration (double v)
    {
    }

    double getMotorVibration (bool isLeft) const
    {
        return 0.0;
    }

    double getLeftMotorVibration() const    { return getMotorVibration (true); }
    double getRightMotorVibration() const   { return getMotorVibration (false); }

    //==============================================================================
    double getShoulderButtonDepression (ShoulderButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case ShoulderButton::leftBumper:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.leftShoulder.value;
                break;

                case ShoulderButton::rightBumper:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.rightShoulder.value;
                break;

                case ShoulderButton::leftAnalog:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.leftThumbstickButton.value;
                break;

                case ShoulderButton::rightAnalog:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.rightThumbstickButton.value;
                break;

                case ShoulderButton::leftTrigger:
                case ShoulderButton::rightTrigger:
                    if (extendedGamepad != nullptr)
                        return b == ShoulderButton::leftTrigger
                                ? (double) extendedGamepad.leftTrigger.value
                                : (double) extendedGamepad.rightTrigger.value;
                break;

                default:
                break;
            };
        }

        return 0.0;
    }

    bool isShoulderButtonDown (ShoulderButton b) const
    {
        return getShoulderButtonDepression (b) > 0.0;
    }

    //==============================================================================
    bool isSpecialButtonDown (SpecialButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case SpecialButton::start:  return extendedGamepad.buttonMenu.pressed == TRUE;
                case SpecialButton::back:   return extendedGamepad.buttonOptions.pressed == TRUE;
                case SpecialButton::select: return extendedGamepad.buttonHome.pressed == TRUE;

                default: break;
            };
        }

        return false;
    }

    double getSpecialButtonDepression (SpecialButton b) const
    {
        return isSpecialButtonDown (b) ? 1.0 : 0.0;
    }

private:
    //==============================================================================
    const GCControllerPlayerIndex playerIndex;
    mutable GCController* controller = nullptr;
    mutable GCGamepad* gamepad = nullptr;
    mutable GCMicroGamepad* microGamepad = nullptr;
    mutable GCExtendedGamepad* extendedGamepad = nullptr;

    struct
        API_AVAILABLE(macos(11.0), ios(14.0), tvos(14.0))
        ControllerProfiles final
    {
        void clear()
        {
            dualShock = nullptr;
            xbox = nullptr;
            dualSense = nullptr;
        }

        GCDualShockGamepad* dualShock = nullptr;
        GCXboxGamepad* xbox = nullptr;
        GCDualSenseGamepad* dualSense = nullptr;
    };

    struct
        API_AVAILABLE(macos(11.0), ios(14.0), tvos(14.0))
        MiscFeatures final
    {
        void clear()
        {
            battery = nullptr;
            haptics = nullptr;
            light = nullptr;
        }

        mutable GCDeviceBattery* battery = nullptr;
        mutable GCDeviceHaptics* haptics = nullptr;
        mutable GCDeviceLight* light = nullptr;
    };

    mutable ControllerProfiles profiles;
    mutable MiscFeatures miscFeatures;

    //==============================================================================
    bool updateState() const
    {
        auto latestControllers = [GCController controllers];
        const auto index = static_cast<int> (playerIndex);

        controller = nullptr;
        gamepad = nullptr;
        microGamepad = nullptr;
        extendedGamepad = nullptr;
        profiles.clear();
        miscFeatures.clear();

        if ([latestControllers count] >= (index + 1))
        {
            controller = latestControllers[index];
            gamepad = [controller gamepad];
            microGamepad = [controller microGamepad];
            extendedGamepad = [controller extendedGamepad];

            if (extendedGamepad != nullptr)
            {
                //dualShock = [extendedGamepad ];
                //xbox = [];
                //dualSense = [];
            }
        }

        return controller != nullptr;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

} // namespace juce
=======
/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include <AvailabilityMacros.h>

namespace juce
{

#undef JUCE_MACOS_SUPPORTS_LIGHT
#define JUCE_MACOS_SUPPORTS_LIGHT MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3

#undef JUCE_MACOS_SUPPORTS_THUMBSTICK
#define JUCE_MACOS_SUPPORTS_THUMBSTICK MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14

#undef JUCE_MACOS_SUPPORTS_XBOX_AND_DUALSHOCK
#define JUCE_MACOS_SUPPORTS_XBOX_AND_DUALSHOCK MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_11_0

#undef JUCE_MACOS_SUPPORTS_DUALSENSE
#define JUCE_MACOS_SUPPORTS_DUALSENSE MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_3

/**
    GamepadInputSource gis (0);

    if (gis.isConnected())
    {
        Logger::writeToLog (String ("ID: ")                     + String (gis.getID()));
        Logger::writeToLog (String ("Battery Type: ")           + String ((int) gis.getBatteryType()));
        Logger::writeToLog (String ("Battery Level: ")          + String (gis.getBatteryLevel()));
        Logger::writeToLog (String ("Type: ")                   + String ((int) gis.getType()));
        Logger::writeToLog (String ("Subtype: ")                + String ((int) gis.getSubtype()));
        Logger::writeToLog (String ("Is wireless? ")            + booleanToString (gis.isWireless(), true));
        Logger::writeToLog (String ("Is virtual? ")             + booleanToString (gis.isVirtual(), true));
        Logger::writeToLog (String ("Supports orientation? ")   + booleanToString (gis.supportsOrientation(), true));
        Logger::writeToLog (String ("Supports motion? ")        + booleanToString (gis.supportsMotion(), true));
        Logger::writeToLog (String ("Has light? ")              + booleanToString (gis.hasLight(), true));
        Logger::writeToLog (String ("Has accelerometer? ")      + booleanToString (gis.hasAccelerometer(), true));
        Logger::writeToLog (String ("Has gyroscope? ")          + booleanToString (gis.hasGyroscope(), true));
    }

    while (gis.isConnected())
    {
        auto addVibesIfPossible = [&] (Point<double> axis, Point<double> deadzone, bool isLeft)
        {
            const auto mag = getMagnitude (axis);

            if (mag > getMagnitude (deadzone))
            {
                if (isLeft)
                  gis.setLeftMotorVibration (1.0 - mag);
                else
                  gis.setRightMotorVibration (1.0 - mag);
            }
        };

        addVibesIfPossible (gis.getLeftAxis(), gis.getLeftAxisDeadZone(), true);
        addVibesIfPossible (gis.getRightAxis(), gis.getRightAxisDeadZone(), false);

        using DirectionalPadButton = GamepadInputSource::DirectionalPadButton;
        auto logIfDirectionalPadButtonIsDown = [&] (DirectionalPadButton b, const String& name)
        {
            if (gis.isDirectionalPadButtonDown (b))
                Logger::writeToLog ("DirectionalPad: " + name);
        };

        logIfDirectionalPadButtonIsDown (DirectionalPadButton::up,      "Up");
        logIfDirectionalPadButtonIsDown (DirectionalPadButton::right,   "Right");
        logIfDirectionalPadButtonIsDown (DirectionalPadButton::down,    "Down");
        logIfDirectionalPadButtonIsDown (DirectionalPadButton::left,    "Left");

        using ActionButton = GamepadInputSource::ActionButton;
        auto logIfActionButtonIsDown = [&] (ActionButton b, const String& name)
        {
            if (gis.isActionButtonDown (b))
                Logger::writeToLog ("ActionButton: " + name);
        };

        logIfActionButtonIsDown (ActionButton::up,      "Up");
        logIfActionButtonIsDown (ActionButton::right,   "Right");
        logIfActionButtonIsDown (ActionButton::down,    "Down");
        logIfActionButtonIsDown (ActionButton::left,    "Left");

        using ShoulderButton = GamepadInputSource::ShoulderButton;
        auto logIfShoulderButtonIsDown = [&] (ShoulderButton b, const String& name)
        {
            if (gis.isShoulderButtonDown (b))
                Logger::writeToLog ("ShoulderButton: " + name);
        };

        logIfShoulderButtonIsDown (ShoulderButton::leftTrigger,     "leftTrigger");
        logIfShoulderButtonIsDown (ShoulderButton::leftBumper,      "leftBumper");
        logIfShoulderButtonIsDown (ShoulderButton::leftAnalog,      "leftAnalog");
        logIfShoulderButtonIsDown (ShoulderButton::rightTrigger,    "rightTrigger");
        logIfShoulderButtonIsDown (ShoulderButton::rightBumper,     "rightBumper");
        logIfShoulderButtonIsDown (ShoulderButton::rightAnalog,     "rightAnalog");

        using SpecialButton = GamepadInputSource::SpecialButton;
        auto logIfSpecialButtonIsDown = [&] (SpecialButton b, const String& name)
        {
            if (gis.isSpecialButtonDown (b))
                Logger::writeToLog ("SpecialButton: " + name);
        };

        logIfSpecialButtonIsDown (SpecialButton::start,         "start");
        logIfSpecialButtonIsDown (SpecialButton::select,        "select");
        logIfSpecialButtonIsDown (SpecialButton::back,          "back");
        logIfSpecialButtonIsDown (SpecialButton::centre,        "centre");
        logIfSpecialButtonIsDown (SpecialButton::guide,         "guide");
        logIfSpecialButtonIsDown (SpecialButton::blue,          "blue");
        logIfSpecialButtonIsDown (SpecialButton::red,           "red");
        logIfSpecialButtonIsDown (SpecialButton::green,         "green");
        logIfSpecialButtonIsDown (SpecialButton::yellow,        "yellow");
        logIfSpecialButtonIsDown (SpecialButton::volumeUp,      "volumeUp");
        logIfSpecialButtonIsDown (SpecialButton::volumeDown,    "volumeDown");
        logIfSpecialButtonIsDown (SpecialButton::volumeMute,    "volumeMute");
        logIfSpecialButtonIsDown (SpecialButton::channelUp,     "channelUp");
        logIfSpecialButtonIsDown (SpecialButton::channelDown,   "channelDown");
        logIfSpecialButtonIsDown (SpecialButton::playPause,     "playPause");
        logIfSpecialButtonIsDown (SpecialButton::stop,          "stop");
        logIfSpecialButtonIsDown (SpecialButton::rewind,        "rewind");
        logIfSpecialButtonIsDown (SpecialButton::fastForward,   "fastForward");
        logIfSpecialButtonIsDown (SpecialButton::previousTrack, "previousTrack");
        logIfSpecialButtonIsDown (SpecialButton::nextTrack,     "nextTrack");

        Thread::sleep (100);
    }
*/

class GamepadInputSource::Pimpl final
{
public:
    Pimpl (int indexOrId) :
        playerIndex (static_cast<GCControllerPlayerIndex> (indexOrId))
    {
        updateState();

#if 0
static id connectObserver = nil;
static id disconnectObserver = nil;

static void poopIn()
{
    jassertfalse;
}
static void poopOut()
{
    jassertfalse;
}

        NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
        connectObserver = [center addObserverForName:GCControllerDidConnectNotification
                                              object:nil
                                               queue:nil
                                          usingBlock:^(NSNotification *note) {
                                              GCController* controller = note.object;
                                              poopIn();
                                          }];

        disconnectObserver = [center addObserverForName:GCControllerDidDisconnectNotification
                                                 object:nil
                                                  queue:nil
                                             usingBlock:^(NSNotification *note) {
                                                 GCController* controller = note.object;
                                                 poopOut();
                                             }];
#endif
    }

    ~Pimpl()
    {
    }

    //==============================================================================
    constexpr int getID() const                                     { return static_cast<int> (playerIndex); }
    String getName() const                                          { return {}; }
    constexpr bool isVirtual() const                                { return false; }
    constexpr bool supportsOrientation() const                      { return false; }
    constexpr bool supportsMotion() const                           { return false; }
    constexpr bool supportsVoice() const                            { return false; }
    constexpr bool hasLight() const                                 { return false; }
    Colour getLightColour() const                                   { return {}; }
    constexpr bool hasAccelerometer() const                         { return false; }
    constexpr bool hasGyroscope() const                             { return false; }
    constexpr double getAccelerometerValue (ThreeAxisUnit) const    { return 0.0; }
    constexpr double getGyroscopeValue (ThreeAxisUnit) const        { return 0.0; }
    bool isConnected() const                                        { return updateState(); }

    //==============================================================================
    Type getType() const
    {
        if (! isConnected())
            return Type::none;

        const auto desc = String (controller.description.UTF8String);

        if (desc.equalsIgnoreCase ("Pro Controller"))       return Type::nintendo;
        else if (desc.containsIgnoreCase ("Xbox"))          return Type::xbox;
        else if (desc.containsIgnoreCase ("playstation"))   return Type::playstation;
        else if (desc.containsIgnoreCase ("Sony"))          return Type::playstation;
        // else {} // TODO: Siri remote & Apple remote

        return Type::unknown;
    }

    Subtype getSubtype() const
    {
        switch (getType())
        {
            case Type::nintendo:
            case Type::xbox:
            case Type::playstation:
                return Subtype::gamepad;

            case Type::siriRemote:
            case Type::appleRemote:
                return Subtype::remote;

            default:
            break;
        };

        return Subtype::none;
    }

    //==============================================================================
    BatteryType getBatteryType() const
    {
        return BatteryType::none;
    }

    bool isWireless() const
    {
        return getBatteryType() != BatteryType::wired;
    }

    double getBatteryLevel() const
    {
        return 0.0;
    }

    //==============================================================================
    bool isDirectionalPadButtonDown (DirectionalPadButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case DirectionalPadButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.up.pressed == TRUE;
                break;

                case DirectionalPadButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.right.pressed == TRUE;
                break;

                case DirectionalPadButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.down.pressed == TRUE;
                break;

                case DirectionalPadButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.left.pressed == TRUE;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return false;
    }

    double getDirectionalPadButtonDepression (DirectionalPadButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case DirectionalPadButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.up.value;
                break;

                case DirectionalPadButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.right.value;
                break;

                case DirectionalPadButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.down.value;
                break;

                case DirectionalPadButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.dpad.left.value;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return 0.0;
    }

    //==============================================================================
    bool isActionButtonDown (ActionButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case ActionButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonY.pressed == TRUE;
                break;

                case ActionButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonB.pressed == TRUE;
                break;

                case ActionButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonA.pressed == TRUE;
                break;

                case ActionButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonX.pressed == TRUE;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return false;
    }

    double getActionButtonDepression (ActionButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case ActionButton::up:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonY.value;
                break;

                case ActionButton::right:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonB.value;
                break;

                case ActionButton::down:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonA.value;
                break;

                case ActionButton::left:
                    if (extendedGamepad != nullptr)
                        return extendedGamepad.buttonX.value;
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return 0.0;
    }

    //==============================================================================
    Point<double> getAxis (GCControllerDirectionPad* button) const
    {
        if (button == nullptr)
            return {};

        return
        {
            (double) button.xAxis.value,
            (double) button.yAxis.value
        };
    }

    Point<double> getAxis (bool isLeft) const
    {
        if (updateState() && extendedGamepad != nullptr)
            return isLeft
                    ? getAxis (extendedGamepad.leftThumbstick)
                    : getAxis (extendedGamepad.rightThumbstick);

        return {};
    }

    Point<double> getLeftAxis (bool) const                      { return getAxis (true); }
    Point<double> getRightAxis (bool) const                     { return getAxis (false); }
    constexpr Point<double> getDeadZone (double value) const    { return {}; }
    constexpr Point<double> getLeftAxisDeadZone() const         { return {}; }
    constexpr Point<double> getRightAxisDeadZone() const        { return {}; }
    void setLeftAxisDeadZone (Point<double>)                    {}
    void setRightAxisDeadZone (Point<double>)                   {}

    //==============================================================================
    void setLeftMotorVibration (double v)
    {
    }

    void setRightMotorVibration (double v)
    {
    }

    double getMotorVibration (bool isLeft) const
    {
        return 0.0;
    }

    double getLeftMotorVibration() const    { return getMotorVibration (true); }
    double getRightMotorVibration() const   { return getMotorVibration (false); }

    //==============================================================================
    double getShoulderButtonDepression (ShoulderButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case ShoulderButton::leftBumper:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.leftShoulder.value;
                break;

                case ShoulderButton::rightBumper:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.rightShoulder.value;
                break;

               #if JUCE_MACOS_SUPPORTS_THUMBSTICK
                case ShoulderButton::leftAnalog:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.leftThumbstickButton.value;
                break;

                case ShoulderButton::rightAnalog:
                    if (extendedGamepad != nullptr)
                        return (double) extendedGamepad.rightThumbstickButton.value;
                break;
               #endif

                case ShoulderButton::leftTrigger:
                case ShoulderButton::rightTrigger:
                    if (extendedGamepad != nullptr)
                        return b == ShoulderButton::leftTrigger
                                ? (double) extendedGamepad.leftTrigger.value
                                : (double) extendedGamepad.rightTrigger.value;
                break;

                default:
                break;
            };
        }

        return 0.0;
    }

    bool isShoulderButtonDown (ShoulderButton b) const
    {
        return getShoulderButtonDepression (b) > 0.0;
    }

    //==============================================================================
    bool isSpecialButtonDown (SpecialButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                case SpecialButton::start:  return extendedGamepad.buttonMenu.pressed == TRUE;
                case SpecialButton::back:   return extendedGamepad.buttonOptions.pressed == TRUE;
                case SpecialButton::select: return extendedGamepad.buttonHome.pressed == TRUE;

                default: break;
            };
        }

        return false;
    }

    double getSpecialButtonDepression (SpecialButton b) const
    {
        return isSpecialButtonDown (b) ? 1.0 : 0.0;
    }

private:
    //==============================================================================
    const GCControllerPlayerIndex playerIndex;
    mutable GCController* controller = nullptr;
    mutable GCGamepad* gamepad = nullptr;
    mutable GCMicroGamepad* microGamepad = nullptr;
    mutable GCExtendedGamepad* extendedGamepad = nullptr;

    struct ControllerProfiles final
    {
        void clear()
        {
           #if JUCE_MACOS_SUPPORTS_XBOX_AND_DUALSHOCK
            dualShock = nullptr;
            xbox = nullptr;
           #endif

           #if JUCE_MACOS_SUPPORTS_DUALSENSE
            dualSense = nullptr;
           #endif
        }

       #if JUCE_MACOS_SUPPORTS_XBOX_AND_DUALSHOCK
        GCDualShockGamepad* dualShock = nullptr;
        GCXboxGamepad* xbox = nullptr;
       #endif

       #if JUCE_MACOS_SUPPORTS_DUALSENSE
        GCDualSenseGamepad* dualSense = nullptr;
       #endif
    };

    struct MiscFeatures final
    {
        void clear()
        {
            battery = nullptr;
            haptics = nullptr;

           #if JUCE_MACOS_SUPPORTS_LIGHT
            light = nullptr;
           #endif
        }

        GCDeviceBattery* battery = nullptr;
        GCDeviceHaptics* haptics = nullptr;

       #if JUCE_MACOS_SUPPORTS_LIGHT
        GCDeviceLight* light = nullptr;
       #endif
    };

    mutable ControllerProfiles profiles;
    mutable MiscFeatures miscFeatures;

    //==============================================================================
    bool updateState() const
    {
        auto latestControllers = [GCController controllers];
        const auto index = static_cast<int> (playerIndex);

        controller = nullptr;
        gamepad = nullptr;
        microGamepad = nullptr;
        extendedGamepad = nullptr;
        profiles.clear();
        miscFeatures.clear();

        if ([latestControllers count] >= (index + 1))
        {
            controller = latestControllers[index];
            gamepad = [controller gamepad];
            microGamepad = [controller microGamepad];
            extendedGamepad = [controller extendedGamepad];

            if (extendedGamepad != nullptr)
            {
                //dualShock = [extendedGamepad ];

                //xbox = [];

               #if JUCE_MACOS_SUPPORTS_DUALSENSE
                profiles.dualSense = nullptr;
               #endif
            }
        }

        return controller != nullptr;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

} // namespace juce
>>>>>>> 112108a90 (Bits.):modules/juce_gui_basics/native/juce_mac_Gamepad.mm
