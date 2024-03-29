// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/translate/translate_ranker_metrics_provider.h"

#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/translate/translate_ranker_factory.h"
#include "components/metrics/proto/chrome_user_metrics_extension.pb.h"
#include "components/metrics/proto/translate_event.pb.h"
#include "components/translate/core/browser/translate_ranker.h"

namespace translate {

void TranslateRankerMetricsProvider::ProvideGeneralMetrics(
    metrics::ChromeUserMetricsExtension* uma_proto) {
  for (auto* profile : ProfileManager::GetLastOpenedProfiles()) {
    TranslateRanker* ranker =
        TranslateRankerFactory::GetInstance()->GetForBrowserContext(profile);
    if (!ranker)
      return;

    std::vector<metrics::TranslateEventProto> translate_events;
    ranker->FlushTranslateEvents(&translate_events);

    for (auto& event : translate_events) {
      uma_proto->add_translate_event()->Swap(&event);
    }
  }
}

}  // namespace translate
