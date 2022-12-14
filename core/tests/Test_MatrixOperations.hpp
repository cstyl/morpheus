/**
 * Test_MatrixOperations.hpp
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

#ifndef TEST_CORE_TEST_MATRIXOPERATIONS_HPP
#define TEST_CORE_TEST_MATRIXOPERATIONS_HPP

#include <Morpheus_Core.hpp>
#include <utils/Utils.hpp>
#include <utils/Macros_DenseVector.hpp>
#include <utils/Macros_DenseMatrix.hpp>
#include <utils/Macros_CooMatrix.hpp>
#include <utils/Macros_CsrMatrix.hpp>
#include <utils/Macros_DiaMatrix.hpp>
#include <utils/MatrixGenerator.hpp>

using CooMatrixTypes =
    typename Morpheus::generate_unary_typelist<Morpheus::CooMatrix<double>,
                                               types::types_set>::type;

using CsrMatrixTypes =
    typename Morpheus::generate_unary_typelist<Morpheus::CsrMatrix<double>,
                                               types::types_set>::type;

using DiaMatrixTypes =
    typename Morpheus::generate_unary_typelist<Morpheus::DiaMatrix<double>,
                                               types::types_set>::type;

using DenseVectorTypes =
    typename Morpheus::generate_unary_typelist<Morpheus::DenseVector<double>,
                                               types::types_set>::type;

using CooMatrixPairs = generate_pair<CooMatrixTypes, DenseVectorTypes>::type;
using CsrMatrixPairs = generate_pair<CsrMatrixTypes, DenseVectorTypes>::type;
using DiaMatrixPairs = generate_pair<DiaMatrixTypes, DenseVectorTypes>::type;

using pairs = typename Morpheus::concat<
    CooMatrixPairs,
    typename Morpheus::concat<CsrMatrixPairs, DiaMatrixPairs>::type>::type;

using MatrixOperationsTypes = to_gtest_types<pairs>::type;

template <typename Containers>
class MatrixOperationsTypesTest : public ::testing::Test {
 public:
  using type              = Containers;
  using mat_container_t   = typename Containers::first_type::type;
  using vec_container_t   = typename Containers::second_type::type;
  using mat_dev_t         = typename mat_container_t::type;
  using mat_host_t        = typename mat_container_t::type::HostMirror;
  using vec_dev_t         = typename vec_container_t::type;
  using vec_host_t        = typename vec_container_t::type::HostMirror;
  using ValueType         = typename mat_dev_t::value_type;
  using IndexType         = typename mat_dev_t::index_type;
  using ArrayLayout       = typename mat_dev_t::array_layout;
  using Backend           = typename mat_dev_t::backend;
  using MirrorArrayLayout = typename mat_host_t::array_layout;
  using MirrorBackend     = typename mat_host_t::backend;

  struct containers {
    using diag_generator = Morpheus::Test::SparseDiagonalMatrixGenerator<
        ValueType, IndexType, MirrorArrayLayout, MirrorBackend>;

    mat_dev_t A;
    vec_host_t ref_diag;

    containers() : A(), ref_diag() {}

    containers(size_t nrows, size_t ncols, std::vector<int>& diag_indexes)
        : A(), ref_diag(ncols, 0) {
      // Generate the diagonal matrix
      diag_generator generator(nrows, ncols, diag_indexes);
      typename diag_generator::DenseMatrix Adense;
      typename diag_generator::SparseMatrix Acoo;
      Adense = generator.generate();
      generator.generate(Acoo);

      for (size_t i = 0; i < (size_t)Adense.ncols(); i++) {
        ref_diag(i) = Adense(i, i);
      }

      // Convert it in the format we are interested in
      mat_host_t Ah;
      Morpheus::convert<Morpheus::Serial>(Acoo, Ah);

      // Copy on device
      A.resize(Ah);
      Morpheus::copy(Ah, A);
    }
  };

  static const int samples = 3;
  size_t nrows[samples]    = {10, 100, 1000};
  size_t ncols[samples]    = {10, 100, 1000};

  struct containers containers[samples];

  void SetUp() override {
    for (size_t i = 0; i < samples; i++) {
      int diag_freq = 5;
      int ndiags    = ((nrows[i] - 1) / diag_freq) * 2 + 1;
      std::vector<int> diags(ndiags, 0);

      diags[0]          = 0;
      size_t diag_count = 1;
      for (int nd = 1; nd < (int)nrows[i]; nd++) {
        if (nd % diag_freq == 0) {
          diags[diag_count]     = nd;
          diags[diag_count + 1] = -nd;
          diag_count += 2;
        }
      }

      struct containers c(nrows[i], ncols[i], diags);
      containers[i] = c;
    }
  }
};

namespace Test {

TYPED_TEST_SUITE(MatrixOperationsTypesTest, MatrixOperationsTypes);

TYPED_TEST(MatrixOperationsTypesTest, UpdateDiagonalCustom) {
  using vec_t      = typename TestFixture::vec_dev_t;
  using vec_h_t    = typename TestFixture::vec_host_t;
  using mat_t      = typename TestFixture::mat_dev_t;
  using mat_h_t    = typename TestFixture::mat_host_t;
  using index_type = typename TestFixture::IndexType;
  using dense_mat_h_t =
      typename TestFixture::containers::diag_generator::DenseMatrix;

  for (index_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    vec_t new_diag(c.A.nrows(), 1);
    mat_t A = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, A);

    Morpheus::update_diagonal<TEST_CUSTOM_SPACE>(A, new_diag);

    mat_h_t Ah = Morpheus::create_mirror_container<Morpheus::Serial>(A);
    Morpheus::copy(A, Ah);

    dense_mat_h_t Adense;
    Morpheus::convert<Morpheus::Serial>(Ah, Adense);
    vec_h_t diag(Adense.nrows(), 0);

    for (size_t row = 0; row < (size_t)Adense.nrows(); row++) {
      diag[row] = Adense(row, row);
    }

    vec_h_t new_diag_h = Morpheus::create_mirror_container(new_diag);
    Morpheus::copy(new_diag, new_diag_h);

    EXPECT_FALSE(Morpheus::Test::is_empty_container(diag));
    for (size_t idx = 0; idx < diag.size(); idx++) {
      if (c.ref_diag[idx] == 0) {
        EXPECT_EQ(diag[idx], c.ref_diag[idx]);
      } else {
        EXPECT_EQ(diag[idx], new_diag_h[idx]);
      }
    }
  }
}

TYPED_TEST(MatrixOperationsTypesTest, UpdateDiagonalGeneric) {
  using vec_t      = typename TestFixture::vec_dev_t;
  using vec_h_t    = typename TestFixture::vec_host_t;
  using mat_t      = typename TestFixture::mat_dev_t;
  using mat_h_t    = typename TestFixture::mat_host_t;
  using index_type = typename TestFixture::IndexType;
  using dense_mat_h_t =
      typename TestFixture::containers::diag_generator::DenseMatrix;

  for (index_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    vec_t new_diag(c.A.nrows(), 1);
    mat_t A = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, A);

    Morpheus::update_diagonal<TEST_GENERIC_SPACE>(A, new_diag);

    mat_h_t Ah = Morpheus::create_mirror_container<Morpheus::Serial>(A);
    Morpheus::copy(A, Ah);

    dense_mat_h_t Adense;
    Morpheus::convert<Morpheus::Serial>(Ah, Adense);
    vec_h_t diag(Adense.nrows(), 0);

    for (size_t row = 0; row < (size_t)Adense.nrows(); row++) {
      diag[row] = Adense(row, row);
    }

    vec_h_t new_diag_h = Morpheus::create_mirror_container(new_diag);
    Morpheus::copy(new_diag, new_diag_h);

    EXPECT_FALSE(Morpheus::Test::is_empty_container(diag));
    for (size_t idx = 0; idx < diag.size(); idx++) {
      if (c.ref_diag[idx] == 0) {
        EXPECT_EQ(diag[idx], c.ref_diag[idx]);
      } else {
        EXPECT_EQ(diag[idx], new_diag_h[idx]);
      }
    }
  }
}

}  // namespace Test

#endif  // TEST_CORE_TEST_MATRIXOPERATIONS_HPP
