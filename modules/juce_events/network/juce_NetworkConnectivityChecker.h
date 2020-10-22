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

/** Use an instance of this class to find out details about internet connectivity,
    and register a NetworkListener to find out when connectivity changes.
*/
class JUCE_API  NetworkConnectivityChecker  final : private Timer
{
public:
    /** Default constructor. */
    NetworkConnectivityChecker();

    //==============================================================================
    /** Starts checking for network connectivity changes, such as the network type changing,
        and if the network goes up or down.

        @intervalMs The interval to check for a change in connectivity.
                    If the checking was already started, this will simply change the interval.
    */
    void start (int intervalMs = 2500);

    /** Stops checking for connectivity changes. */
    void stop();

    /** @returns the last known network type. */
    NetworkDetails::NetworkType getLastKnownNetworkType() const { return networkType; }

    //==============================================================================
    /** */
    class NetworkListener
    {
    public:
        virtual ~NetworkListener() {}

        /** */
        virtual void networkStatusChanged (NetworkDetails::NetworkType newNetworkType) = 0;
    };

    /** */
    void addListener (NetworkListener* listener);

    /** */
    void removeListener (NetworkListener* listener);

    /** You can assign a lambda to this callback object to have it called when the NetworkType is changed. */
    std::function<void (NetworkDetails::NetworkType)> onChange;

private:
    //==============================================================================
    NetworkDetails::NetworkType networkType = NetworkDetails::NetworkType::none;
    ListenerList<NetworkListener> listeners;

    void notifyListeners();
    void timerCallback() override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkConnectivityChecker)
};

} // namespace juce
