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
#define JUCE_MACOS_SUPPORTS_LIGHT (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_11_0)

#undef JUCE_MACOS_SUPPORTS_THUMBSTICK
#define JUCE_MACOS_SUPPORTS_THUMBSTICK (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14_1)

#undef JUCE_MACOS_SUPPORTS_XBOX_AND_DUALSHOCK
#define JUCE_MACOS_SUPPORTS_XBOX_AND_DUALSHOCK (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_11_0)

#undef JUCE_MACOS_SUPPORTS_DUALSENSE
#define JUCE_MACOS_SUPPORTS_DUALSENSE (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_3)

static id connectObserver = nil;
static id disconnectObserver = nil;

class GamepadInputSource::Pimpl final
{
public:
    Pimpl (int indexOrId) :
        playerIndex (static_cast<GCControllerPlayerIndex> (indexOrId))
    {
        updateState();

        if (connectObserver != nullptr)
            return;

        NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
        connectObserver = [center addObserverForName:GCControllerDidConnectNotification
                                              object:nil
                                               queue:nil
                                             usingBlock:^(NSNotification*) { }];

        disconnectObserver = [center addObserverForName:GCControllerDidDisconnectNotification
                                                 object:nil
                                                  queue:nil
                                             usingBlock:^(NSNotification*) { }];
    }

    ~Pimpl()
    {
    }

    //==============================================================================
    constexpr int getID() const                                     { return static_cast<int> (playerIndex); }
    bool isConnected() const                                        { return updateState(); }

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

    String getName() const
    {
        if (! isConnected())
            return {};

        String name (controller.description.UTF8String);

        if (name.containsIgnoreCase ("vendorName"))
            name = name.fromFirstOccurrenceOf ("'", false, false)
                       .upToLastOccurrenceOf("'", false, false);

        return name;
    }

    Type getType() const
    {
        const auto desc = getName();

        if (desc.isEmpty())                                     return Type::none;
        else if (desc.equalsIgnoreCase ("Pro Controller"))      return Type::switchPro;
        else if (desc.containsIgnoreCase ("Xbox360"))           return Type::xbox360;
        else if (desc.containsIgnoreCase ("Xbox"))              return Type::xboxOne;
        else if (desc.containsIgnoreCase ("DualShock 4"))       return Type::dualShock4;
        else if (desc.containsIgnoreCase ("DualShock"))         return Type::dualShock;
        else if (desc.containsIgnoreCase ("DualSense"))         return Type::dualSense;
        else if (desc.containsIgnoreCase ("Luna"))              return Type::luna;
        // else {} // TODO: Siri remote & Apple remote

        return Type::unknown;
    }

    Subtype getSubtype() const
    {
        switch (getType())
        {
            case Type::xbox360:
            case Type::xboxOne:
            case Type::dualShock:
            case Type::dualShock4:
            case Type::dualSense:
            case Type::switchPro:
            case Type::luna:
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
        if (isConnected())
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
        if (isConnected())
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
        if (isConnected())
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
        if (isConnected())
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
        if (isConnected())
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
        if (isConnected())
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
