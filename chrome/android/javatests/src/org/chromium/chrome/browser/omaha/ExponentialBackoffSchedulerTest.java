// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.omaha;

import android.content.Context;
import android.content.Intent;
import android.support.test.filters.SmallTest;
import android.test.InstrumentationTestCase;

import org.chromium.base.test.util.AdvancedMockContext;
import org.chromium.base.test.util.Feature;

/** Tests the ExponentialBackoffScheduler. */
public class ExponentialBackoffSchedulerTest extends InstrumentationTestCase {
    private static final String INTENT_STRING = "schedulerIntent";
    private static final String PREFERENCE_NAME = "scheduler";
    private static final long BACKOFF_MS = 15000;
    private static final long MAX_MS = 1000000;

    /**
     * Checks that the correct number of failures are set/reset.
     */
    @SmallTest
    @Feature({"Omaha", "Sync"})
    public void testExponentialBackoffSchedulerFailureSetting() {
        Context targetContext = getInstrumentation().getTargetContext();
        TestContext context = new TestContext(targetContext);

        ExponentialBackoffScheduler writer =
                new ExponentialBackoffScheduler(PREFERENCE_NAME, context, BACKOFF_MS, MAX_MS);
        ExponentialBackoffScheduler reader =
                new ExponentialBackoffScheduler(PREFERENCE_NAME, context, BACKOFF_MS, MAX_MS);

        assertEquals("Expected no failures for freshly created class", 0,
                reader.getNumFailedAttempts());
        writer.increaseFailedAttempts();
        writer.increaseFailedAttempts();
        assertEquals("Expected 2 failures after 2 increments.", 2, reader.getNumFailedAttempts());
        writer.resetFailedAttempts();
        assertEquals("Expected 0 failures after reset.", 0, reader.getNumFailedAttempts());
    }

    /**
     * Check that the delay generated by the scheduler is within the correct range.
     */
    @SmallTest
    @Feature({"Omaha", "Sync"})
    public void testExponentialBackoffSchedulerDelayCalculation() {
        Context targetContext = getInstrumentation().getTargetContext();
        TestContext context = new TestContext(targetContext);
        MockExponentialBackoffScheduler scheduler =
                new MockExponentialBackoffScheduler(PREFERENCE_NAME, context, BACKOFF_MS, MAX_MS);

        Intent intent = new Intent(INTENT_STRING);
        scheduler.createAlarm(intent, scheduler.calculateNextTimestamp());

        // With no failures, expect the base backoff delay.
        long delay = scheduler.getAlarmTimestamp() - scheduler.getCurrentTime();
        assertEquals("Expected delay of " + BACKOFF_MS + " milliseconds.", BACKOFF_MS,
                delay);

        // With two failures, expect a delay within [BACKOFF_MS, BACKOFF_MS * 2^2].
        scheduler.increaseFailedAttempts();
        scheduler.increaseFailedAttempts();
        scheduler.createAlarm(intent, scheduler.calculateNextTimestamp());

        delay = scheduler.getAlarmTimestamp() - scheduler.getCurrentTime();
        final long minDelay = BACKOFF_MS;
        final long maxDelay = BACKOFF_MS * (1 << scheduler.getNumFailedAttempts());
        assertTrue("Expected delay greater than the minimum.", delay >= minDelay);
        assertTrue("Expected delay within maximum of " + maxDelay, delay <= maxDelay);
    }

    /**
     * Check that the alarm is being set by the class.
     */
    @SmallTest
    @Feature({"Omaha", "Sync"})
    public void testExponentialBackoffSchedulerAlarmCreation() {
        Context targetContext = getInstrumentation().getTargetContext();
        TestContext context = new TestContext(targetContext);

        MockExponentialBackoffScheduler scheduler =
                new MockExponentialBackoffScheduler(PREFERENCE_NAME, context, BACKOFF_MS, MAX_MS);

        Intent intent = new Intent(INTENT_STRING);
        scheduler.createAlarm(intent, scheduler.calculateNextTimestamp());
        assertTrue("Never requested the alarm manager.", context.mRequestedAlarmManager);
        assertTrue("Never received a call to set the alarm.", scheduler.getAlarmWasSet());
    }

    /**
     * Ensures that the AlarmManager is the only service requested.
     */
    private static class TestContext extends AdvancedMockContext {
        public boolean mRequestedAlarmManager;

        public TestContext(Context context) {
            super(context);
        }

        /**
         * Checks that we're requesting the AlarmManager.
         * @param name Name of the service.  Should be the AlarmManager's service name.
         * @return null since we can't create an AlarmManager.
         */
        @Override
        public Object getSystemService(final String name) {
            assertTrue("Requested service other than AlarmManager.",
                    Context.ALARM_SERVICE.equals(name));
            mRequestedAlarmManager = true;
            return super.getSystemService(name);
        }
    }
}
