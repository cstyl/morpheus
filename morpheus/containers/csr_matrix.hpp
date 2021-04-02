/**
 * csr_matrix.hpp
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

#ifndef MORPHEUS_CONTAINERS_CSR_MATRIX_HPP
#define MORPHEUS_CONTAINERS_CSR_MATRIX_HPP

#include <string>

#include <morpheus/core/exceptions.hpp>
#include <morpheus/core/matrix_traits.hpp>
#include <morpheus/core/matrix_tags.hpp>
#include <morpheus/containers/vector.hpp>

namespace Morpheus {

struct CsrTag : public Impl::SparseMatTag {};

template <class... Properties>
class CsrMatrix : public Impl::MatrixTraits<Properties...> {
 public:
  using type   = CsrMatrix<Properties...>;
  using traits = Impl::MatrixTraits<Properties...>;

  using index_type = typename traits::index_type;
  using value_type = typename traits::value_type;

  using memory_space    = typename traits::memory_space;
  using execution_space = typename traits::execution_space;
  using device_type     = typename traits::device_type;
  using tag             = typename MatrixFormatTag<CsrTag>::tag;

  using index_array_type = Morpheus::vector<index_type>;
  using value_array_type = Morpheus::vector<value_type>;

  index_array_type row_offsets, column_indices;
  value_array_type values;

  ~CsrMatrix()                 = default;
  CsrMatrix(const CsrMatrix &) = default;
  CsrMatrix(CsrMatrix &&)      = default;
  CsrMatrix &operator=(const CsrMatrix &) = default;
  CsrMatrix &operator=(CsrMatrix &&) = default;

  // Construct an empty CsrMatrix
  inline CsrMatrix()
      : row_offsets(1),
        column_indices(0),
        values(0),
        _name("CsrMatrix"),
        _m(0),
        _n(0),
        _nnz(0) {}

  // Construct a CsrMatrix with a specific shape and number of non-zero entries
  inline CsrMatrix(const index_type num_rows, const index_type num_cols,
                   const index_type num_entries)
      : row_offsets(num_rows + 1),
        column_indices(num_entries),
        values(num_entries),
        _name("CsrMatrix"),
        _m(num_rows),
        _n(num_cols),
        _nnz(num_entries) {}

  inline CsrMatrix(const std::string name, const index_type num_rows,
                   const index_type num_cols, const index_type num_entries)
      : row_offsets(num_rows + 1),
        column_indices(num_entries),
        values(num_entries),
        _name(name),
        _m(num_rows),
        _n(num_cols),
        _nnz(num_entries) {}

  // Construct from another matrix type
  template <typename MatrixType>
  CsrMatrix(const MatrixType &matrix) {
    // TODO: CsrMatrix(const MatrixType& matrix)
    throw Morpheus::NotImplementedException(
        "CsrMatrix(const MatrixType& matrix)");
  }

  // Resize matrix dimensions and underlying storage
  inline void resize(const index_type num_rows, const index_type num_cols,
                     const index_type num_entries) {
    _m   = num_rows;
    _n   = num_cols;
    _nnz = num_entries;
    row_offsets.resize(_m + 1);
    column_indices.resize(_nnz);
    values.resize(_nnz);
  }

  // Assignment from another matrix type
  template <typename MatrixType>
  CsrMatrix &operator=(const MatrixType &matrix) {
    // TODO: operator=(const MatrixType& matrix)
    throw Morpheus::NotImplementedException(
        "CsrMatrix.operator=(const MatrixType& matrix)");
  }

  // Unified routines across all formats
  inline std::string name() const { return _name; }
  inline index_type nrows() const { return _m; }
  inline index_type ncols() const { return _n; }
  inline index_type nnnz() const { return _nnz; }

 private:
  std::string _name;
  index_type _m, _n, _nnz;
};
}  // namespace Morpheus

#endif  // MORPHEUS_CONTAINERS_CSR_MATRIX_HPP