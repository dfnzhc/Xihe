/**
 * @File Eigen.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Defines.hpp"

// Eigen 主头

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wunsafe-buffer-usage")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-deprecated-sync")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation")
XIHE_CLANG_DISABLE_WARNING("-Wreserved-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wunused-template")
XIHE_CLANG_DISABLE_WARNING("-Wzero-as-null-pointer-constant")
XIHE_CLANG_DISABLE_WARNING("-Wsign-conversion")
XIHE_CLANG_DISABLE_WARNING("-Wshadow")
XIHE_CLANG_DISABLE_WARNING("-Wextra-semi-stmt")
XIHE_CLANG_DISABLE_WARNING("-Wextra-semi")
XIHE_CLANG_DISABLE_WARNING("-Wimplicit-int-conversion")
XIHE_CLANG_DISABLE_WARNING("-Wmissing-noreturn")
XIHE_CLANG_DISABLE_WARNING("-Wcast-align")
XIHE_CLANG_DISABLE_WARNING("-Wdeprecated-declarations")
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
XIHE_POP_WARNING

namespace xihe {
// =============================
// 基础模板别名
// =============================

template<typename T, int N>
using Vec = Eigen::Matrix<T, N, 1>;

template<typename T, int R, int C>
using Mat = Eigen::Matrix<T, R, C>;

template<typename T, int N>
using Arr = Eigen::Array<T, N, 1>;

// 动态尺寸
template<typename T>
using VecX = Eigen::Matrix<T, Eigen::Dynamic, 1>;

template<typename T>
using MatX = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

template<typename T>
using ArrX = Eigen::Array<T, Eigen::Dynamic, 1>;

// =============================
// 常用定长向量
// =============================

using Vec2f = Vec<f32, 2>;
using Vec3f = Vec<f32, 3>;
using Vec4f = Vec<f32, 4>;

using Vec2d = Vec<f64, 2>;
using Vec3d = Vec<f64, 3>;
using Vec4d = Vec<f64, 4>;

using Vec2i = Vec<i32, 2>;
using Vec3i = Vec<i32, 3>;
using Vec4i = Vec<i32, 4>;

// 动态向量
using VecXf = VecX<f32>;
using VecXd = VecX<f64>;
using VecXi = VecX<i32>;

// =============================
// 常用低阶方阵
// =============================

using Mat2f = Mat<f32, 2, 2>;
using Mat3f = Mat<f32, 3, 3>;
using Mat4f = Mat<f32, 4, 4>;

using Mat2d = Mat<f64, 2, 2>;
using Mat3d = Mat<f64, 3, 3>;
using Mat4d = Mat<f64, 4, 4>;

using MatXf = MatX<f32>;
using MatXd = MatX<f64>;

// =============================
// Array 工具类型（逐元素运算）
// =============================

using Arr2f = Arr<f32, 2>;
using Arr3f = Arr<f32, 3>;
using Arr4f = Arr<f32, 4>;

using Arr2d = Arr<f64, 2>;
using Arr3d = Arr<f64, 3>;
using Arr4d = Arr<f64, 4>;

using ArrXf = ArrX<f32>;
using ArrXd = ArrX<f64>;

// =============================
// 变换与四元数（常见工具）
// =============================

using Quatf = Eigen::Quaternion<f32>;
using Quatd = Eigen::Quaternion<f64>;

using Affine3f = Eigen::Transform<f32, 3, Eigen::Affine>;
using Affine3d = Eigen::Transform<f64, 3, Eigen::Affine>;
} // namespace xihe