/**
 * Test_CsrMatrix.hpp
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

#ifndef TEST_CORE_TEST_CSRMATRIX_HPP
#define TEST_CORE_TEST_CSRMATRIX_HPP

#include <Morpheus_Core.hpp>
#include <Utils.hpp>

using CsrMatrixTypes =
    typename Morpheus::generate_unary_typelist<Morpheus::CsrMatrix<double>,
                                               types::types_set>::type;
using CsrMatrixUnary = to_gtest_types<CsrMatrixTypes>::type;

/**
 * @brief Checks the sizes of a CsrMatrix container against a number of rows,
 * columns and non-zeros
 *
 */
#define CHECK_CSR_SIZES(A, num_rows, num_cols, num_nnz) \
  {                                                     \
    EXPECT_EQ(A.nrows(), num_rows);                     \
    EXPECT_EQ(A.ncols(), num_cols);                     \
    EXPECT_EQ(A.nnnz(), num_nnz);                       \
    EXPECT_EQ(A.row_offsets().size(), num_rows + 1);    \
    EXPECT_EQ(A.column_indices().size(), num_nnz);      \
    EXPECT_EQ(A.values().size(), num_nnz);              \
  }

/**
 * @brief Checks the sizes of two CsrMatrix containers if they match
 *
 */
#define CHECK_CSR_CONTAINERS(A, B)                                   \
  {                                                                  \
    EXPECT_EQ(A.nrows(), B.nrows());                                 \
    EXPECT_EQ(A.ncols(), B.ncols());                                 \
    EXPECT_EQ(A.nnnz(), B.nnnz());                                   \
    EXPECT_EQ(A.row_offsets().size(), B.row_offsets().size());       \
    EXPECT_EQ(A.column_indices().size(), B.column_indices().size()); \
    EXPECT_EQ(A.values().size(), B.values().size());                 \
  }

/**
 * @brief Checks if the data arrays of two CsrMatrix containers contain the same
 * data.
 *
 */
#define VALIDATE_CSR_CONTAINER(A, Aref, nrows, nnnz, type)    \
  {                                                           \
    for (type n = 0; n < nrows + 1; n++) {                    \
      EXPECT_EQ(A.row_offsets(n), Aref.row_offsets(n));       \
    }                                                         \
    for (type n = 0; n < nnnz; n++) {                         \
      EXPECT_EQ(A.column_indices(n), Aref.column_indices(n)); \
      EXPECT_EQ(A.values(n), Aref.values(n));                 \
    }                                                         \
  }

/**
 * @brief Builds a sample CooMatrix container. Assumes we have already
 * constructed the matrix and we are only adding data.
 *
 * @tparam Matrix A CooMatrix type
 * @param A The CooMatrix we will be initializing.
 */
template <typename Matrix>
void build_csrmatrix(Matrix& A) {
  // Matrix to Build
  // [1.11 *    2.22]
  // [*    *    3.33]
  // [*    4.44 *   ]
  CHECK_CSR_SIZES(A, 3, 3, 4);

  // clang-format off
  A.row_offsets(0) = 0; 
  A.row_offsets(1) = 2; 
  A.row_offsets(2) = 3; 
  A.row_offsets(3) = 4; 

  A.column_indices(0) = 0; A.values(0) = 1.11;
  A.column_indices(1) = 2; A.values(1) = 2.22;
  A.column_indices(2) = 2; A.values(2) = 3.33;
  A.column_indices(3) = 1; A.values(3) = 4.44;
  // clang-format on
}

// Used for testing unary operations for same type container
template <typename UnaryContainer>
class CsrMatrixUnaryTest : public ::testing::Test {
 public:
  using type   = UnaryContainer;
  using device = typename UnaryContainer::type;
  using host   = typename UnaryContainer::type::HostMirror;

  CsrMatrixUnaryTest() : Aref(3, 3, 4), Ahref(3, 3, 4) {}

  void SetUp() override {
    build_csrmatrix(Ahref);

    // Send Matrix to device
    Morpheus::copy(Ahref, Aref);
  }

  device Aref;
  host Ahref;
};

namespace Test {

/**
 * @brief Test Suite using the Unary CsrMatrix
 *
 */
TYPED_TEST_CASE(CsrMatrixUnaryTest, CsrMatrixUnary);

/**
 * @brief Testing default construction of CsrMatrix container
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, DefaultConstruction) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;

  Matrix A;
  EXPECT_EQ(A.nrows(), 0);
  EXPECT_EQ(A.ncols(), 0);
  EXPECT_EQ(A.nnnz(), 0);
  EXPECT_EQ(A.row_offsets().size(), 0);
  EXPECT_EQ(A.column_indices().size(), 0);
  EXPECT_EQ(A.values().size(), 0);
  EXPECT_EQ(A.row_offsets().data(), nullptr);
  EXPECT_EQ(A.column_indices().data(), nullptr);
  EXPECT_EQ(A.values().data(), nullptr);

  HostMatrix Ah;
  EXPECT_EQ(Ah.nrows(), 0);
  EXPECT_EQ(Ah.ncols(), 0);
  EXPECT_EQ(Ah.nnnz(), 0);
  EXPECT_EQ(Ah.row_offsets().size(), 0);
  EXPECT_EQ(Ah.column_indices().size(), 0);
  EXPECT_EQ(Ah.values().size(), 0);
  EXPECT_EQ(Ah.row_offsets().data(), nullptr);
  EXPECT_EQ(Ah.column_indices().data(), nullptr);
  EXPECT_EQ(Ah.values().data(), nullptr);
}

/**
 * @brief Testing the enum value assigned to the container is what we expect it
 * to be i.e CSR_FORMAT.
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, FormatEnum) {
  using Matrix = typename TestFixture::device;

  Matrix A;
  EXPECT_EQ(Morpheus::CSR_FORMAT, A.format_enum());
}

/**
 * @brief Testing the format index assigned to the container is what we expect
 * it to be i.e 1.
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, FormatIndex) {
  using Matrix = typename TestFixture::device;

  Matrix A;
  EXPECT_EQ(1, A.format_index());
}

TYPED_TEST(CsrMatrixUnaryTest, ReferenceByIndex) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz)
  Morpheus::copy(A, Ah);

  VALIDATE_CSR_CONTAINER(Ah, this->Ahref, nrows, nnnz, index_type);

  for (index_type n = 0; n < nrows + 1; n++) {
    EXPECT_EQ(Ah.crow_offsets(n), this->Ahref.row_offsets(n));
  }

  for (index_type n = 0; n < nnnz; n++) {
    EXPECT_EQ(Ah.ccolumn_indices(n), this->Ahref.column_indices(n));
    EXPECT_EQ(Ah.cvalues(n), this->Ahref.values(n));
  }
}

TYPED_TEST(CsrMatrixUnaryTest, Reference) {
  using Matrix                = typename TestFixture::device;
  using HostMatrix            = typename TestFixture::host;
  using index_type            = typename Matrix::index_type;
  using index_array_type      = typename Matrix::index_array_type;
  using value_array_type      = typename Matrix::value_array_type;
  using host_index_array_type = typename index_array_type::HostMirror;
  using host_value_array_type = typename value_array_type::HostMirror;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz)
  Morpheus::copy(A, Ah);

  index_array_type roff = A.row_offsets();
  index_array_type cind = A.column_indices();
  value_array_type vals = A.values();

  host_index_array_type roff_h(nrows + 1, 0);
  Morpheus::copy(roff, roff_h);
  host_index_array_type cind_h(nnnz, 0);
  Morpheus::copy(cind, cind_h);
  host_value_array_type vals_h(nnnz, 0);
  Morpheus::copy(vals, vals_h);

  for (index_type n = 0; n < nrows + 1; n++) {
    EXPECT_EQ(roff_h[n], this->Ahref.row_offsets(n));
  }

  for (index_type n = 0; n < nnnz; n++) {
    EXPECT_EQ(cind_h[n], this->Ahref.column_indices(n));
    EXPECT_EQ(vals_h[n], this->Ahref.values(n));
  }
}

TYPED_TEST(CsrMatrixUnaryTest, ConstReference) {
  using Matrix                = typename TestFixture::device;
  using HostMatrix            = typename TestFixture::host;
  using index_type            = typename Matrix::index_type;
  using index_array_type      = typename Matrix::index_array_type;
  using value_array_type      = typename Matrix::value_array_type;
  using host_index_array_type = typename index_array_type::HostMirror;
  using host_value_array_type = typename value_array_type::HostMirror;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  const index_array_type roff = A.crow_offsets();
  const index_array_type cind = A.ccolumn_indices();
  const value_array_type vals = A.cvalues();
  host_index_array_type roff_test(nrows + 1, 0);
  Morpheus::copy(roff, roff_test);
  host_index_array_type cind_test(nnnz, 0);
  Morpheus::copy(cind, cind_test);
  host_value_array_type vals_test(nnnz, 0);
  Morpheus::copy(vals, vals_test);

  for (index_type n = 0; n < nrows + 1; n++) {
    EXPECT_EQ(roff_test[n], this->Ahref.row_offsets(n));
  }
  for (index_type n = 0; n < nnnz; n++) {
    EXPECT_EQ(cind_test[n], this->Ahref.column_indices(n));
    EXPECT_EQ(vals_test[n], this->Ahref.values(n));
  }

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz);
  Morpheus::copy(A, Ah);

  const host_index_array_type roff_h = Ah.crow_offsets();
  const host_index_array_type cind_h = Ah.ccolumn_indices();
  const host_value_array_type vals_h = Ah.cvalues();

  for (index_type n = 0; n < nrows + 1; n++) {
    EXPECT_EQ(roff_h[n], this->Ahref.row_offsets(n));
  }
  for (index_type n = 0; n < nnnz; n++) {
    EXPECT_EQ(cind_h[n], this->Ahref.column_indices(n));
    EXPECT_EQ(vals_h[n], this->Ahref.values(n));
  }
}

/**
 * @brief Testing default copy assignment of CooMatrix container from another
 * CooMatrix container with the same parameters. Resulting container should be
 * a shallow copy of the original.
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, DefaultCopyAssignment) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz);

  Morpheus::copy(A, Ah);
  VALIDATE_CSR_CONTAINER(Ah, this->Ahref, nrows, nnnz, index_type);

  // Default copy asssignment
  HostMatrix Bh = Ah;
  CHECK_CSR_CONTAINERS(Bh, Ah);

  // Change values in one container
  Ah.row_offsets(2)    = 2;
  Ah.column_indices(1) = 1;
  Ah.values(3)         = -3.33;

  // Other container should reflect the same changes
  VALIDATE_CSR_CONTAINER(Bh, Ah, nrows, nnnz, index_type);

  // Now check device Matrix
  Matrix B = A;
  CHECK_CSR_CONTAINERS(B, A);
  Morpheus::copy(Ah, A);

  // Send other vector back to host for check
  HostMatrix Bt(nrows, ncols, nnnz);
  Morpheus::copy(B, Bt);
  VALIDATE_CSR_CONTAINER(Bt, Ah, nrows, nnnz, index_type);
}

/**
 * @brief Testing default copy constructor of CooMatrix container from another
 * CooMatrix container with the same parameters. Resulting container should be
 * a shallow copy of the original.
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, DefaultCopyConstructor) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz);

  Morpheus::copy(A, Ah);
  VALIDATE_CSR_CONTAINER(Ah, this->Ahref, nrows, nnnz, index_type);

  // Default copy asssignment
  HostMatrix Bh(Ah);
  CHECK_CSR_CONTAINERS(Bh, Ah);

  // Change values in one container
  Ah.row_offsets(2)    = 2;
  Ah.column_indices(1) = 1;
  Ah.values(3)         = -3.33;

  // Other container should reflect the same changes
  VALIDATE_CSR_CONTAINER(Bh, Ah, nrows, nnnz, index_type);

  // Now check device Matrix
  Matrix B(A);
  CHECK_CSR_CONTAINERS(B, A);
  Morpheus::copy(Ah, A);

  // Send other vector back to host for check
  HostMatrix Bt(nrows, ncols, nnnz);
  Morpheus::copy(B, Bt);
  VALIDATE_CSR_CONTAINER(Bt, Ah, nrows, nnnz, index_type);
}

/**
 * @brief Testing default move assignment of CooMatrix container from another
 * CooMatrix container with the same parameters. Resulting container should be
 * a shallow copy of the original.
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, DefaultMoveAssignment) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz);

  Morpheus::copy(A, Ah);
  VALIDATE_CSR_CONTAINER(Ah, this->Ahref, nrows, nnnz, index_type);

  // Default copy asssignment
  HostMatrix Bh = std::move(Ah);
  CHECK_CSR_CONTAINERS(Bh, Ah);

  // Change values in one container
  Ah.row_offsets(2)    = 2;
  Ah.column_indices(1) = 1;
  Ah.values(3)         = -3.33;

  // Other container should reflect the same changes
  VALIDATE_CSR_CONTAINER(Bh, Ah, nrows, nnnz, index_type);

  // Now check device Matrix
  Matrix B = std::move(A);
  CHECK_CSR_CONTAINERS(B, A);
  Morpheus::copy(Ah, A);

  // Send other vector back to host for check
  HostMatrix Bt(nrows, ncols, nnnz);
  Morpheus::copy(B, Bt);
  VALIDATE_CSR_CONTAINER(Bt, Ah, nrows, nnnz, index_type);
}

/**
 * @brief Testing default move construction of CooMatrix container from
 * another CooMatrix container with the same parameters. Resulting container
 * should be a shallow copy of the original.
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, DefaultMoveConstructor) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz);

  Morpheus::copy(A, Ah);
  VALIDATE_CSR_CONTAINER(Ah, this->Ahref, nrows, nnnz, index_type);

  // Default copy asssignment
  HostMatrix Bh(std::move(Ah));
  CHECK_CSR_CONTAINERS(Bh, Ah);

  // Change values in one container
  Ah.row_offsets(2)    = 2;
  Ah.column_indices(1) = 1;
  Ah.values(3)         = -3.33;

  // Other container should reflect the same changes
  VALIDATE_CSR_CONTAINER(Bh, Ah, nrows, nnnz, index_type);

  // Now check device Matrix
  Matrix B(std::move(A));
  CHECK_CSR_CONTAINERS(B, A);
  Morpheus::copy(Ah, A);

  // Send other vector back to host for check
  HostMatrix Bt(nrows, ncols, nnnz);
  Morpheus::copy(B, Bt);
  VALIDATE_CSR_CONTAINER(Bt, Ah, nrows, nnnz, index_type);
}

TYPED_TEST(CsrMatrixUnaryTest, ConstructionFromShape) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;
  using value_type = typename Matrix::value_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz);

  for (index_type n = 0; n < nrows + 1; n++) {
    EXPECT_EQ(Ah.row_offsets(n), (index_type)0);
  }
  for (index_type n = 0; n < nnnz; n++) {
    EXPECT_EQ(Ah.column_indices(n), (index_type)0);
    EXPECT_EQ(Ah.values(n), (value_type)0);
  }

  build_csrmatrix(Ah);

  Matrix A(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(A, nrows, ncols, nnnz)
  // Send vectors to device
  Morpheus::copy(Ah, A);

  HostMatrix Ah_test(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah_test, nrows, ncols, nnnz)
  Morpheus::copy(A, Ah_test);

  VALIDATE_CSR_CONTAINER(Ah_test, Ah, nrows, nnnz, index_type);
}

// /**
//  * @brief Testing construction of CooMatrix from a raw pointers
//  *
//  */
// TYPED_TEST(CsrMatrixUnaryTest, ConstructionFromPointers) { EXPECT_EQ(1, 0); }

/**
 * @brief Testing construction of CooMatrix from \p DenseVector arrays.
 *
 */
TYPED_TEST(CsrMatrixUnaryTest, ConstructionFromDenseVector) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  // Build matrix from the device vectors
  Matrix A(nrows, ncols, nnnz, this->Aref.row_offsets(),
           this->Aref.column_indices(), this->Aref.values());
  CHECK_CSR_CONTAINERS(A, this->Aref);

  HostMatrix Ah(nrows, ncols, nnnz);
  CHECK_CSR_SIZES(Ah, nrows, ncols, nnnz);

  Morpheus::copy(A, Ah);
  VALIDATE_CSR_CONTAINER(Ah, this->Ahref, nrows, nnnz, index_type);

  HostMatrix Ah_test(nrows, ncols, nnnz);
  Morpheus::copy(A, Ah_test);

  VALIDATE_CSR_CONTAINER(Ah, Ah_test, nrows, nnnz, index_type);

  Ah.row_offsets(2) = 2;
  EXPECT_NE(Ah.row_offsets(2), Ah_test.row_offsets(2));
  Ah.column_indices(1) = 1;
  EXPECT_NE(Ah.column_indices(1), Ah_test.column_indices(1));
  Ah.values(0) = -1.11;
  EXPECT_NE(Ah.values(0), Ah_test.values(0));
}

TYPED_TEST(CsrMatrixUnaryTest, Resize) {
  using Matrix     = typename TestFixture::device;
  using HostMatrix = typename TestFixture::host;
  using index_type = typename Matrix::index_type;

  index_type nrows = 3, ncols = 3, nnnz = 4;
  index_type large_nrows = 500, large_ncols = 400, large_nnnz = 640;
  index_type small_nrows = 2, small_ncols = 3, small_nnnz = 2;

  Matrix A(nrows, ncols, nnnz);
  Morpheus::copy(this->Ahref, A);

  // Resize to larger shape and non-zeros
  A.resize(large_nrows, large_ncols, large_nnnz);
  CHECK_CSR_SIZES(A, large_nrows, large_ncols, large_nnnz);

  HostMatrix Ah(large_nrows, large_ncols, large_nnnz);
  CHECK_CSR_SIZES(Ah, large_nrows, large_ncols, large_nnnz);

  Morpheus::copy(A, Ah);
  VALIDATE_CSR_CONTAINER(Ah, this->Ahref, nrows, nnnz, index_type);
  for (index_type n = nrows + 1; n < Ah.nrows() + 1; n++) {
    EXPECT_EQ(Ah.row_offsets(n), 0);
  }
  for (index_type n = nnnz; n < Ah.nnnz(); n++) {
    EXPECT_EQ(Ah.column_indices(n), 0);
    EXPECT_EQ(Ah.values(n), 0);
  }

  // Resizing to larger sizes should invoke a new allocation so changes in
  // matrix should not be reflected in reference
  Ah.row_offsets(1)    = 1;
  Ah.column_indices(2) = 10;
  Ah.values(0)         = -1.11;
  Morpheus::copy(Ah, A);

  // Copy reference back to see if there are any changes
  HostMatrix Ahref_test(nrows, ncols, nnnz);
  Morpheus::copy(this->Ahref, Ahref_test);
  EXPECT_NE(Ah.row_offsets(1), Ahref_test.row_offsets(1));
  EXPECT_NE(Ah.column_indices(2), Ahref_test.column_indices(2));
  EXPECT_NE(Ah.values(0), Ahref_test.values(0));

  for (index_type n = nrows + 1; n < Ah.nrows() + 1; n++) {
    EXPECT_EQ(Ah.row_offsets(n), 0);
  }
  for (index_type n = nnnz; n < Ah.nnnz(); n++) {
    EXPECT_EQ(Ah.column_indices(n), 0);
    EXPECT_EQ(Ah.values(n), 0);
  }

  // Resize to smaller shape and non-zeros
  A.resize(small_nrows, small_ncols, small_nnnz);
  CHECK_CSR_SIZES(A, small_nrows, small_ncols, small_nnnz);
  Ah.resize(small_nrows, small_ncols, small_nnnz);
  CHECK_CSR_SIZES(Ah, small_nrows, small_ncols, small_nnnz);

  // Set back to normal
  Ah.row_offsets(1)    = 2;
  Ah.column_indices(2) = 1;
  Ah.values(0)         = 1.11;
  Morpheus::copy(Ah, A);

  VALIDATE_CSR_CONTAINER(Ah, Ahref_test, Ah.nrows(), Ah.nnnz(), index_type);
}

}  // namespace Test

#endif  // TEST_CORE_TEST_CSRMATRIX_HPP