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

#if JUCE_IOS

bool GPSLocation::isLocationAvailable()
{
    return [CLLocationManager locationServicesEnabled]
        && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusDenied
        && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusRestricted;
}

GPSLocation GPSLocation::getCurrentLocation()
{
    DTVAppDelegate *pApp = (DTVAppDelegate *)[[UIApplication sharedApplication] delegate];

    if (CLLocationManager *pLocationManager = [pApp getLocationManager])
    {
        if ([CLLocationManager authorizationStatus] == kCLAuthorizationStatusNotDetermined)
        {
            [pLocationManager requestWhenInUseAuthorization];
        }

        if (IsLocationAvailable())
        {
            [pLocationManager requestLocation];

            return  {
                        static_cast<float>(pLocationManager.location.coordinate.latitude),
                        static_cast<float>(pLocationManager.location.coordinate.longitude),
                        static_cast<double>(pLocationManager.location.altitude
                    };
        }
    }

    return {};
}

#endif // YI_IOS

}