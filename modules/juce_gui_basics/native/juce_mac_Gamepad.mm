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

class GamepadInputSource::Pimpl final
{
public:
    Pimpl (int indexOrId) :
        playerIndex (static_cast<GCControllerPlayerIndex> (indexOrId))
    {
        updateState();
        
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
    }

    ~Pimpl()
    {
    }

    //==============================================================================
    void updateCapabilities (int& result) const
    {
        result = 0;

        //if (updateState())
        //    result = XInputGetCapabilities (index, BATTERY_DEVTYPE_GAMEPAD, &caps);
    }

    void updateCapabilities (bool& wasOk) const
    {
        wasOk = false;

        //DWORD v = ERROR_SUCCESS;
        //updateCapabilities (v);
        //wasOk = v == ERROR_SUCCESS;
    }

    //==============================================================================
    constexpr int getID() const                                     { return static_cast<int> (playerIndex); }
    String getName() const                                          { return {}; }
    constexpr bool isVirtual() const                                { return false; }
    constexpr bool supportsOrientation() const                      { return false; }
    constexpr bool supportsMotion() const                           { return false; }
    constexpr bool hasLight() const                                 { return false; }
    Colour getLightColour() const                                   { return {}; }
    constexpr bool hasAccelerometer() const                         { return false; }
    constexpr bool hasGyroscope() const                             { return false; }
    constexpr double getAccelerometerValue (ThreeAxisUnit) const    { return 0.0; }
    constexpr double getGyroscopeValue (ThreeAxisUnit) const        { return 0.0; }

    //==============================================================================
    bool isConnected() const
    {
        return updateState();
    }

    Type getType() const
    {
        if (! isConnected())
            return Type::none;

        return Type::unknown;
    }

    Subtype getSubtype() const
    {
/*
        DWORD rv;
        updateCapabilities (rv);

        if (rv == ERROR_DEVICE_NOT_CONNECTED)   return Subtype::none;
        else if (rv != ERROR_SUCCESS)           return Subtype::unknown;

        switch (caps.SubType)
        {
            case XINPUT_DEVSUBTYPE_GAMEPAD:             return Subtype::gamepad;
            case XINPUT_DEVSUBTYPE_WHEEL:               return Subtype::racewheel;
            case XINPUT_DEVSUBTYPE_FLIGHT_STICK:        return Subtype::flightstick;
            case XINPUT_DEVSUBTYPE_DANCE_PAD:           return Subtype::dancePad;
            case XINPUT_DEVSUBTYPE_GUITAR:              return Subtype::guitar;
            case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE:    return Subtype::guitarAlternate;
            case XINPUT_DEVSUBTYPE_GUITAR_BASS:         return Subtype::bass;
            case XINPUT_DEVSUBTYPE_DRUM_KIT:            return Subtype::drumKit;
            case XINPUT_DEVSUBTYPE_ARCADE_STICK:        return Subtype::arcadeStick;
            case XINPUT_DEVSUBTYPE_ARCADE_PAD:          return Subtype::arcadePad;

            default:
            break;
        };
*/
        return Subtype::unknown;
    }

    bool supportsVoice() const
    {
        /*
        DWORD rv;
        updateCapabilities (rv);
        if (rv == ERROR_SUCCESS)
            return (caps.Flags & XINPUT_CAPS_VOICE_SUPPORTED) != 0;
*/
        return false;
    }

    //==============================================================================
    void updateBatteryInfo (bool& wasOk) const
    {
        wasOk = false;
    }

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
        return false;
    }

    double getDirectionalPadButtonDepression (DirectionalPadButton b) const
    {
        return isDirectionalPadButtonDown (b) ? 1.0 : 0.0;
    }

    //==============================================================================
    bool isActionButtonDown (ActionButton b) const
    {
        return false;
    }

    double getActionButtonDepression (ActionButton b) const
    {
        return isActionButtonDown (b) ? 1.0 : 0.0;
    }

    //==============================================================================
    Point<double> getAxis (double x, double y, bool isLeft, bool ignoreDeadzone) const
    {
        if (ignoreDeadzone)
            return
            {
                jmap (x, (double) SHRT_MIN, (double) SHRT_MAX, -1.0, 1.0),
                jmap (y, (double) SHRT_MIN, (double) SHRT_MAX, -1.0, 1.0),
            };

        const auto dz = isLeft
                            ? (double) 0.0
                            : (double) 0.0;

        const auto lowerLimit = (double) SHRT_MIN + dz;
        const auto upperLimit = (double) SHRT_MAX - dz;

        return
        {
            jmap (jmax (x, dz), lowerLimit, upperLimit, -1.0, 1.0),
            jmap (jmax (y, dz), lowerLimit, upperLimit, -1.0, 1.0),
        };
    }

    Point<double> getLeftAxis (bool ignoreDeadzone) const
    {
        return {};
    }

    Point<double> getRightAxis (bool ignoreDeadzone) const
    {
        return {};
    }

    Point<double> getDeadZone (double value) const
    {
        return
        {
            jmap (value, -32768.0, 32767.0, -1.0, 1.0),
            jmap (value, -32768.0, 32767.0, -1.0, 1.0),
        };
    }

    Point<double> getLeftAxisDeadZone() const   { return getDeadZone ((double) 0.0); }
    Point<double> getRightAxisDeadZone() const  { return getDeadZone ((double) 0.0); }

    void setLeftAxisDeadZone (Point<double>) {}
    void setRightAxisDeadZone (Point<double>) {}

    //==============================================================================
    void setLeftMotorVibration (double v)
    {
    }

    void setRightMotorVibration (double v)
    {
    }

    static double remapMotorVibration (int value)
    {
        return jmap ((double) value, 0.0, 65535.0, 0.0, 1.0);
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
        return 0.0;
    }

    bool isShoulderButtonDown (ShoulderButton b) const
    {
        return getShoulderButtonDepression (b) > 0.0;
    }

    //==============================================================================
    bool isSpecialButtonDown (SpecialButton b) const
    {
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
    mutable GCDeviceBattery* battery = nullptr;
    mutable GCDeviceHaptics* haptics = nullptr;
    mutable GCDeviceLight* light = nullptr;

    //==============================================================================
    bool updateState() const
    {
        auto latestControllers = [GCController controllers];
        const auto index = static_cast<int> (playerIndex);

        if ([latestControllers count] >= (index + 1))
        {
            controller = latestControllers[index];
            gamepad = [controller gamepad];
            microGamepad = [controller microGamepad];
            extendedGamepad = [controller extendedGamepad];
            battery = [controller battery];
            haptics = [controller haptics];
            light = [controller light];
        }
        else
        {
            controller = nullptr;
            gamepad = nullptr;
            microGamepad = nullptr;
            extendedGamepad = nullptr;
            battery = nullptr;
            haptics = nullptr;
            light = nullptr;
        }

        return controller != nullptr;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

} // namespace juce
