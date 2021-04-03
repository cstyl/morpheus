/**
 * dense_vector.hpp
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
#ifndef MORPHEUS_CONTAINERS_DENSE_VECTOR_HPP
#define MORPHEUS_CONTAINERS_DENSE_VECTOR_HPP

#include <string>

#include <morpheus/core/core.hpp>
#include <morpheus/core/vector_traits.hpp>
#include <morpheus/core/vector_tags.hpp>

namespace Morpheus {

/* Forward declaration */
template <class... Properties>
class DenseVector;

struct DenseVectorTag : public Impl::VectorTag {};

/* Alias type to match std::vector */
template <class... Properties>
using vector = DenseVector<Properties...>;

template <class... Properties>
class DenseVector : public Impl::VectorTraits<Properties...> {
 public:
  using type   = DenseVector<Properties...>;
  using traits = Impl::VectorTraits<Properties...>;
  using tag    = typename VectorFormatTag<DenseVectorTag>::tag;

  using value_type = typename traits::value_type;
  using index_type = size_t;

  using memory_space    = typename traits::memory_space;
  using execution_space = typename traits::execution_space;
  using device_type     = typename traits::device_type;

  using pointer         = value_type*;
  using const_pointer   = const value_type*;
  using reference       = value_type&;
  using const_reference = const value_type&;

  using iterator       = value_type*;
  using const_iterator = const value_type*;

  using value_array_type =
      Kokkos::View<value_type*, Kokkos::LayoutRight, device_type>;
  using value_array_pointer   = typename value_array_type::pointer_type;
  using value_array_reference = typename value_array_type::reference_type;

 public:
  //   Member functions
  ~DenseVector()                  = default;
  DenseVector(const DenseVector&) = default;
  DenseVector(DenseVector&&)      = default;
  DenseVector& operator=(const DenseVector&) = default;
  DenseVector& operator=(DenseVector&&) = default;

  inline DenseVector()
      : _name("Vector"), _size(0), _extra_storage(1.1), _values() {}

  inline DenseVector(const std::string name, int n, value_type val = 0)
      : _name(name),
        _size(n),
        _extra_storage(1.1),
        _values(name, size_t(n * 1.1)) {
    assign(n, val);
  }

  inline DenseVector(int n, value_type val = 0)
      : _name("Vector"),
        _size(n),
        _extra_storage(1.1),
        _values("Vector", size_t(n * 1.1)) {
    assign(n, val);
  }

  inline void assign(const index_type n, const value_type val) {
    /* Resize if necessary (behavior of std:vector) */
    this->resize(n);

    Kokkos::RangePolicy<execution_space, index_type> range(0, n);
    Kokkos::parallel_for(
        "Morpheus::DenseVector::assign", range,
        KOKKOS_LAMBDA(const int i) { _values(i) = val; });
  }

  // Element access
  inline value_array_reference operator()(index_type i) const {
    return _values(i);
  }

  inline value_array_reference operator[](index_type i) const {
    return _values(i);
  }

  inline index_type size() const { return _size; }

  inline value_array_pointer data() const { return _values.data(); }

  // Iterators
  inline iterator begin() { return _values.data(); }

  inline iterator end() {
    return _size > 0 ? _values.data() + _size : _values.data();
  }

  inline const_iterator cbegin() const { return _values.data(); }

  inline const_iterator cend() const {
    return _size > 0 ? _values.data() + _size : _values.data();
  }

  // Capacity
  //   TODO: reserve should be enabled when push_back methods etc are developed
  //   inline void reserve(size_t n) {
  //     Kokkos::resize(_values, size_t(n * _extra_storage));
  //   }

  // Modifiers
  inline void resize(index_type n) {
    if (n > _values.span()) Kokkos::resize(_values, size_t(n * _extra_storage));
    _size = n;
  }

  inline void resize(const index_type n, const index_type val) {
    assign(n, val);
  }

  // TODO: Data management routines for copying to and from a space

  // Other
  inline std::string name() const { return _name; }

 private:
  std::string _name;
  index_type _size;
  float _extra_storage;
  value_array_type _values;
};
}  // namespace Morpheus

#endif  // MORPHEUS_CONTAINERS_DENSE_VECTOR_HPP