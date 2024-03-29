// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSMatrixComponent_h
#define CSSMatrixComponent_h

#include <memory>
#include "core/css/cssom/CSSTransformComponent.h"
#include "platform/transforms/TransformationMatrix.h"

namespace blink {

class CORE_EXPORT CSSMatrixComponent final : public CSSTransformComponent {
  WTF_MAKE_NONCOPYABLE(CSSMatrixComponent);
  DEFINE_WRAPPERTYPEINFO();

 public:
  static CSSMatrixComponent* create(double a,
                                    double b,
                                    double c,
                                    double d,
                                    double e,
                                    double f) {
    return new CSSMatrixComponent(a, b, c, d, e, f);
  }

  static CSSMatrixComponent* create(double m11,
                                    double m12,
                                    double m13,
                                    double m14,
                                    double m21,
                                    double m22,
                                    double m23,
                                    double m24,
                                    double m31,
                                    double m32,
                                    double m33,
                                    double m34,
                                    double m41,
                                    double m42,
                                    double m43,
                                    double m44) {
    return new CSSMatrixComponent(m11, m12, m13, m14, m21, m22, m23, m24, m31,
                                  m32, m33, m34, m41, m42, m43, m44);
  }

  static CSSMatrixComponent* fromCSSValue(const CSSFunctionValue& value) {
    return nullptr;
  }

  // 2D matrix attributes
  double a() const { return m_matrix->a(); }
  double b() const { return m_matrix->b(); }
  double c() const { return m_matrix->c(); }
  double d() const { return m_matrix->d(); }
  double e() const { return m_matrix->e(); }
  double f() const { return m_matrix->f(); }

  // 3D matrix attributes
  double m11() const { return m_matrix->m11(); }
  double m12() const { return m_matrix->m12(); }
  double m13() const { return m_matrix->m13(); }
  double m14() const { return m_matrix->m14(); }
  double m21() const { return m_matrix->m21(); }
  double m22() const { return m_matrix->m22(); }
  double m23() const { return m_matrix->m23(); }
  double m24() const { return m_matrix->m24(); }
  double m31() const { return m_matrix->m31(); }
  double m32() const { return m_matrix->m32(); }
  double m33() const { return m_matrix->m33(); }
  double m34() const { return m_matrix->m34(); }
  double m41() const { return m_matrix->m41(); }
  double m42() const { return m_matrix->m42(); }
  double m43() const { return m_matrix->m43(); }
  double m44() const { return m_matrix->m44(); }

  TransformComponentType type() const override {
    return m_is2D ? MatrixType : Matrix3DType;
  }

  // Bindings require a non const return value.
  CSSMatrixComponent* asMatrix() const override {
    return const_cast<CSSMatrixComponent*>(this);
  }

  CSSFunctionValue* toCSSValue() const override;

  static CSSMatrixComponent* perspective(double length);

  static CSSMatrixComponent* rotate(double angle);
  static CSSMatrixComponent* rotate3d(double angle,
                                      double x,
                                      double y,
                                      double z);

  static CSSMatrixComponent* scale(double x, double y);
  static CSSMatrixComponent* scale3d(double x, double y, double z);

  static CSSMatrixComponent* skew(double x, double y);

  static CSSMatrixComponent* translate(double x, double y);
  static CSSMatrixComponent* translate3d(double x, double y, double z);

 private:
  CSSMatrixComponent(double a, double b, double c, double d, double e, double f)
      : CSSTransformComponent(),
        m_matrix(TransformationMatrix::create(a, b, c, d, e, f)),
        m_is2D(true) {}

  CSSMatrixComponent(double m11,
                     double m12,
                     double m13,
                     double m14,
                     double m21,
                     double m22,
                     double m23,
                     double m24,
                     double m31,
                     double m32,
                     double m33,
                     double m34,
                     double m41,
                     double m42,
                     double m43,
                     double m44)
      : CSSTransformComponent(),
        m_matrix(TransformationMatrix::create(m11,
                                              m12,
                                              m13,
                                              m14,
                                              m21,
                                              m22,
                                              m23,
                                              m24,
                                              m31,
                                              m32,
                                              m33,
                                              m34,
                                              m41,
                                              m42,
                                              m43,
                                              m44)),
        m_is2D(false) {}

  CSSMatrixComponent(std::unique_ptr<const TransformationMatrix> matrix,
                     TransformComponentType fromType)
      : CSSTransformComponent(),
        m_matrix(std::move(matrix)),
        m_is2D(is2DComponentType(fromType)) {}

  // TransformationMatrix needs to be 16-byte aligned. PartitionAlloc
  // supports 16-byte alignment but Oilpan doesn't. So we use an std::unique_ptr
  // to allocate TransformationMatrix on PartitionAlloc.
  // TODO(oilpan): Oilpan should support 16-byte aligned allocations.
  std::unique_ptr<const TransformationMatrix> m_matrix;
  bool m_is2D;
};

}  // namespace blink

#endif
