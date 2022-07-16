/**
 * Morpheus_Print_Impl.hpp
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

#ifndef MORPHEUS_DYNAMIC_PRINT_IMPL_HPP
#define MORPHEUS_DYNAMIC_PRINT_IMPL_HPP

#include <Morpheus_FormatTags.hpp>

#include <impl/Morpheus_Print_Impl.hpp>
#include <impl/Morpheus_Variant.hpp>

namespace Morpheus {
namespace Impl {

struct print_fn {
  using result_type = void;
  // TODO: Specify the stream to print to
  // template <typename T, typename S>
  // result_type operator()(const T& mat, S& s) const {
  //   Morpheus::print(mat, s);
  // }
  template <typename Printable>
  result_type operator()(const Printable& p) const {
    Impl::print(p);
  }
};

template <typename Printable, typename Stream>
void print(const Printable& p, Stream& s,
           typename std::enable_if<Morpheus::is_dynamic_matrix_format_container<
               Printable>::value>::type* = nullptr) {
  print_matrix_header(p, s);
  // TODO: Using a stream in this way doesn't seem to work
  Morpheus::Impl::Variant::visit([&](auto&& arg) { Impl::print(arg); },
                                 p.const_formats());
}

}  // namespace Impl
}  // namespace Morpheus

#endif  // MORPHEUS_DYNAMIC_PRINT_IMPL_HPP