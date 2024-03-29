/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#include "core/events/ErrorEvent.h"

#include <memory>

#include "bindings/core/v8/V8Binding.h"
#include "v8/include/v8.h"

namespace blink {

ErrorEvent::ErrorEvent()
    : m_sanitizedMessage(),
      m_location(SourceLocation::create(String(), 0, 0, nullptr)),
      m_world(DOMWrapperWorld::current(v8::Isolate::GetCurrent())) {}

ErrorEvent::ErrorEvent(const AtomicString& type,
                       const ErrorEventInit& initializer)
    : Event(type, initializer),
      m_sanitizedMessage(),
      m_world(DOMWrapperWorld::current(v8::Isolate::GetCurrent())) {
  if (initializer.hasMessage())
    m_sanitizedMessage = initializer.message();
  m_location = SourceLocation::create(
      initializer.hasFilename() ? initializer.filename() : String(),
      initializer.hasLineno() ? initializer.lineno() : 0,
      initializer.hasColno() ? initializer.colno() : 0, nullptr);
  if (initializer.hasError())
    m_error = initializer.error();
}

ErrorEvent::ErrorEvent(const String& message,
                       std::unique_ptr<SourceLocation> location,
                       DOMWrapperWorld* world)
    : Event(EventTypeNames::error, false, true),
      m_sanitizedMessage(message),
      m_location(std::move(location)),
      m_world(world) {}

void ErrorEvent::setUnsanitizedMessage(const String& message) {
  DCHECK(m_unsanitizedMessage.isEmpty());
  m_unsanitizedMessage = message;
}

ErrorEvent::~ErrorEvent() {}

const AtomicString& ErrorEvent::interfaceName() const {
  return EventNames::ErrorEvent;
}

ScriptValue ErrorEvent::error(ScriptState* scriptState) const {
  // Don't return |m_error| when we are in the different worlds to avoid
  // leaking a V8 value.
  // We do not clone Error objects (exceptions), for 2 reasons:
  // 1) Errors carry a reference to the isolated world's global object, and
  //    thus passing it around would cause leakage.
  // 2) Errors cannot be cloned (or serialized):
  if (world() != &scriptState->world())
    return ScriptValue();
  return m_error;
}

DEFINE_TRACE(ErrorEvent) {
  Event::trace(visitor);
}

}  // namespace blink
