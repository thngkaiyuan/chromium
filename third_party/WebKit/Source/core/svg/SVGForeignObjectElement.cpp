/*
 * Copyright (C) 2006 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "core/svg/SVGForeignObjectElement.h"

#include "core/dom/StyleChangeReason.h"
#include "core/frame/UseCounter.h"
#include "core/layout/svg/LayoutSVGForeignObject.h"
#include "core/svg/SVGLength.h"
#include "wtf/Assertions.h"

namespace blink {

inline SVGForeignObjectElement::SVGForeignObjectElement(Document& document)
    : SVGGraphicsElement(SVGNames::foreignObjectTag, document),
      m_x(SVGAnimatedLength::create(this,
                                    SVGNames::xAttr,
                                    SVGLength::create(SVGLengthMode::Width),
                                    CSSPropertyX)),
      m_y(SVGAnimatedLength::create(this,
                                    SVGNames::yAttr,
                                    SVGLength::create(SVGLengthMode::Height),
                                    CSSPropertyY)),
      m_width(SVGAnimatedLength::create(this,
                                        SVGNames::widthAttr,
                                        SVGLength::create(SVGLengthMode::Width),
                                        CSSPropertyWidth)),
      m_height(
          SVGAnimatedLength::create(this,
                                    SVGNames::heightAttr,
                                    SVGLength::create(SVGLengthMode::Height),
                                    CSSPropertyHeight)) {
  addToPropertyMap(m_x);
  addToPropertyMap(m_y);
  addToPropertyMap(m_width);
  addToPropertyMap(m_height);

  UseCounter::count(document, UseCounter::SVGForeignObjectElement);
}

DEFINE_TRACE(SVGForeignObjectElement) {
  visitor->trace(m_x);
  visitor->trace(m_y);
  visitor->trace(m_width);
  visitor->trace(m_height);
  SVGGraphicsElement::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGForeignObjectElement)

void SVGForeignObjectElement::collectStyleForPresentationAttribute(
    const QualifiedName& name,
    const AtomicString& value,
    MutableStylePropertySet* style) {
  SVGAnimatedPropertyBase* property = propertyFromAttribute(name);
  if (property == m_width) {
    addPropertyToPresentationAttributeStyle(style, property->cssPropertyId(),
                                            m_width->cssValue());
  } else if (property == m_height) {
    addPropertyToPresentationAttributeStyle(style, property->cssPropertyId(),
                                            m_height->cssValue());
  } else if (property == m_x) {
    addPropertyToPresentationAttributeStyle(style, property->cssPropertyId(),
                                            m_x->cssValue());
  } else if (property == m_y) {
    addPropertyToPresentationAttributeStyle(style, property->cssPropertyId(),
                                            m_y->cssValue());
  } else {
    SVGGraphicsElement::collectStyleForPresentationAttribute(name, value,
                                                             style);
  }
}

void SVGForeignObjectElement::svgAttributeChanged(
    const QualifiedName& attrName) {
  bool isWidthHeightAttribute =
      attrName == SVGNames::widthAttr || attrName == SVGNames::heightAttr;
  bool isXYAttribute =
      attrName == SVGNames::xAttr || attrName == SVGNames::yAttr;

  if (isXYAttribute || isWidthHeightAttribute) {
    SVGElement::InvalidationGuard invalidationGuard(this);

    invalidateSVGPresentationAttributeStyle();
    setNeedsStyleRecalc(
        LocalStyleChange,
        isWidthHeightAttribute
            ? StyleChangeReasonForTracing::create(
                  StyleChangeReason::SVGContainerSizeChange)
            : StyleChangeReasonForTracing::fromAttribute(attrName));

    updateRelativeLengthsInformation();
    if (LayoutObject* layoutObject = this->layoutObject())
      markForLayoutAndParentResourceInvalidation(layoutObject);

    return;
  }

  SVGGraphicsElement::svgAttributeChanged(attrName);
}

LayoutObject* SVGForeignObjectElement::createLayoutObject(
    const ComputedStyle&) {
  return new LayoutSVGForeignObject(this);
}

bool SVGForeignObjectElement::layoutObjectIsNeeded(const ComputedStyle& style) {
  // Suppress foreignObject layoutObjects in SVG hidden containers.
  // (https://bugs.webkit.org/show_bug.cgi?id=87297)
  // Note that we currently do not support foreignObject instantiation via
  // <use>, hence it is safe to use parentElement() here. If that changes, this
  // method should be updated to use parentOrShadowHostElement() instead.
  Element* ancestor = parentElement();
  while (ancestor && ancestor->isSVGElement()) {
    if (ancestor->layoutObject() &&
        ancestor->layoutObject()->isSVGHiddenContainer())
      return false;

    ancestor = ancestor->parentElement();
  }

  return SVGGraphicsElement::layoutObjectIsNeeded(style);
}

bool SVGForeignObjectElement::selfHasRelativeLengths() const {
  return m_x->currentValue()->isRelative() ||
         m_y->currentValue()->isRelative() ||
         m_width->currentValue()->isRelative() ||
         m_height->currentValue()->isRelative();
}

}  // namespace blink
