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
#if JUCE_ENABLE_GPS_LOCATION_SERVICES && (JUCE_MAC || JUCE_IOS)

bool GPSLocation::requestLocationPermissions (bool)
{
   #if JUCE_IOS && (defined (__IPHONE_7_0) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_7_0)
    if (! isLocationAvailable())
    {
        id<NSApplicationDelegate> appDelegate = [[NSApplication sharedApplication] delegate];

        if (auto* locationManager = [appDelegate locationManager])
            [locationManager requestWhenInUseAuthorization];
    }
   #endif

    return isLocationAvailable();
}

bool GPSLocation::isLocationAvailable()
{
    return [CLLocationManager locationServicesEnabled]
        && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusNotDetermined
        && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusDenied
        && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusRestricted;
}

GPSLocation GPSLocation::getCurrentLocation()
{
    //If you reach this, you need to prompt the user to enable using the location services!
    jassert ([CLLocationManager authorizationStatus] != kCLAuthorizationStatusNotDetermined);

    id<NSApplicationDelegate> appDelegate = [[NSApplication sharedApplication] delegate];

    if (auto* locationManager = [appDelegate locationManager])
        locationManager.startUpdatingLocation();

    if (isLocationAvailable())
    {
        id<NSApplicationDelegate> appDelegate = [[NSApplication sharedApplication] delegate];
        if (auto* locationManager = [appDelegate getLocationManager])
        {
            return
            {
                static_cast<double> (locationManager.location.coordinate.latitude),
                static_cast<double> (locationManager.location.coordinate.longitude),
                static_cast<double> (locationManager.location.altitude)
            };
        }
    }

    return {};
}

#endif

}
