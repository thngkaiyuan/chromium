// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/new_session_cdm_result_promise.h"

#include "base/logging.h"
#include "base/metrics/histogram_functions.h"
#include "media/blink/cdm_result_promise_helper.h"
#include "third_party/WebKit/public/platform/WebString.h"

namespace media {

const char kTimeUMAPrefix[] = "TimeTo.";

static blink::WebContentDecryptionModuleResult::SessionStatus ConvertStatus(
    SessionInitStatus status) {
  switch (status) {
    case SessionInitStatus::UNKNOWN_STATUS:
      break;
    case SessionInitStatus::NEW_SESSION:
      return blink::WebContentDecryptionModuleResult::NewSession;
    case SessionInitStatus::SESSION_NOT_FOUND:
      return blink::WebContentDecryptionModuleResult::SessionNotFound;
    case SessionInitStatus::SESSION_ALREADY_EXISTS:
      return blink::WebContentDecryptionModuleResult::SessionAlreadyExists;
  }
  NOTREACHED();
  return blink::WebContentDecryptionModuleResult::SessionNotFound;
}

NewSessionCdmResultPromise::NewSessionCdmResultPromise(
    const blink::WebContentDecryptionModuleResult& result,
    const std::string& key_system_uma_prefix,
    const std::string& uma_name,
    const SessionInitializedCB& new_session_created_cb)
    : web_cdm_result_(result),
      key_system_uma_prefix_(key_system_uma_prefix),
      uma_name_(uma_name),
      new_session_created_cb_(new_session_created_cb),
      creation_time_(base::TimeTicks::Now()) {}

NewSessionCdmResultPromise::~NewSessionCdmResultPromise() {
  if (!IsPromiseSettled())
    RejectPromiseOnDestruction();
}

void NewSessionCdmResultPromise::resolve(const std::string& session_id) {
  // |new_session_created_cb_| uses a WeakPtr<> and may not do anything
  // if the session object has been destroyed.
  SessionInitStatus status = SessionInitStatus::UNKNOWN_STATUS;
  new_session_created_cb_.Run(session_id, &status);

  if (status == SessionInitStatus::UNKNOWN_STATUS) {
    reject(INVALID_STATE_ERROR, 0, "Cannot finish session initialization");
    return;
  }

  MarkPromiseSettled();
  ReportCdmResultUMA(key_system_uma_prefix_ + uma_name_, SUCCESS);

  // Only report time for promise resolution (not rejection).
  base::UmaHistogramTimes(key_system_uma_prefix_ + kTimeUMAPrefix + uma_name_,
                          base::TimeTicks::Now() - creation_time_);

  web_cdm_result_.completeWithSession(ConvertStatus(status));
}

void NewSessionCdmResultPromise::reject(CdmPromise::Exception exception_code,
                                        uint32_t system_code,
                                        const std::string& error_message) {
  MarkPromiseSettled();
  ReportCdmResultUMA(uma_name_,
                     ConvertCdmExceptionToResultForUMA(exception_code));
  web_cdm_result_.completeWithError(ConvertCdmException(exception_code),
                                    system_code,
                                    blink::WebString::fromUTF8(error_message));
}

}  // namespace media
