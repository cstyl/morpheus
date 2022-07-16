/**
 * Morpheus_Multiply_Impl.hpp
 *
 * EPCC, The University of Edinburgh
 *
 * (c) 2021 The University of Edinburgh
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

#ifndef MORPHEUS_COO_SERIAL_MULTIPLY_IMPL_HPP
#define MORPHEUS_COO_SERIAL_MULTIPLY_IMPL_HPP

#include <Morpheus_TypeTraits.hpp>
#include <Morpheus_FormatTags.hpp>

namespace Morpheus {
namespace Impl {

template <typename ExecSpace, typename Matrix, typename Vector1,
          typename Vector2>
inline void multiply(
    const Matrix& A, const Vector1& x, Vector2& y, const bool init, CooTag,
    DenseVectorTag, DenseVectorTag,
    typename std::enable_if_t<
        !Morpheus::is_kokkos_space_v<ExecSpace> &&
        Morpheus::is_serial_execution_space_v<ExecSpace> &&
        Morpheus::has_access_v<typename ExecSpace::execution_space, Matrix,
                               Vector1, Vector2>>* = nullptr) {
  using index_type = typename Matrix::index_type;
  using value_type = typename Matrix::value_type;

  if (init) {
    for (index_type n = 0; n < A.nrows(); n++) {
      y[n] = value_type(0);
    }
  }

  for (index_type n = 0; n < A.nnnz(); n++) {
    y[A.crow_indices(n)] += A.cvalues(n) * x[A.ccolumn_indices(n)];
  }
}

}  // namespace Impl
}  // namespace Morpheus

#endif  // MORPHEUS_COO_SERIAL_MULTIPLY_IMPL_HPP