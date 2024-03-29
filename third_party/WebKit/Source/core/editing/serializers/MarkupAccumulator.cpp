/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2012 Apple Inc. All rights
 * reserved.
 * Copyright (C) 2009, 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#include "core/editing/serializers/MarkupAccumulator.h"

#include "core/XLinkNames.h"
#include "core/XMLNSNames.h"
#include "core/XMLNames.h"
#include "core/dom/Attr.h"
#include "core/dom/CDATASection.h"
#include "core/dom/Comment.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentType.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/editing/Editor.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

MarkupAccumulator::MarkupAccumulator(EAbsoluteURLs resolveUrlsMethod,
                                     SerializationType serializationType)
    : m_formatter(resolveUrlsMethod, serializationType) {}

MarkupAccumulator::~MarkupAccumulator() {}

void MarkupAccumulator::appendString(const String& string) {
  m_markup.append(string);
}

void MarkupAccumulator::appendStartTag(Node& node, Namespaces* namespaces) {
  appendStartMarkup(m_markup, node, namespaces);
}

void MarkupAccumulator::appendEndTag(const Element& element) {
  appendEndMarkup(m_markup, element);
}

void MarkupAccumulator::appendStartMarkup(StringBuilder& result,
                                          Node& node,
                                          Namespaces* namespaces) {
  switch (node.getNodeType()) {
    case Node::kTextNode:
      appendText(result, toText(node));
      break;
    case Node::kElementNode:
      appendElement(result, toElement(node), namespaces);
      break;
    case Node::kAttributeNode:
      // Only XMLSerializer can pass an Attr.  So, |documentIsHTML| flag is
      // false.
      m_formatter.appendAttributeValue(result, toAttr(node).value(), false);
      break;
    default:
      m_formatter.appendStartMarkup(result, node, namespaces);
      break;
  }
}

static bool elementCannotHaveEndTag(const Node& node) {
  if (!node.isHTMLElement())
    return false;

  return !toHTMLElement(node).shouldSerializeEndTag();
}

void MarkupAccumulator::appendEndMarkup(StringBuilder& result,
                                        const Element& element) {
  m_formatter.appendEndMarkup(result, element);
}

void MarkupAccumulator::appendCustomAttributes(StringBuilder&,
                                               const Element&,
                                               Namespaces*) {}

void MarkupAccumulator::appendText(StringBuilder& result, Text& text) {
  m_formatter.appendText(result, text);
}

bool MarkupAccumulator::shouldIgnoreAttribute(
    const Element& element,
    const Attribute& attribute) const {
  return false;
}

bool MarkupAccumulator::shouldIgnoreElement(const Element& element) const {
  return false;
}

void MarkupAccumulator::appendElement(StringBuilder& result,
                                      const Element& element,
                                      Namespaces* namespaces) {
  appendOpenTag(result, element, namespaces);

  AttributeCollection attributes = element.attributes();
  for (const auto& attribute : attributes) {
    if (!shouldIgnoreAttribute(element, attribute))
      appendAttribute(result, element, attribute, namespaces);
  }

  // Give an opportunity to subclasses to add their own attributes.
  appendCustomAttributes(result, element, namespaces);

  appendCloseTag(result, element);
}

void MarkupAccumulator::appendOpenTag(StringBuilder& result,
                                      const Element& element,
                                      Namespaces* namespaces) {
  m_formatter.appendOpenTag(result, element, namespaces);
}

void MarkupAccumulator::appendCloseTag(StringBuilder& result,
                                       const Element& element) {
  m_formatter.appendCloseTag(result, element);
}

void MarkupAccumulator::appendAttribute(StringBuilder& result,
                                        const Element& element,
                                        const Attribute& attribute,
                                        Namespaces* namespaces) {
  m_formatter.appendAttribute(result, element, attribute, namespaces);
}

EntityMask MarkupAccumulator::entityMaskForText(const Text& text) const {
  return m_formatter.entityMaskForText(text);
}

bool MarkupAccumulator::serializeAsHTMLDocument(const Node& node) const {
  return m_formatter.serializeAsHTMLDocument(node);
}

template <typename Strategy>
static void serializeNodesWithNamespaces(MarkupAccumulator& accumulator,
                                         Node& targetNode,
                                         EChildrenOnly childrenOnly,
                                         const Namespaces* namespaces) {
  if (targetNode.isElementNode() &&
      accumulator.shouldIgnoreElement(toElement(targetNode))) {
    return;
  }

  Namespaces namespaceHash;
  if (namespaces)
    namespaceHash = *namespaces;

  if (!childrenOnly)
    accumulator.appendStartTag(targetNode, &namespaceHash);

  if (!(accumulator.serializeAsHTMLDocument(targetNode) &&
        elementCannotHaveEndTag(targetNode))) {
    Node* current =
        isHTMLTemplateElement(targetNode)
            ? Strategy::firstChild(*toHTMLTemplateElement(targetNode).content())
            : Strategy::firstChild(targetNode);
    for (; current; current = Strategy::nextSibling(*current))
      serializeNodesWithNamespaces<Strategy>(accumulator, *current, IncludeNode,
                                             &namespaceHash);
  }

  if ((!childrenOnly && targetNode.isElementNode()) &&
      !(accumulator.serializeAsHTMLDocument(targetNode) &&
        elementCannotHaveEndTag(targetNode)))
    accumulator.appendEndTag(toElement(targetNode));
}

template <typename Strategy>
String serializeNodes(MarkupAccumulator& accumulator,
                      Node& targetNode,
                      EChildrenOnly childrenOnly) {
  Namespaces* namespaces = nullptr;
  Namespaces namespaceHash;
  if (!accumulator.serializeAsHTMLDocument(targetNode)) {
    // Add pre-bound namespaces for XML fragments.
    namespaceHash.set(xmlAtom, XMLNames::xmlNamespaceURI);
    namespaces = &namespaceHash;
  }

  serializeNodesWithNamespaces<Strategy>(accumulator, targetNode, childrenOnly,
                                         namespaces);
  return accumulator.toString();
}

template String serializeNodes<EditingStrategy>(MarkupAccumulator&,
                                                Node&,
                                                EChildrenOnly);

}  // namespace blink
