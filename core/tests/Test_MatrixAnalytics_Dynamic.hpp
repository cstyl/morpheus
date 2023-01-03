/**
 * Test_MatrixAnalytics_Dynamic.hpp
 *
 * EPCC, The University of Edinburgh
 *
 * (c) 2021 - 2023 The University of Edinburgh
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

#ifndef TEST_CORE_TEST_MATRIXANALYTICS_DYNAMIC_HPP
#define TEST_CORE_TEST_MATRIXANALYTICS_DYNAMIC_HPP

#include <Morpheus_Core.hpp>
#include <utils/Utils.hpp>
#include <utils/Macros_DenseVector.hpp>
#include <utils/Macros_DynamicMatrix.hpp>
#include <utils/MatrixGenerator.hpp>

using DynamicMatrixTypes =
    typename Morpheus::generate_unary_typelist<Morpheus::DynamicMatrix<double>,
                                               types::types_set>::type;
using DynamicMatrixAnalyticsTypes = to_gtest_types<DynamicMatrixTypes>::type;

template <typename Containers>
class DynamicMatrixAnalyticsTypesTest : public ::testing::Test {
 public:
  using type            = Containers;
  using mat_container_t = typename Containers::type;
  using vec_container_t =
      Morpheus::DenseVector<typename mat_container_t::index_type,
                            typename mat_container_t::index_type,
                            typename mat_container_t::array_layout,
                            typename mat_container_t::backend>;
  using mat_dev_t         = typename mat_container_t::type;
  using mat_host_t        = typename mat_container_t::type::HostMirror;
  using vec_dev_t         = typename vec_container_t::type;
  using vec_host_t        = typename vec_container_t::type::HostMirror;
  using SizeType          = typename mat_dev_t::size_type;
  using ValueType         = typename mat_dev_t::value_type;
  using IndexType         = typename mat_dev_t::index_type;
  using ArrayLayout       = typename mat_dev_t::array_layout;
  using Backend           = typename mat_dev_t::backend;
  using MirrorArrayLayout = typename mat_host_t::array_layout;
  using MirrorBackend     = typename mat_host_t::backend;

  class ContainersClass {
   public:
    using diag_generator = Morpheus::Test::DiagonalMatrixGenerator<
        ValueType, IndexType, MirrorArrayLayout, MirrorBackend>;

    mat_dev_t A;
    vec_dev_t nnz_per_row;
    ContainersClass() : A(), nnz_per_row() {}

    ContainersClass(SizeType nrows, SizeType ncols,
                    std::vector<int>& diag_indexes)
        : A(), nnz_per_row(nrows, 0) {
      // Generate the diagonal matrix
      diag_generator generator(nrows, ncols, diag_indexes);
      typename diag_generator::DenseMatrix Adense;
      typename diag_generator::SparseMatrix Acoo;
      Adense = generator.generate();
      generator.generate(Acoo);

      // Assign Coo Host matrix to Dynamic Host container
      mat_host_t Ah = Acoo;

      // Copy on device
      A.resize(Ah);
      Morpheus::copy(Ah, A);

      auto nnz_per_row_h = Morpheus::create_mirror_container(nnz_per_row);
      nnz_per_row_h.assign(nnz_per_row_h.size(), 0);

      for (SizeType i = 0; i < nrows; i++) {
        for (SizeType j = 0; j < ncols; j++) {
          if (Adense(i, j) != 0) {
            nnz_per_row_h(i)++;
          }
        }
      }
      Morpheus::copy(nnz_per_row_h, nnz_per_row);
    }
  };

  static const SizeType samples = 3;
  SizeType nrows[samples]       = {10, 100, 1000};
  SizeType ncols[samples]       = {10, 100, 1000};

  ContainersClass containers[samples];

  void SetUp() override {
    for (SizeType i = 0; i < samples; i++) {
      int diag_freq = 5;
      int ndiags    = ((nrows[i] - 1) / diag_freq) * 2 + 1;
      std::vector<int> diags(ndiags, 0);

      diags[0]            = 0;
      SizeType diag_count = 1;
      for (int nd = 1; nd < (int)nrows[i]; nd++) {
        if (nd % diag_freq == 0) {
          diags[diag_count]     = nd;
          diags[diag_count + 1] = -nd;
          diag_count += 2;
        }
      }

      ContainersClass c(nrows[i], ncols[i], diags);
      containers[i] = c;
    }
  }
};

namespace Test {

TYPED_TEST_SUITE(DynamicMatrixAnalyticsTypesTest, DynamicMatrixAnalyticsTypes);

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, NumberOfRows) {
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      auto num_rows = Morpheus::number_of_rows(A);
      EXPECT_EQ(num_rows, c.A.nrows());
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, NumberOfColumns) {
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      auto num_cols = Morpheus::number_of_columns(A);
      EXPECT_EQ(num_cols, c.A.ncols());
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, NumberOfNonZero) {
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      auto num_nnz = Morpheus::number_of_nnz(A);
      EXPECT_EQ(num_nnz, c.A.nnnz());
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, AverageNonZeros) {
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      auto avg_nnnz = Morpheus::average_nnnz(A);
      EXPECT_EQ(avg_nnnz, c.A.nnnz() / c.A.nrows());
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, Density) {
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      auto matrix_density = Morpheus::density(A);
      EXPECT_EQ(matrix_density, c.A.nnnz() / (c.A.nrows() * c.A.ncols()));
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, NonZerosPerRowCustomInit) {
  using vec_t     = typename TestFixture::vec_dev_t;
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      vec_t nnz_per_row(c.A.nrows(), 1);
      Morpheus::count_nnz_per_row<TEST_CUSTOM_SPACE>(A, nnz_per_row);

      EXPECT_FALSE(Morpheus::Test::is_empty_container(nnz_per_row));
      EXPECT_TRUE(
          Morpheus::Test::have_approx_same_data(c.nnz_per_row, nnz_per_row));
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, NonZerosPerRowGenericInit) {
  using vec_t     = typename TestFixture::vec_dev_t;
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      vec_t nnz_per_row(c.A.nrows(), 1);
      Morpheus::count_nnz_per_row<TEST_GENERIC_SPACE>(A, nnz_per_row);

      EXPECT_FALSE(Morpheus::Test::is_empty_container(nnz_per_row));
      EXPECT_TRUE(
          Morpheus::Test::have_approx_same_data(c.nnz_per_row, nnz_per_row));
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, NonZerosPerRowCustom) {
  using vec_t     = typename TestFixture::vec_dev_t;
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      vec_t nnz_per_row(c.A.nrows(), 1);
      Morpheus::count_nnz_per_row<TEST_CUSTOM_SPACE>(A, nnz_per_row, false);

      EXPECT_FALSE(Morpheus::Test::is_empty_container(nnz_per_row));
      EXPECT_FALSE(
          Morpheus::Test::have_approx_same_data(c.nnz_per_row, nnz_per_row));

      auto nnz_per_row_h = Morpheus::create_mirror_container(nnz_per_row);
      Morpheus::copy(nnz_per_row, nnz_per_row_h);
      auto cnnz_per_row_h = Morpheus::create_mirror(c.nnz_per_row);
      Morpheus::copy(c.nnz_per_row, cnnz_per_row_h);
      for (size_type n = 0; n < cnnz_per_row_h.size(); n++) {
        cnnz_per_row_h(n) += 1;
      }
      EXPECT_TRUE(
          Morpheus::Test::have_approx_same_data(cnnz_per_row_h, nnz_per_row_h));
    }
  }
}

TYPED_TEST(DynamicMatrixAnalyticsTypesTest, NonZerosPerRowGeneric) {
  using vec_t     = typename TestFixture::vec_dev_t;
  using size_type = typename TestFixture::SizeType;
  using backend   = typename TestFixture::Backend;

  for (size_type i = 0; i < this->samples; i++) {
    auto c = this->containers[i];

    // Create a duplicate of A on host
    auto Ah = Morpheus::create_mirror(c.A);
    Morpheus::copy(c.A, Ah);

    for (auto fmt_idx = 0; fmt_idx < Morpheus::NFORMATS; fmt_idx++) {
      // Convert to the new active state
      Morpheus::convert<Morpheus::Serial>(Ah, fmt_idx);
      auto A = Morpheus::create_mirror_container<backend>(Ah);
      Morpheus::copy(Ah, A);

      vec_t nnz_per_row(c.A.nrows(), 1);
      Morpheus::count_nnz_per_row<TEST_GENERIC_SPACE>(A, nnz_per_row, false);

      EXPECT_FALSE(Morpheus::Test::is_empty_container(nnz_per_row));
      EXPECT_FALSE(
          Morpheus::Test::have_approx_same_data(c.nnz_per_row, nnz_per_row));

      auto nnz_per_row_h = Morpheus::create_mirror_container(nnz_per_row);
      Morpheus::copy(nnz_per_row, nnz_per_row_h);
      auto cnnz_per_row_h = Morpheus::create_mirror(c.nnz_per_row);
      Morpheus::copy(c.nnz_per_row, cnnz_per_row_h);
      for (size_type n = 0; n < cnnz_per_row_h.size(); n++) {
        cnnz_per_row_h(n) += 1;
      }
      EXPECT_TRUE(
          Morpheus::Test::have_approx_same_data(cnnz_per_row_h, nnz_per_row_h));
    }
  }
}

}  // namespace Test

#endif  // TEST_CORE_TEST_MATRIXANALYTICS_DYNAMIC_HPP
