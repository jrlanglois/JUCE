/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

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

/** Use the static methods provided to find out various details about the current
    network connectivity, such as the wifi name (if we're connected to wifi),
    or if we're simply connected to a network of some kind.
*/
class JUCE_API  NetworkDetails  final
{
public:
    /** Describes a type of network connection. */
    enum class NetworkType
    {
        none,   /*!< Indicates that there is no network connection present. */
        wifi,   /*!< The current network connection is wifi. */
        wired,  /*!< The current network connection is wired, such as an ethernet connection. */
        mobile, /*!< The current network connection is mobile, such as a 3G, 4G, or 5G network. */
        other   /*!< The current network connection is active, but we don't know exactly what it is (eg: coffee machine?). */
    };

    /** @returns a native call to the most current network type. */
    static NetworkType getCurrentNetworkType();

    /** @returns true if a network connection is present, regardless of how it's present.

        @note This does NOT guarantee internet connectivity (eg: able to reach http://google.com).
              The purpose is to simply guarantee that that a network connection is made.
    */
    static bool isConnectedToNetwork();

    /** @returns the SSID, or current name of your network, if one is present. */
    static String getCurrentNetworkName();

    /** @returns all possible names of potential network devices
        you can connect to (assuming you're connected to a network).

        @param showWifiOnly By default this is set to true to only list possible wifi networks that you can connect to.
                            Set this to false to get everything possible, although you might end up a which a huge list!

        @note This may also contain the network you're presently connected to.
    */
    static StringArray getNetworkNames (bool showWifiOnly = true);

    /** @returns a normalised (0.0 to 1.0) Received Signal Strength Indicator.

        @note This is applicable to wifi and mobile data connectivity only.
    */
    static double getRSSI();

private:
    //==============================================================================
    static double getCurrentSystemRSSI();

    //==============================================================================
    NetworkDetails() = delete;
    JUCE_DECLARE_NON_COPYABLE (NetworkDetails)
};

} // namespace juce
