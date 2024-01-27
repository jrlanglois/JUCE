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
        playerIndex (indexOrId)
    {
        updateState();
    }

    ~Pimpl()
    {
    }

    //==============================================================================
    constexpr int getID() const                                     { return playerIndex; }
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

        String name;

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
                break;

                case DirectionalPadButton::right:
                break;

                case DirectionalPadButton::down:
                break;

                case DirectionalPadButton::left:
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
                break;

                case DirectionalPadButton::right:
                break;

                case DirectionalPadButton::down:
                break;

                case DirectionalPadButton::left:
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
                break;

                case ActionButton::right:
                break;

                case ActionButton::down:
                break;

                case ActionButton::left:
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
                break;

                case ActionButton::right:
                break;

                case ActionButton::down:
                break;

                case ActionButton::left:
                break;

                default:
                    jassertfalse;
                break;
            };
        }

        return 0.0;
    }

    //==============================================================================
    Point<double> getAxis (bool isLeft) const
    {
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
    int playerIndex = 0;

    //==============================================================================
    bool updateState() const
    {
        return false;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

} // namespace juce
