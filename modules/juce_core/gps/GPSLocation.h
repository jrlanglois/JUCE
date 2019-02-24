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

        @see isNull
    */
    GPSLocation() = default;

    /** Creates a GPS location based on the provided coordinates and altitude. */
    GPSLocation (float latitude, float longitude, double fAltitude = 0.0);

    /** Creates a GPS location based on the provided coordinates and altitude. */
    GPSLocation (Point<float> coordinate, double fAltitude = 0.0);

    /** Copy constructor.  */
    GPSLocation (const GPSLocation &rOther);

    /** Destructor. */
    ~GPSLocation() = default;

    /** @returns true if the location can be retrieved.
        This can return false if the platform doesn't have location support or has location disabled.
    */
    static bool isLocationAvailable();

    /** @returns the current location of the platform's GPS.
        The instance returned might be null if this feature is not available
        due to platform limitations or lacking platform permissions.
    */
    static GPSLocation getCurrentLocation();

    /** @returns true if the location is null (latitude: 0.0, longitude: 0.0).

        The vertical axis is irrespective of the location and so the
        altitude is not considered part of the null-ness.

        Technically this location is valid, although highly unlikely to reach
        and probably will end up unrelated to the app, assuming there is even
        a GPS and/or cell network connection:
        @see https://en.wikipedia.org/wiki/Null_Island
    */
    bool isNull() const;

    GPSLocation &operator= (const GPSLocation &rOther);
    bool operator== (const GPSLocation &rOther) const;
    bool operator!= (const GPSLocation &rOther) const;

    Point<float> coordinate;     //< The geographic location, where x is the latitude and y is the longitude.
    double altitude = 0.0;       //< The altitude. Though typically this is in meters, it may be platform dependant.
};

}
