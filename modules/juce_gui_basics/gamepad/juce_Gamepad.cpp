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

GamepadInputSource::GamepadInputSource (int indexOrId) :
    pimpl (new Pimpl (indexOrId))
{
    clearVibrations();
}

GamepadInputSource::~GamepadInputSource()
{
    // NB: Not doing this can leave the controller in a stuck state with vibrations running...
    clearVibrations();
}

int GamepadInputSource::getID() const                                                       { return pimpl->getID(); }
String GamepadInputSource::getName() const                                                  { return pimpl->getName(); }
bool GamepadInputSource::isConnected() const                                                { return pimpl->isConnected(); }
bool GamepadInputSource::isVirtual() const                                                  { return pimpl->isVirtual(); }
bool GamepadInputSource::supportsVoice() const                                              { return pimpl->supportsVoice(); }
bool GamepadInputSource::supportsOrientation() const                                        { return pimpl->supportsOrientation(); }
bool GamepadInputSource::supportsMotion() const                                             { return pimpl->supportsMotion(); }
bool GamepadInputSource::hasLight() const                                                   { return pimpl->hasLight(); }
Colour GamepadInputSource::getLightColour() const                                           { return pimpl->getLightColour(); }
bool GamepadInputSource::isWireless() const                                                 { return pimpl->isWireless(); }
GamepadInputSource::BatteryType GamepadInputSource::getBatteryType() const                  { return pimpl->getBatteryType(); }
double GamepadInputSource::getBatteryLevel() const                                          { return pimpl->getBatteryLevel(); }
GamepadInputSource::Type GamepadInputSource::getType() const                                { return pimpl->getType(); }
GamepadInputSource::Subtype GamepadInputSource::getSubtype() const                          { return pimpl->getSubtype(); }
bool GamepadInputSource::isDirectionalPadButtonDown (DirectionalPadButton b) const          { return pimpl->isDirectionalPadButtonDown (b); }
double GamepadInputSource::getDirectionalPadButtonDepression (DirectionalPadButton b) const { return pimpl->getDirectionalPadButtonDepression (b); }
bool GamepadInputSource::isActionButtonDown (ActionButton b) const                          { return pimpl->isActionButtonDown (b); }
double GamepadInputSource::getActionButtonDepression (ActionButton b) const                 { return pimpl->getActionButtonDepression (b); }
Point<double> GamepadInputSource::getLeftAxis (bool b) const                                { return pimpl->getLeftAxis (b); }
Point<double> GamepadInputSource::getLeftAxisDeadZone() const                               { return pimpl->getLeftAxisDeadZone(); }
void GamepadInputSource::setLeftAxisDeadZone (Point<double> p)                              { return pimpl->setLeftAxisDeadZone (p); }
Point<double> GamepadInputSource::getRightAxis (bool b) const                               { return pimpl->getRightAxis (b); }
Point<double> GamepadInputSource::getRightAxisDeadZone() const                              { return pimpl->getRightAxisDeadZone(); }
void GamepadInputSource::setRightAxisDeadZone (Point<double> p)                             { return pimpl->setRightAxisDeadZone (p); }
void GamepadInputSource::setLeftMotorVibration (double v)                                   { return pimpl->setLeftMotorVibration (v); }
double GamepadInputSource::getLeftMotorVibration() const                                    { return pimpl->getLeftMotorVibration(); }
void GamepadInputSource::setRightMotorVibration (double v)                                  { return pimpl->setRightMotorVibration (v); }
double GamepadInputSource::getRightMotorVibration() const                                   { return pimpl->getRightMotorVibration(); }
bool GamepadInputSource::isShoulderButtonDown (ShoulderButton b) const                      { return pimpl->isShoulderButtonDown (b); }
double GamepadInputSource::getShoulderButtonDepression (ShoulderButton b) const             { return pimpl->getShoulderButtonDepression (b); }
bool GamepadInputSource::isSpecialButtonDown (SpecialButton b) const                        { return pimpl->isSpecialButtonDown (b); }
double GamepadInputSource::getSpecialButtonDepression (SpecialButton b) const               { return pimpl->getSpecialButtonDepression (b); }
bool GamepadInputSource::hasAccelerometer() const                                           { return pimpl->hasAccelerometer(); }
double GamepadInputSource::getAccelerometerValue (ThreeAxisUnit u) const                    { return pimpl->getAccelerometerValue (u); }
bool GamepadInputSource::hasGyroscope() const                                               { return pimpl->hasGyroscope(); }
double GamepadInputSource::getGyroscopeValue (ThreeAxisUnit u) const                        { return pimpl->getGyroscopeValue (u); }

void GamepadInputSource::clearVibrations()
{
    setLeftMotorVibration (0.0);
    setRightMotorVibration (0.0);
}

} // namespace juce
