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

NetworkConnectivityChecker::NetworkConnectivityChecker()                { networkType = getCurrentNetworkType(); start(); }
void NetworkConnectivityChecker::start (int intervalMs)                 { startTimer (intervalMs); }
void NetworkConnectivityChecker::stop()                                 { stopTimer(); }
bool NetworkConnectivityChecker::isConnectedToInternet() const          { return getCurrentNetworkType() != NetworkType::none; }
void NetworkConnectivityChecker::addListener (NetworkListener* l)       { listeners.add (l); }
void NetworkConnectivityChecker::removeListener (NetworkListener* l)    { listeners.remove (l); }

NetworkConnectivityChecker::NetworkType NetworkConnectivityChecker::getCurrentNetworkType() const
{
    return getCurrentSystemNetworkType();
}

void NetworkConnectivityChecker::notifyListeners()
{
    listeners.call ([&] (NetworkConnectivityChecker::NetworkListener& l) { l.networkStatusChanged(); });
}

void NetworkConnectivityChecker::timerCallback()
{
    const auto currentNetworkType = getCurrentNetworkType();

    //NB: This weird looking condition is just checking if the last known state
    //    was connected and now it's disconnected, or vice-versa.
    //    We don't care about the type of network in this case, just that it has changed.
    //    ie: "connected" was 1 now 0, or was 0 now 1
    if ((networkType != NetworkType::none) != (currentNetworkType != NetworkType::none))
    {
        networkType = currentNetworkType;
        notifyListeners();
    }
}

double NetworkConnectivityChecker::getRSSI()
{
    const auto currentNetworkType = getCurrentNetworkType();
    if (currentNetworkType == NetworkType::none)
        return 0.0;

    if (currentNetworkType != NetworkType::wifi && currentNetworkType != NetworkType::mobile)
        return 1.0;

    return getCurrentSystemRSSI();
}

} //juce
