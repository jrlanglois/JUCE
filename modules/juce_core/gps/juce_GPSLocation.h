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

//==============================================================================
/** Represents a GPS location that provides the coordinates and altitude.

    Call getCurrentLocation to get the platform's GPS location.
*/
class GPSLocation final
{
public:
    /** Represents a null GPS location.

        The latitude and longitude will be infinity.

        @see isNull
    */
    GPSLocation();

    /** Creates a GPS location based on the provided coordinates (in degrees), and the altitude. */
    GPSLocation (double latitude, double longitude, double altitude = 0.0);

    /** Copy constructor. */
    GPSLocation (const GPSLocation& other);

    /** Destructor. */
    ~GPSLocation() = default;

    /** @returns true if the operation succeeded and that the user accepted.
        This will return false if the platform doesn't have location support or the user didn't want to give access.

        @coarseLocation This is only applicable to non-Apple platforms.
                        If false, this requests fine location handling.
    */
    static bool requestLocationPermissions (bool coarseLocation);

    /** @returns true if the location can be retrieved.
        This can return false if the platform doesn't have location support or has location disabled.

        @see requestLocationPermissions
    */
    static bool isLocationAvailable();

    /** @returns the current location of the platform's GPS.
        The instance returned might be null if this feature is not available
        due to platform limitations or lacking platform permissions.

        @see requestLocationPermissions
    */
    static GPSLocation getCurrentLocation();

    /** @returns true if the location is null.

        In the case of a null representation, the latitude and longitude
        will be set to infinity.

        The vertical axis is irrespective of the location and so the
        altitude is not considered part of the null-ness.
    */
    bool isNull() const;

    GPSLocation& operator= (const GPSLocation& other);
    bool operator== (const GPSLocation& other) const;
    bool operator!= (const GPSLocation& other) const;

    double latitude, longitude; //< The coordinates in degrees.
    double altitude = 0.0;      //< The altitude. Though typically this is in meters, it is platform dependant.
};

}
