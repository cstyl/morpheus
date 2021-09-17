/**
 * Examples_Kokkos.cpp
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

#include <Morpheus_Core.hpp>
#include <iostream>

#if defined(MORPHEUS_ENABLE_CUDA)

using coo = Morpheus::CooMatrix<double, int, Kokkos::CudaSpace>;
using vec = Morpheus::DenseVector<double, Kokkos::CudaSpace>;

int main() {

  Morpheus::initialize();
  {
    coo A("A", 100, 200, 1250);
    vec x("x", 200, 0), y("y", 100, 0);

    std::cout << "<Kokkos::Cuda>" << std::endl;
    Morpheus::multiply<Kokkos::Cuda>(A, x, y);

    std::cout << "<Morpheus::Cuda>" << std::endl;
    Morpheus::multiply<Morpheus::Cuda>(A, x, y);
  }
  Morpheus::finalize();

  return 0;
}

#endif
