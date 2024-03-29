/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "modules/mediasource/MediaSourceRegistry.h"

#include "modules/mediasource/MediaSource.h"
#include "platform/weborigin/KURL.h"

namespace blink {

MediaSourceRegistry& MediaSourceRegistry::registry() {
  DCHECK(isMainThread());
  DEFINE_STATIC_LOCAL(MediaSourceRegistry, instance, ());
  return instance;
}

void MediaSourceRegistry::registerURL(SecurityOrigin*,
                                      const KURL& url,
                                      URLRegistrable* registrable) {
  DCHECK_EQ(&registrable->registry(), this);
  DCHECK(isMainThread());

  MediaSource* source = static_cast<MediaSource*>(registrable);
  source->addedToRegistry();
  m_mediaSources.set(url.getString(), source);
}

void MediaSourceRegistry::unregisterURL(const KURL& url) {
  DCHECK(isMainThread());
  PersistentHeapHashMap<String, Member<MediaSource>>::iterator iter =
      m_mediaSources.find(url.getString());
  if (iter == m_mediaSources.end())
    return;

  MediaSource* source = iter->value;
  m_mediaSources.remove(iter);
  source->removedFromRegistry();
}

URLRegistrable* MediaSourceRegistry::lookup(const String& url) {
  DCHECK(isMainThread());
  return url.isNull() ? nullptr : m_mediaSources.at(url);
}

MediaSourceRegistry::MediaSourceRegistry() {
  HTMLMediaSource::setRegistry(this);
}

}  // namespace blink
