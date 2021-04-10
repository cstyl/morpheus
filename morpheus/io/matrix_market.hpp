/**
 * matrix_market.hpp
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

#ifndef MORPHEUS_IO_MATRIX_MARKET_HPP
#define MORPHEUS_IO_MATRIX_MARKET_HPP

#include <string>

#include <morpheus/io/impl/matrix_market_impl.hpp>

namespace Morpheus {
namespace Io {
template <typename Matrix>
void read_matrix_market(Matrix& mtx, const std::string& filename);

template <typename Matrix, typename Stream>
void read_matrix_market(Matrix& mtx, Stream& input);

template <typename Matrix>
void write_matrix_market(const Matrix& mtx, const std::string& filename);

template <typename Matrix, typename Stream>
void write_matrix_market(const Matrix& mtx, Stream& output);
}  // namespace Io
}  // namespace Morpheus

#endif  // MORPHEUS_IO_MATRIX_MARKET_HPP