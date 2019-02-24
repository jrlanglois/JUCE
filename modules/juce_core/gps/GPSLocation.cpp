/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

GPSLocation::GPSLocation(float latitude, float longitude, double a)
  : GPSLocation({ latitude, longitude }, a)
{
}

GPSLocation::GPSLocation (Point<float> coords, double a)
  : coordinate (coords),
    altitude (a)
{
}

GPSLocation::GPSLocation (const GPSLocation &other)
  : coordinate (other.coordinate),
    altitude (other.altitude)
{
}

GPSLocation &GPSLocation::operator= (const GPSLocation &other)
{
    coordinate = other.coordinate;
    altitude = other.altitude;
    return *this;
}

bool GPSLocation::operator== (const GPSLocation &other) const
{
    return coordinate == other.coordinate && altitude == other.altitude;
}

bool GPSLocation::operator!= (const GPSLocation &other) const
{
    return ! operator== (other);
}

bool GPSLocation::isNull() const
{
    return coordinate.x == 0.0f && coordinate.y == 0.0f;
}

#if JUCE_IOS

/**
    Please see GPSLocation.mm for the Apple platform implementation!
*/

#elif JUCE_ANDROID

#include "app/android/YiAndroidCommon.h"

bool GPSLocation::isLocationAvailable()
{
    auto* jniEnv = GetJNIEnv();

    static jmethodID _isLocationAvailable = nullptr;
    if (_isLocationAvailable == nullptr)
        _isLocationAvailable = jniEnv->GetMethodID (GetJNIActivityClass(), "_isLocationAvailable", "()Z");

    return jniEnv->CallBooleanMethod (cachedActivity, _isLocationAvailable);
}

GPSLocation GPSLocation::getCurrentLocation()
{
    auto* jniEnv = GetJNIEnv();

    static jmethodID _getGPSLocation = nullptr;
    if (_getGPSLocation == nullptr)
        _getGPSLocation = jniEnv->GetMethodID (GetJNIActivityClass(), "_getGPSLocation", "()[D");

    auto javaGPSCurrentLocationResult = (jdoubleArray) jniEnv->CallObjectMethod (cachedActivity, _getGPSLocation);
    if (javaGPSCurrentLocationResult == nullptr)
        return {}; //If this is reached, the GPS permission was probably disabled but it's hard to say exactly...

    auto numItems = jniEnv->GetArrayLength (javaGPSCurrentLocationResult);
    std::vector<double> input (numItems);
    jniEnv->GetDoubleArrayRegion (javaGPSCurrentLocationResult, 0, numItems, &input[0]);

    GPSLocation result;
    decltype (numItems) index = 0;
    if (index < numItems)   result.coordinate.x = (float) input[index++];
    if (index < numItems)   result.coordinate.y = (float) input[index++];
    if (index < numItems)   result.altitude     = (double) input[index++];

    return result;
}

#else

bool GPSLocation::isLocationAvailable()
{
    return false;
}

GPSLocation GPSLocation::getCurrentLocation()
{
    return {};
}

#endif

}