/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UnitTestHelpers_h
#define UnitTestHelpers_h

#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class SharedBuffer;

namespace testing {

// Note: You may want to use TestingPlatformSupportWithMockScheduler to
// provides runUntilIdle() method that can work with WebURLLoaderMockFactory.
void runPendingTasks();

// Waits for delayed task to complete or timers to fire for |delayMs|
// milliseconds.
void runDelayedTasks(double delayMs);

void enterRunLoop();
void exitRunLoop();

void yieldCurrentThread();

// Returns Blink top directory as an absolute path, e.g.
// /src/third_party/WebKit.
String blinkRootDir();

// Returns test data absolute path for webkit_unit_tests, i.e.
// <blinkRootDir>/Source/web/tests/data/<relativePath>.
// It returns the top web test directory if |relativePath| was not specified.
String webTestDataPath(const String& relativePath = String());

// Returns test data absolute path for blink_platform_unittests, i.e.
// <blinkRootDir>/Source/platform/testing/data/<relativePath>.
// It returns the top platform test directory if |relativePath| was not
// specified.
String platformTestDataPath(const String& relativePath = String());

PassRefPtr<SharedBuffer> readFromFile(const String& path);

}  // namespace testing
}  // namespace blink

#endif
