/**
 * CsrMatrix_Definition_Utils.hpp
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

#ifndef MORPHEUS_CORE_TESTS_CSRMATRIX_DEFINITION_UTILS_HPP
#define MORPHEUS_CORE_TESTS_CSRMATRIX_DEFINITION_UTILS_HPP

#include <setup/TypeDefinition_Utils.hpp>

template <typename ValueType, typename IndexType, typename ArrayLayout,
          typename Space>
struct CsrMatrixTypes {
  using DefaultValueType   = double;
  using DefaultIndexType   = int;
  using DefaultSpace       = Kokkos::DefaultExecutionSpace;
  using DefaultArrayLayout = typename DefaultSpace::array_layout;

  // <ValueType>
  using v =
      typename Impl::ContainerTester<Morpheus::CsrMatrix<ValueType>, ValueType,
                                     DefaultIndexType, DefaultSpace,
                                     DefaultArrayLayout>::type;

  // <ValueType, ArrayLayout>
  using vl = typename Impl::ContainerTester<
      Morpheus::CsrMatrix<ValueType, ArrayLayout>, ValueType, DefaultIndexType,
      DefaultSpace, ArrayLayout>::type;


  // <ValueType, IndexType, Space>
  using vis = typename Impl::ContainerTester<
      Morpheus::CsrMatrix<ValueType, IndexType, Space>, ValueType, IndexType,
      Space, typename Space::array_layout>::type;

  // <ValueType, IndexType, ArrayLayout>
  using vil = typename Impl::ContainerTester<
      Morpheus::CsrMatrix<ValueType, IndexType, ArrayLayout>, ValueType,
      IndexType, DefaultSpace, ArrayLayout>::type;

  //  <ValueType, IndexType, ArrayLayout, Space>
  using vils = typename Impl::ContainerTester<
      Morpheus::CsrMatrix<ValueType, IndexType, ArrayLayout, Space>, ValueType,
      IndexType, Space, ArrayLayout>::type;

  // <ValueType, ArrayLayout, Space>
  using vls = typename Impl::ContainerTester<
      Morpheus::CsrMatrix<ValueType, ArrayLayout, Space>, ValueType,
      DefaultIndexType, Space, ArrayLayout>::type;
};

// using CsrMatrixUnary =
//     ::testing::Types<typename CsrMatrixTypes<double, int, Kokkos::LayoutRight,
//                                              Kokkos::Serial>::v,
//                      typename CsrMatrixTypes<double, int, Kokkos::LayoutRight,
//                                              Kokkos::Serial>::vl,
//                      typename CsrMatrixTypes<double, int, Kokkos::LayoutRight,
//                                              Kokkos::Serial>::vis,
//                      typename CsrMatrixTypes<double, int, Kokkos::LayoutRight,
//                                              Kokkos::Serial>::vil,
//                      typename CsrMatrixTypes<double, int, Kokkos::LayoutRight,
//                                              Kokkos::Serial>::vils,
//                      typename CsrMatrixTypes<double, int, Kokkos::LayoutRight,
//                                              Kokkos::Serial>::vls>;
using CsrMatrixUnary =
    ::testing::Types<typename CsrMatrixTypes<double, int, Kokkos::LayoutRight,
                                             TEST_EXECSPACE>::vis>;

#endif  // MORPHEUS_CORE_TESTS_CSRMATRIX_DEFINITION_UTILS_HPP
