/**
 * Morpheus_Multiply_Impl.hpp
 *
 * EPCC, The University of Edinburgh
 *
 * (c) 2021 - 2022 The University of Edinburgh
 *
 * Contributing Authors:
 * Christodoulos Stylianou (c.stylianou@ed.ac.uk)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MORPHEUS_CSR_SERIAL_MULTIPLY_IMPL_HPP
#define MORPHEUS_CSR_SERIAL_MULTIPLY_IMPL_HPP

#include <Morpheus_Macros.hpp>
#if defined(MORPHEUS_ENABLE_SERIAL)

#include <Morpheus_SpaceTraits.hpp>
#include <Morpheus_FormatTraits.hpp>
#include <Morpheus_FormatTags.hpp>
#include <Morpheus_Spaces.hpp>

#include <impl/Csr/Serial/Morpheus_Multiply_ARMPL_Impl.hpp>

namespace Morpheus {
namespace Impl {

template <typename ExecSpace, typename Matrix, typename Vector>
inline void multiply(
    const Matrix& A, const Vector& x, Vector& y, const bool init,
    typename std::enable_if_t<
        Morpheus::is_csr_matrix_format_container_v<Matrix> &&
        Morpheus::is_dense_vector_format_container_v<Vector> &&
        Morpheus::has_custom_backend_v<ExecSpace> &&
        Morpheus::has_serial_execution_space_v<ExecSpace> &&
        Morpheus::has_access_v<ExecSpace, Matrix, Vector>>* = nullptr) {
  using index_type = typename Matrix::index_type;
  using value_type = typename Matrix::value_type;

#if defined(MORPHEUS_ENABLE_TPL_ARMPL)
  if constexpr (std::is_floating_point_v<value_type> && std::is_same_v<index_type, int>) {
    multiply_armpl_csr(A.nrows(), A.ncols(), A.crow_offsets().data(), 
                 A.ccolumn_indices().data(), A.cvalues().data(),
                 x.const_view().data(), y.data(), init);
  }else{
    for (index_type i = 0; i < A.nrows(); i++) {
    value_type sum = init ? value_type(0) : y[i];
    for (index_type jj = A.crow_offsets(i); jj < A.crow_offsets(i + 1); jj++) {
      sum += A.cvalues(jj) * x[A.ccolumn_indices(jj)];
    }
    y[i] = sum;
  }
  }
  
#else
  for (index_type i = 0; i < A.nrows(); i++) {
    value_type sum = init ? value_type(0) : y[i];
    for (index_type jj = A.crow_offsets(i); jj < A.crow_offsets(i + 1); jj++) {
      sum += A.cvalues(jj) * x[A.ccolumn_indices(jj)];
    }
    y[i] = sum;
  }
#endif // MORPHEUS_ENABLE_TPL_ARMPL
}

}  // namespace Impl
}  // namespace Morpheus

#endif  // MORPHEUS_ENABLE_SERIAL
#endif  // MORPHEUS_CSR_SERIAL_MULTIPLY_IMPL_HPP