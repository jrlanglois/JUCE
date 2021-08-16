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

/** Represents a real-time gamepad source.

    To obtain a gamepad's instance... TODO

    @see https://docs.microsoft.com/en-us/windows/win32/xinput/programming-guide
    @see https://developer.apple.com/documentation/gamecontroller
    @see https://github.com/elanthis/gamepad/blob/master/gamepad.c
*/
class  JUCE_API  GamepadInputSource  final
{
public:
    //==============================================================================
    /** TODO should be private. */
    GamepadInputSource (int);

    /** Destructor. */
    ~GamepadInputSource();

    //==============================================================================
    /** @returns the ID used to represent the gamepad.
        On Windows and Apple platforms, this is an index of 0 to 3.
    */
    int getID() const;

    /** @returns */
    String getName() const;

    /** @returns */
    bool isConnected() const;

    /** @returns */
    bool isWireless() const;

    //==============================================================================
    /** Use this to help determine the layout of the controller.

        For example, the Nintendo Switch's controllers have the X, Y, A, B buttons laid out
        in a different order than that of the Xbox controllers.

        @see ActionButton
    */
    enum class Type
    {
        none,
        unknown,
        xbox,
        playstation,
        nintendo,
        siriRemote,
        appleRemote
    };

    /** @returns the type of gamepad. This will return 'none' if disconnected. */
    Type getType() const;

    /** Not all gamepads are created equal!
        This enumeration gives an assortment of options to help
        determine layouts with more granularity, and of course to help
        iron out any UI/UX as needed.
    */
    enum class Subtype
    {
        none,
        unknown,
        gamepad,
        remote,
        racewheel,
        flightstick,
        arcadePad,
        arcadeStick,
        dancePad,
        guitar,
        guitarAlternate,
        bass,
        drumKit
    };

    /** @returns the subtype of gamepad. This will return 'none' if disconnected. */
    Subtype getSubtype() const;

    //==============================================================================
    /** */
    enum class BatteryType
    {
        none,       // This device is not connected
        unknown,    // Cannot determine the battery type
        wired,      // Wired device, no battery
        alkaline,   // Alkaline battery source
        nimh        // Nickel Metal Hydride battery source
    };

    /** @returns */
    BatteryType getBatteryType() const;

    /** @returns a normalised value; from 0.0 (empty) to 1.0 (full). */
    double getBatteryLevel() const;

    //==============================================================================
    /** @returns true if the gamepad instance is virtual, meaning it's not a physical device. */
    bool isVirtual() const;

    /** @returns true if the gamepad instance supports voice I/O. */
    bool supportsVoice() const;

    /** @returns true if the gamepad instance supports detecting its orientation. */
    bool supportsOrientation() const;

    /** @returns true if the gamepad instance supports detecting motion. */
    bool supportsMotion() const;

    /** @returns true if the gamepad instance has a light */
    bool hasLight() const;

    /** @returns the light colour if the gamepad instance supports it. */
    Colour getLightColour() const;

    //==============================================================================
    /** */
    enum class DirectionalPadButton
    {
        up,
        right,
        down,
        left
    };

    /** @returns */
    bool isDirectionalPadButtonDown (DirectionalPadButton) const;

    /** Some controllers may support ranges for some or all of their directional pad buttons.

        For the buttons that don't have this option,
        this will simply return a 0 (is up) or 1 (is down).

        @returns a normalised value.
    */
    double getDirectionalPadButtonDepression (DirectionalPadButton) const;

    //==============================================================================
    /** A list of possible action buttons that a controller may have.

        @warning This attempts to generically map the buttons on the "right-hand"
                 side of the controller. It's not entirely consistent across systems,
                 so be prepared to call getType to discern the layout!

        @see Type, getType
    */
    enum class ActionButton
    {
        /** 'Y' on Xbox controllers,
            'X' on Nintendo Switch controllers,
            'Triangle' on Playstation controllers,
            'C-Up' on Nintendo64 controllers.
        */
        up,

        /** 'B' on Xbox controllers,
            'A' on Switch controllers,
            'Circle' on Playstation controllers,
            'C-Right' on Nintendo64 controllers.
        */
        right,

        /** 'A' on Xbox controllers,
            'B' on Switch controllers,
            'X' on Playstation controllers,
            'C-Down' on Nintendo64 controllers.
        */
        down,

        /** 'X' on Xbox controllers,
            'Y' on Switch controllers,
            'Square' on Playstation controllers,
            'C-Left' on Nintendo64 controllers.
        */
        left
    };

    /** @returns */
    bool isActionButtonDown (ActionButton) const;

    /** Some controllers may support ranges for some or all of their action buttons.

        For the buttons that don't have this option,
        this will simply return a 0 (is up) or 1 (is down).

        @returns a normalised value.
    */
    double getActionButtonDepression (ActionButton) const;

    //==============================================================================
    /** Also known generally as bumpers. */
    enum class ShoulderButton
    {
        /** 'LT' on Xbox controllers,
            'L Button' on Switch controllers,
            'L1' on Playstation controllers,
            'L' on Nintendo64 controllers.
        */
        leftTrigger,

        /** 'LB' on Xbox controllers,
            '-' on Switch controllers,
            'L2' on Playstation controllers.
        */ 
        leftBumper,

        /** 'LSB' on Xbox controllers,
            'Left Stick' on Switch controllers,
            'L3' on Playstation controllers.
        */
        leftAnalog,

        /** 'RT' on Xbox controllers,
            'R Button' on Switch controllers,
            'R1' on Playstation controllers,
            'R' on Nintendo64 controllers.
        */
        rightTrigger,

        /** 'RB' on Xbox controllers,
            '+' on Switch controllers,
            'R2' on Playstation controllers.
        */
        rightBumper,
  
        /** 'RSB' on Xbox controllers,
            'Right Stick' on Switch controllers,
            'R3' on Playstation controllers.
        */
        rightAnalog
    };

    /** @returns */
    bool isShoulderButtonDown (ShoulderButton) const;

    /** Some controllers may support ranges for some or all of their shoulder buttons.

        For the buttons that don't have this option,
        this will simply return a 0 (is up) or 1 (is down).

        @returns a normalised value.
    */
    double getShoulderButtonDepression (ShoulderButton) const;

    //==============================================================================
    /** This is a short-list of the various media control buttons that can be available,
        and of course the typical start/select buttons.
    */
    enum class SpecialButton
    {
        start,
        select,
        back,
        centre,
        guide,
        blue,
        red,
        green,
        yellow,
        volumeUp,
        volumeDown,
        volumeMute,
        channelUp,
        channelDown,
        playPause,
        stop,
        rewind,
        fastForward,
        previousTrack,
        nextTrack
    };

    /** */
    bool isSpecialButtonDown (SpecialButton) const;

    /** Some controllers may support ranges for some or all of their special buttons.

        For the buttons that don't have this option,
        this will simply return a 0 (is up) or 1 (is down).

        @returns a normalised value.
    */
    double getSpecialButtonDepression (SpecialButton) const;

    //==============================================================================
    /** @returns */
    Point<double> getLeftAxis (bool ignoreDeadzone = true) const;

    /** @returns */
    Point<double> getLeftAxisDeadZone() const;

    /** */
    void setLeftAxisDeadZone (Point<double>);

    /** @returns */
    Point<double> getRightAxis (bool ignoreDeadzone = true) const;

    /** @returns */
    Point<double> getRightAxisDeadZone() const;

    /** */
    void setRightAxisDeadZone (Point<double>);

    //==============================================================================
    /** */
    void setLeftMotorVibration (double);

    /** */
    double getLeftMotorVibration() const;

    /** */
    void setRightMotorVibration (double);

    /** */
    double getRightMotorVibration() const;

    /** */
    void clearVibrations();

    //==============================================================================
    /** */
    enum class ThreeAxisUnit
    {
        x, y, z
    };

    /** @returns */
    bool hasAccelerometer() const;

    /** @returns */
    double getAccelerometerValue (ThreeAxisUnit) const;

    /** @returns */
    bool hasGyroscope() const;

    /** @returns */
    double getGyroscopeValue (ThreeAxisUnit) const;

private:
    //==============================================================================
    class Pimpl;
    std::unique_ptr<Pimpl> pimpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GamepadInputSource)
};

} // namespace juce
