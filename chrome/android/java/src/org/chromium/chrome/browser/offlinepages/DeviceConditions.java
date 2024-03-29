// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.offlinepages;

/** Device network and power conditions. */
public class DeviceConditions {
    private final boolean mPowerConnected;
    private final int mBatteryPercentage;
    private final int mNetConnectionType;

    /**
     * Creates set of device network and power conditions.
     * @param powerConnected whether device is connected to power
     * @param batteryPercentage percentage (0-100) of remaining battery power
     * @param connectionType the org.chromium.net.ConnectionType value for the network connection
     */
    public DeviceConditions(boolean powerConnected, int batteryPercentage, int netConnectionType) {
        mPowerConnected = powerConnected;
        mBatteryPercentage = batteryPercentage;
        mNetConnectionType = netConnectionType;
    }

    /** @return Whether power is connected. */
    public boolean isPowerConnected() {
        return mPowerConnected;
    }

    /** @return Battery percentage. */
    public int getBatteryPercentage() {
        return mBatteryPercentage;
    }

    /**
     * @return Network connection type, where possible values are defined by
     *     org.chromium.net.ConnectionType.
     */
    public int getNetConnectionType() {
        return mNetConnectionType;
    }
}
