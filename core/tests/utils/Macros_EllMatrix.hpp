/**
 * Macros_EllMatrix.hpp
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

#ifndef TEST_CORE_UTILS_MACROS_ELLMATRIX_HPP
#define TEST_CORE_UTILS_MACROS_ELLMATRIX_HPP

#include <Morpheus_Core.hpp>

/**
 * @brief Checks the sizes of a EllMatrix container against a number of rows,
 * columns and non-zeros
 *
 */
#define CHECK_ELL_SIZES(A, num_rows, num_cols, num_nnz, num_entries_per_row, \
                        align)                                               \
  {                                                                          \
    using container_size_type = decltype(A.nrows());                         \
    EXPECT_EQ(A.nrows(), num_rows);                                          \
    EXPECT_EQ(A.ncols(), num_cols);                                          \
    EXPECT_EQ(A.nnnz(), num_nnz);                                            \
    EXPECT_EQ(A.entries_per_row(), num_entries_per_row);                     \
    EXPECT_EQ(A.alignment(), align);                                         \
    EXPECT_EQ(A.column_indices().nrows(), num_rows);                         \
    EXPECT_EQ(A.column_indices().view().extent(0), num_rows);                \
    EXPECT_EQ(A.column_indices().ncols(),                                    \
              Morpheus::Impl::get_pad_size<container_size_type>(             \
                  num_entries_per_row, align));                              \
    EXPECT_EQ(A.column_indices().view().extent(1),                           \
              Morpheus::Impl::get_pad_size<container_size_type>(             \
                  num_entries_per_row, align));                              \
    EXPECT_EQ(A.values().nrows(), num_rows);                                 \
    EXPECT_EQ(A.values().view().extent(0), num_rows);                        \
    EXPECT_EQ(A.values().ncols(),                                            \
              Morpheus::Impl::get_pad_size<container_size_type>(             \
                  num_entries_per_row, align));                              \
    EXPECT_EQ(A.values().view().extent(1),                                   \
              Morpheus::Impl::get_pad_size<container_size_type>(             \
                  num_entries_per_row, align));                              \
  }

/**
 * @brief Checks the sizes of an empty EllMatrix container
 *
 */
#define CHECK_ELL_EMPTY(A)                          \
  {                                                 \
    EXPECT_EQ(A.nrows(), 0);                        \
    EXPECT_EQ(A.ncols(), 0);                        \
    EXPECT_EQ(A.nnnz(), 0);                         \
    EXPECT_EQ(A.entries_per_row(), 0);              \
    EXPECT_EQ(A.alignment(), 0);                    \
    EXPECT_EQ(A.column_indices().nrows(), 0);       \
    EXPECT_EQ(A.column_indices().ncols(), 0);       \
    EXPECT_EQ(A.column_indices().nnnz(), 0);        \
    EXPECT_EQ(A.column_indices().view().size(), 0); \
    EXPECT_EQ(A.values().nrows(), 0);               \
    EXPECT_EQ(A.values().ncols(), 0);               \
    EXPECT_EQ(A.values().nnnz(), 0);                \
    EXPECT_EQ(A.values().view().size(), 0);         \
  }

/**
 * @brief Checks the sizes of two EllMatrix containers if they match
 *
 */
#define CHECK_ELL_CONTAINERS(A, B)                                     \
  {                                                                    \
    EXPECT_EQ(A.nrows(), B.nrows());                                   \
    EXPECT_EQ(A.ncols(), B.ncols());                                   \
    EXPECT_EQ(A.nnnz(), B.nnnz());                                     \
    EXPECT_EQ(A.entries_per_row(), B.entries_per_row());               \
    EXPECT_EQ(A.alignment(), B.alignment());                           \
    EXPECT_EQ(A.column_indices().nrows(), B.column_indices().nrows()); \
    EXPECT_EQ(A.column_indices().ncols(), B.column_indices().ncols()); \
    EXPECT_EQ(A.column_indices().nnnz(), B.column_indices().nnnz());   \
    EXPECT_EQ(A.column_indices().view().size(),                        \
              B.column_indices().view().size());                       \
    EXPECT_EQ(A.values().nrows(), B.values().nrows());                 \
    EXPECT_EQ(A.values().ncols(), B.values().ncols());                 \
    EXPECT_EQ(A.values().nnnz(), B.values().nnnz());                   \
    EXPECT_EQ(A.values().view().size(), B.values().view().size());     \
  }

/**
 * @brief Checks if the data arrays of two EllMatrix containers contain the same
 * data.
 *
 */
#define VALIDATE_ELL_CONTAINER(A, Aref)                               \
  {                                                                   \
    using container_type      = decltype(A);                          \
    using container_size_type = typename container_type::size_type;   \
    for (container_size_type i = 0; i < A.values().nrows(); i++) {    \
      for (container_size_type j = 0; j < A.values().ncols(); j++) {  \
        EXPECT_EQ(A.column_indices(i, j), Aref.column_indices(i, j)); \
        EXPECT_EQ(A.values(i, j), Aref.values(i, j));                 \
      }                                                               \
    }                                                                 \
  }

namespace Morpheus {
namespace Test {
template <typename Container>
void reset_small_container(
    Container& c,
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container>>* = nullptr) {
  using size_type  = typename Container::size_type;
  using value_type = typename Container::value_type;
  // Matrix
  // [1.11 *    2.22]
  // [*    *    3.33]
  // [*    4.44 *   ]

  for (size_type i = 0; i < c.values().nrows(); i++) {
    for (size_type j = 0; j < c.values().ncols(); j++) {
      c.column_indices(i, j) = c.invalid_index();
      c.values(i, j)         = value_type(0);
    }
  }

  // clang-format off
  c.column_indices(0,0) = 0; 
  c.column_indices(0,1) = 2; 
  c.column_indices(1,0) = 2; 
  c.column_indices(2,0) = 1; 

  c.values(0,0) = (value_type)1.11; 
  c.values(0,1) = (value_type)2.22; 
  c.values(1,0) = (value_type)3.33; 
  c.values(2,0) = (value_type)4.44;
  // clang-format on
}

/**
 * @brief Builds a sample EllMatrix container. Assumes we have already
 * constructed the matrix and we are only adding data.
 *
 * @tparam Matrix A EllMatrix type
 * @param A The EllMatrix we will be initializing.
 */
template <typename Container>
void build_small_container(
    Container& c,
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container>>* = nullptr) {
  // Matrix to Build
  // [1.11 *    2.22]
  // [*    *    3.33]
  // [*    4.44 *   ]
  CHECK_ELL_SIZES(c, 3, 3, 4, 2, 32);

  reset_small_container(c);
}

template <typename Container>
void update_small_container(
    Container& c,
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container>>* = nullptr) {
  using size_type  = typename Container::size_type;
  using value_type = typename Container::value_type;
  // New Matrix
  // [1.11 *    *    ]
  // [*    *    -3.33]
  // [2.22 4.44 *    ]

  for (size_type i = 0; i < c.values().nrows(); i++) {
    for (size_type j = 0; j < c.values().ncols(); j++) {
      c.column_indices(i, j) = c.invalid_index();
      c.values(i, j)         = value_type(0);
    }
  }

  // clang-format off
  c.column_indices(0,0) = 0; 
  c.column_indices(1,0) = 2; 
  c.column_indices(2,0) = 0; 
  c.column_indices(2,1) = 1; 

  c.values(0,0) = (value_type)1.11; 
  c.values(1,0) = (value_type)-3.33; 
  c.values(2,0) = (value_type)2.22; 
  c.values(2,1) = (value_type)4.44;
  // clang-format on
}

template <class Container>
void setup_small_container(
    Container& c,
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container>>* = nullptr) {
  c.resize(3, 3, 4, 2, 32);
  build_small_container(c);
}

template <class Container1, class Container2>
bool is_same_size(
    Container1& c1, Container2& c2,
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container1> &&
        Morpheus::is_ell_matrix_format_container_v<Container2>>* = nullptr) {
  bool same_nrows = c1.nrows() == c2.nrows() ? true : false;
  bool same_ncols = c1.ncols() == c2.ncols() ? true : false;
  bool same_nnnz  = c1.nnnz() == c2.nnnz() ? true : false;
  bool same_entries_per_row =
      c1.entries_per_row() == c2.entries_per_row() ? true : false;
  bool same_nalignment = c1.alignment() == c2.alignment() ? true : false;
  bool same_cind_rows =
      c1.column_indices().nrows() == c2.column_indices().nrows() ? true : false;
  bool same_cind_cols =
      c1.column_indices().ncols() == c2.column_indices().ncols() ? true : false;
  bool same_vals_rows =
      c1.values().nrows() == c2.values().nrows() ? true : false;
  bool same_vals_cols =
      c1.values().ncols() == c2.values().ncols() ? true : false;

  return same_nrows && same_ncols && same_nnnz && same_entries_per_row &&
         same_nalignment && same_cind_rows && same_cind_cols &&
         same_vals_rows && same_vals_cols;
}

template <class Container>
bool is_empty_container(
    Container& c,
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container>>* = nullptr) {
  using value_type = typename Container::value_type;
  using size_type  = typename Container::size_type;

  typename Container::HostMirror ch;
  ch.resize(c);
  Morpheus::copy(c, ch);

  for (size_type i = 0; i < c.values().nrows(); i++) {
    for (size_type j = 0; j < c.values().ncols(); j++) {
      if ((ch.values(i, j) != (value_type)0.0)) {
        return false;
      }
    }
  }

  return true;
}

template <typename Container, typename ValueType, typename IndexType,
          typename ArrayLayout, typename Space>
Morpheus::EllMatrix<ValueType, IndexType, ArrayLayout, Space> create_container(
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container>>* = nullptr) {
  return Morpheus::EllMatrix<ValueType, IndexType, ArrayLayout, Space>();
}

template <class Container1, class Container2>
bool have_same_data(
    Container1& c1, Container2& c2,
    typename std::enable_if_t<
        Morpheus::is_ell_matrix_format_container_v<Container1> &&
        Morpheus::is_ell_matrix_format_container_v<Container2>>* = nullptr) {
  using size_type = typename Container1::size_type;

  if (!is_same_size(c1, c2)) return false;

  typename Container1::HostMirror c1_h;
  c1_h.resize(c1);
  Morpheus::copy(c1, c1_h);

  typename Container1::HostMirror c2_h;
  c2_h.resize(c2);
  Morpheus::copy(c2, c2_h);

  for (size_type i = 0; i < c1_h.values().nrows(); i++) {
    for (size_type j = 0; j < c1_h.values().ncols(); j++) {
      if ((c1_h.column_indices(i, j) != c2_h.column_indices(i, j)) ||
          (c1_h.values(i, j) != c2_h.values(i, j))) {
        return false;
      }
    }
  }

  return true;
}

}  // namespace Test
}  // namespace Morpheus

#endif  // TEST_CORE_UTILS_MACROS_ELLMATRIX_HPP