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

class GamepadInputSource::Pimpl final
{
public:
    Pimpl (int indexOrId) :
        index (indexOrId)
    {
        jassert (indexOrId <= 3);
    }

    ~Pimpl()
    {
    }

    //==============================================================================
    constexpr int getID() const                                     { return index; }
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

        return Type::none;
    }

    Subtype getSubtype() const
    {
#if 0
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
#endif
        return Subtype::unknown;
    }

    bool supportsVoice() const
    {
#if 0
        DWORD rv;
        updateCapabilities (rv);
        if (rv == ERROR_SUCCESS)
            return (caps.Flags & XINPUT_CAPS_VOICE_SUPPORTED) != 0;
#endif
        return false;
    }

    //==============================================================================
    void updateBatteryInfo (bool& wasOk) const
    {
        wasOk = false;
    }

    BatteryType getBatteryType() const
    {
        bool wasOk = true;
        updateBatteryInfo (wasOk);

        if (wasOk)
        {
#if 0
            switch (batteryInfo.BatteryType)
            {
                case BATTERY_TYPE_WIRED:    return BatteryType::wired;
                case BATTERY_TYPE_ALKALINE: return BatteryType::alkaline;
                case BATTERY_TYPE_NIMH:     return BatteryType::nimh;
                case BATTERY_TYPE_UNKNOWN:  return BatteryType::unknown;

                default: break;
            };
#endif
        }

        return BatteryType::none;
    }

    bool isWireless() const
    {
        return getBatteryType() != BatteryType::wired;
    }

    double getBatteryLevel() const
    {
        bool wasOk;
        updateBatteryInfo (wasOk);
        if (wasOk)
        {
#if 0
            switch (batteryInfo.BatteryLevel)
            {
                case BATTERY_LEVEL_EMPTY:   return 0.0;
                case BATTERY_LEVEL_LOW:     return 0.25;
                case BATTERY_LEVEL_MEDIUM:  return 0.75;
                case BATTERY_LEVEL_FULL:    return 1.0;

                default: break;
            };
#endif
        }

        return 0.0;
    }

    //==============================================================================
    bool isDirectionalPadButtonDown (DirectionalPadButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                // case DirectionalPadButton::up:      return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
                // case DirectionalPadButton::right:   return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
                // case DirectionalPadButton::down:    return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
                // case DirectionalPadButton::left:    return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;

                default: jassertfalse; break;
            };
        }

        return false;
    }

    double getDirectionalPadButtonDepression (DirectionalPadButton b) const
    {
        return isDirectionalPadButtonDown (b) ? 1.0 : 0.0;
    }

    //==============================================================================
    bool isActionButtonDown (ActionButton b) const
    {
        if (updateState())
        {
            switch (b)
            {
                // case ActionButton::down:    return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
                // case ActionButton::right:   return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
                // case ActionButton::left:    return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
                // case ActionButton::up:      return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;

                default: jassertfalse; break;
            };
        }

        return false;
    }

    double getActionButtonDepression (ActionButton b) const
    {
        return isActionButtonDown (b) ? 1.0 : 0.0;
    }

    //==============================================================================
    Point<double> getAxis (double x, double y, bool isLeft, bool ignoreDeadzone) const
    {
#if 0
        if (ignoreDeadzone)
            return
            {
                jmap (x, (double) SHRT_MIN, (double) SHRT_MAX, -1.0, 1.0),
                jmap (y, (double) SHRT_MIN, (double) SHRT_MAX, -1.0, 1.0),
            };

        const auto dz = isLeft
                            ? (double) XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
                            : (double) XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

        const auto lowerLimit = (double) SHRT_MIN + dz;
        const auto upperLimit = (double) SHRT_MAX - dz;

        return
        {
            jmap (jmax (x, dz), lowerLimit, upperLimit, -1.0, 1.0),
            jmap (jmax (y, dz), lowerLimit, upperLimit, -1.0, 1.0),
        };
#endif
        return {};
    }

    Point<double> getLeftAxis (bool ignoreDeadzone) const
    {
        // if (updateState())
        //     return getAxis ((double) state.Gamepad.sThumbLX, (double) state.Gamepad.sThumbLY, true, ignoreDeadzone);

        return {};
    }

    Point<double> getRightAxis (bool ignoreDeadzone) const
    {
        // if (updateState())
        //     return getAxis ((double) state.Gamepad.sThumbRX, (double) state.Gamepad.sThumbRY, false, ignoreDeadzone);

        return {};
    }

    static Point<double> getDeadZone (double value)
    {
        return
        {
            jmap (value, -32768.0, 32767.0, -1.0, 1.0),
            jmap (value, -32768.0, 32767.0, -1.0, 1.0),
        };
    }

    static double remapMotorVibration (DWORD value)
    {
        return jmap ((double) value, 0.0, 65535.0, 0.0, 1.0);
    }

    Point<double> getLeftAxisDeadZone() const   { return {}; } // return getDeadZone ((double) XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE); }
    Point<double> getRightAxisDeadZone() const  { return {}; } // return getDeadZone ((double) XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE); }

    void setLeftAxisDeadZone (Point<double>) {}
    void setRightAxisDeadZone (Point<double>) {}

    //==============================================================================
    void setLeftMotorVibration (double v)
    {
        // bool wasOk;
        // updateCapabilities (wasOk);
        // if (! wasOk)
        //     return;
        // 
        // caps.Vibration.wLeftMotorSpeed = (WORD) jmap (v, 0.0, 65535.0);
        // wasOk = XInputSetState (index, &caps.Vibration) == ERROR_SUCCESS;
        // jassertquiet (wasOk);
    }

    void setRightMotorVibration (double v)
    {
        // bool wasOk;
        // updateCapabilities (wasOk);
        // if (! wasOk)
        //     return;
        // 
        // caps.Vibration.wRightMotorSpeed = (WORD) jmap (v, 0.0, 65535.0);
        // wasOk = XInputSetState (index, &caps.Vibration) == ERROR_SUCCESS;
        // jassertquiet (wasOk);
    }

    double getMotorVibration (bool isLeft) const
    {
        // bool wasOk;
        // updateCapabilities (wasOk);
        // if (! wasOk)
        //     return 0.0;
        // 
        // return remapMotorVibration (isLeft
        //                                 ? caps.Vibration.wLeftMotorSpeed
        //                                 : caps.Vibration.wRightMotorSpeed);

        return {};
    }

    double getLeftMotorVibration() const    { return getMotorVibration (true); }
    double getRightMotorVibration() const   { return getMotorVibration (false); }

    //==============================================================================
    double getShoulderButtonDepression (ShoulderButton b) const
    {
        if (updateState())
        {
            // switch (b)
            // {
            //     case ShoulderButton::leftBumper:    return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0 ? 1.0 : 0.0;
            //     case ShoulderButton::rightBumper:   return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0 ? 1.0 : 0.0;
            //     case ShoulderButton::leftAnalog:    return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0 ? 1.0 : 0.0;
            //     case ShoulderButton::rightAnalog:   return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0 ? 1.0 : 0.0;
            // 
            //     case ShoulderButton::leftTrigger:
            //     case ShoulderButton::rightTrigger:
            //     {
            //         auto v = b == ShoulderButton::leftTrigger
            //                     ? state.Gamepad.bLeftTrigger
            //                     : state.Gamepad.bRightTrigger;
            // 
            //         if (v <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
            //             return 0.0;
            // 
            //         v -= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            //         return (double) v / (255.0 - (double) XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
            //     }
            // 
            //     default: jassertfalse; break;
            // };
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
            // switch (b)
            // {
            //     case SpecialButton::start:  return (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
            //     case SpecialButton::back:   return (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
            // 
            //     default: break;
            // };
        }

        return false;
    }

    double getSpecialButtonDepression (SpecialButton b) const
    {
        return isSpecialButtonDown (b) ? 1.0 : 0.0;
    }

private:
    //==============================================================================
    const int index = 0;
    // mutable XINPUT_STATE state;
    // mutable XINPUT_CAPABILITIES caps;
    // mutable XINPUT_BATTERY_INFORMATION batteryInfo;

    //==============================================================================
    bool updateState() const
    {
        auto hr = RoInitialize(RO_INIT_MULTITHREADED);
        jassert(SUCCEEDED(hr));

        ComPtr<IGamepadStatics> gamepadStatics;
        hr = RoGetActivationFactory(HStringReference(L"Windows.Gaming.Input.Gamepad").Get(), __uuidof(IGamepadStatics), &gamepadStatics);
        jassert(SUCCEEDED(hr));

        ComPtr<IVectorView<Gamepad*>> gamepads;
        hr = gamepadStatics->get_Gamepads(&gamepads);
        jassert(SUCCEEDED(hr));

        uint32_t gamepadCount;
        hr = gamepads->get_Size(&gamepadCount);
        jassert(SUCCEEDED(hr));

        for (uint32_t i = 0; i < gamepadCount; i++)
        {
            ComPtr<IGamepad> gamepad;
            hr = gamepads->GetAt(i, &gamepad);
            jassert(SUCCEEDED(hr));

            GamepadReading gamepadReading;
            hr = gamepad->GetCurrentReading(&gamepadReading);
            jassert(SUCCEEDED(hr));

            std::cout << "Gamepad " << i + 1 << " buttons value is: " << gamepadReading.Buttons << std::endl;
        }

        return false;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

} // namespace juce
