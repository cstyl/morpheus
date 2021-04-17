/**
 * convert_impl.hpp
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

#ifndef MORPHEUS_ALGORITHMS_IMPL_CONVERT_IMPL_HPP
#define MORPHEUS_ALGORITHMS_IMPL_CONVERT_IMPL_HPP

#include <morpheus/algorithms/impl/csr_matrix/convert_impl.hpp>
#include <morpheus/algorithms/impl/dia_matrix/convert_impl.hpp>
#include <morpheus/algorithms/impl/dynamic_matrix/convert_impl.hpp>

#include <morpheus/containers/fwd/coo_matrix.hpp>

namespace Morpheus {
namespace Impl {
template <typename SourceType, typename DestinationType, typename Format>
void convert(const SourceType& src, DestinationType& dst, Format, Format) {
  // should it be shallow copy? dst = src
  Morpheus::copy(src, dst);
}

template <typename SourceType, typename DestinationType, typename Format1,
          typename Format2>
void convert(const SourceType& src, DestinationType& dst, Format1, Format2,
             typename std::enable_if_t<!std::is_same_v<Format1, DynamicTag> &&
                                       !std::is_same_v<Format2, DynamicTag>>* =
                 nullptr) {
  using value_type   = typename SourceType::value_type;
  using index_type   = typename SourceType::index_type;
  using array_layout = typename SourceType::array_layout;
  using memory_space = typename SourceType::memory_space;
  // convert src -> coo_matrix -> dst
  using Coo =
      Morpheus::CooMatrix<value_type, index_type, array_layout, memory_space>;
  Coo tmp;

  Morpheus::Impl::convert(src, tmp, Format1(), typename Coo::tag());
  Morpheus::Impl::convert(tmp, dst, typename Coo::tag(), Format2());
}
}  // namespace Impl
}  // namespace Morpheus

#endif  // MORPHEUS_ALGORITHMS_IMPL_CONVERT_IMPL_HPP