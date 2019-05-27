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

GPSLocation::GPSLocation()
  : latitude (std::numeric_limits<double>::infinity()),
    longitude (std::numeric_limits<double>::infinity())
{
}

GPSLocation::GPSLocation (double lat, double lo, double a)
  : latitude (lat),
    longitude (lo),
    altitude (a)
{
}

GPSLocation::GPSLocation (const GPSLocation& other)
  : GPSLocation()
{
    operator= (other);
}

GPSLocation& GPSLocation::operator= (const GPSLocation& other)
{
    latitude = other.latitude;
    longitude = other.longitude;
    altitude = other.altitude;
    return *this;
}

bool GPSLocation::operator== (const GPSLocation& other) const
{
    return latitude == other.latitude
        && longitude == other.longitude
        && altitude == other.altitude;
}

bool GPSLocation::operator!= (const GPSLocation& other) const
{
    return ! operator== (other);
}

bool GPSLocation::isNull() const
{
    return latitude == std::numeric_limits<double>::infinity()
        && longitude == std::numeric_limits<double>::infinity();
}

#if JUCE_ENABLE_GPS_LOCATION_SERVICES && (JUCE_MAC || JUCE_IOS)

/**
    Please see juce_GPSLocation.mm for the Apple platform implementations!
*/

#elif JUCE_ENABLE_GPS_LOCATION_SERVICES && JUCE_WINDOWS

#pragma comment (lib, "locationapi.lib")

bool GPSLocation::requestLocationPermissions (bool coarseLocation)
{
    ComSmartPtr<ILocation> ilocation;
    if (SUCCEEDED (ilocation.CoCreateInstance (CLSID_Location)))
    {
        IID types[] = { coarseLocation ? IID_IDefaultLocation : IID_ILatLongReport };
        return SUCCEEDED (ilocation->RequestPermissions (nullptr, types, (ULONG) 1, TRUE));
    }

    return false;
}

bool GPSLocation::isLocationAvailable()
{
    ComSmartPtr<ILocation> ilocation;
    if (SUCCEEDED (ilocation.CoCreateInstance (CLSID_Location)))
    {
        auto status = REPORT_NOT_SUPPORTED;
        return SUCCEEDED (ilocation->GetReportStatus (IID_ILatLongReport, &status)) && status == REPORT_RUNNING;
    }

    return false;
}

GPSLocation GPSLocation::getCurrentLocation()
{
    ComSmartPtr<ILocation> ilocation;
    if (FAILED (ilocation.CoCreateInstance (CLSID_Location)))
    {
        jassertfalse;
        return {};
    }

    ComSmartPtr<ILatLongReport> latLongReport;
    double latitude = 0.0, longitude = 0.0, altitude = 0.0;

    auto parseReport = [&]() -> bool
    {
        return latLongReport != nullptr
            && SUCCEEDED (latLongReport->GetLatitude (&latitude))
            && SUCCEEDED (latLongReport->GetLongitude (&longitude))
            && SUCCEEDED (latLongReport->GetAltitude (&altitude));
    };

    //Attempt getting the most up to date location:
    ComSmartPtr<ILocationReport> locationReport;

    if (SUCCEEDED (ilocation->GetReport (IID_ILatLongReport, locationReport.resetAndGetPointerAddress()))
        && SUCCEEDED (locationReport->QueryInterface (latLongReport.resetAndGetPointerAddress()))
        && parseReport())
    {
        return GPSLocation (latitude, longitude, altitude);
    }

    //Attempt getting the default location:
    ComSmartPtr<IDefaultLocation> defaultLocation;

    if (SUCCEEDED (defaultLocation.CoCreateInstance (CLSID_DefaultLocation))
        && SUCCEEDED (defaultLocation->QueryInterface (latLongReport.resetAndGetPointerAddress()))
        && parseReport())
    {
        return GPSLocation (latitude, longitude, altitude);
    }

    return {};
}

#elif JUCE_ENABLE_GPS_LOCATION_SERVICES && JUCE_ANDROID

bool GPSLocation::requestLocationPermissions (bool coarseLocation)
{
    jassertfalse; //TODO
    return false;
}

bool GPSLocation::isLocationAvailable()
{
    auto* env = getEnv();

    static jmethodID _isLocationAvailable = nullptr;
    if (_isLocationAvailable == nullptr)
        _isLocationAvailable = env->GetMethodID (GetJNIActivityClass(), "_isLocationAvailable", "()Z");

    if (_isLocationAvailable != nullptr)
        return env->CallBooleanMethod (cachedActivity, _isLocationAvailable);

    return false;
}

GPSLocation GPSLocation::getCurrentLocation()
{
    auto* env = getEnv();
    if (env == nullptr)
        return {};

    static jmethodID _getGPSLocation = nullptr;
    if (_getGPSLocation == nullptr)
        _getGPSLocation = env->GetMethodID (GetJNIActivityClass(), "_getGPSLocation", "()[D");

    if (_getGPSLocation == nullptr)
        return {}; //Possibly unimplemented or unsupported method...

    auto javaGPSCurrentLocationResult = (jdoubleArray) env->CallObjectMethod (cachedActivity, _getGPSLocation);
    if (javaGPSCurrentLocationResult == nullptr)
        return {}; //If this is reached, the GPS permission was probably disabled but it's hard to say exactly...

    auto numItems = env->GetArrayLength (javaGPSCurrentLocationResult);
    std::vector<double> input (numItems);
    env->GetDoubleArrayRegion (javaGPSCurrentLocationResult, 0, numItems, &input[0]);

    GPSLocation result;
    decltype (numItems) index = 0;
    if (index < numItems)   result.latitude     = (double) input[index++];
    if (index < numItems)   result.longitude    = (double) input[index++];
    if (index < numItems)   result.altitude     = (double) input[index++];

    return result;
}

#else

//Unsupported platform implementations...
bool GPSLocation::requestLocationPermissions (bool) { return false; }
bool GPSLocation::isLocationAvailable()             { return false; }
GPSLocation GPSLocation::getCurrentLocation()       { return {}; }

#endif

}
