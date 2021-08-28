/**
 * Morpheus_DynamicMatrix.hpp
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

#ifndef MORPHEUS_DYNAMICMATRIX_HPP
#define MORPHEUS_DYNAMICMATRIX_HPP

#include <Morpheus_Copy.hpp>
#include <Morpheus_Print.hpp>
#include <Morpheus_FormatTags.hpp>

#include <impl/Morpheus_MatrixBase.hpp>
#include <impl/Morpheus_DynamicMatrix_Impl.hpp>

#include <iostream>
#include <string>
#include <variant>
#include <functional>

namespace Morpheus {

template <class ValueType, class... Properties>
class DynamicMatrix
    : public Impl::MatrixBase<DynamicMatrix, ValueType, Properties...> {
 public:
  using traits = Impl::ContainerTraits<DynamicMatrix, ValueType, Properties...>;
  using type   = typename traits::type;
  using base   = Impl::MatrixBase<DynamicMatrix, ValueType, Properties...>;
  using tag    = typename MatrixFormatTag<Morpheus::DynamicTag>::tag;

  using value_type           = typename traits::value_type;
  using non_const_value_type = typename traits::non_const_value_type;
  using size_type            = typename traits::index_type;
  using index_type           = typename traits::index_type;
  using non_const_index_type = typename traits::non_const_index_type;
  using array_layout         = typename traits::array_layout;

  using memory_space    = typename traits::memory_space;
  using execution_space = typename traits::execution_space;
  using device_type     = typename traits::device_type;

  using HostMirror       = typename traits::HostMirror;
  using host_mirror_type = typename traits::host_mirror_type;

  using pointer         = typename traits::pointer;
  using const_pointer   = typename traits::const_pointer;
  using reference       = typename traits::reference;
  using const_reference = typename traits::const_reference;

  using variant_type =
      typename MatrixFormats<ValueType, Properties...>::variant;

  ~DynamicMatrix()                     = default;
  DynamicMatrix(const DynamicMatrix &) = default;
  DynamicMatrix(DynamicMatrix &&)      = default;
  DynamicMatrix &operator=(const DynamicMatrix &) = default;
  DynamicMatrix &operator=(DynamicMatrix &&) = default;

  inline DynamicMatrix() : base("DynamicMatrix"), _formats() {}

  template <typename Matrix>
  inline DynamicMatrix(
      const Matrix &src,
      typename std::enable_if<is_variant_member_v<Matrix, variant_type>>::type
          * = nullptr)
      : base("DynamicMatrix", src.nrows(), src.ncols(), src.nnnz()),
        _formats(src) {}

  template <typename Matrix>
  inline DynamicMatrix(
      const std::string name, const Matrix &src,
      typename std::enable_if<is_variant_member_v<Matrix, variant_type>>::type
          * = nullptr)
      : base(name + "DynamicMatrix", src.nrows(), src.ncols(), src.nnnz()),
        _formats(src) {}

  // Assignment from another matrix type
  template <typename Matrix>
  typename std::enable_if<is_variant_member_v<Matrix, variant_type>,
                          DynamicMatrix &>::type
  operator=(const Matrix &matrix) {
    _formats = matrix;
    return *this;
  }

  // template <typename Matrix>
  // inline DynamicMatrix(
  //     const std::string name, const Matrix &src,
  //     typename std::enable_if<is_variant_member_v<Matrix,
  //     variant_type>>::type
  //         * = nullptr)
  //     : base(name + "DynamicMatrix", src.nrows(), src.ncols(), src.nnnz()),
  //       _formats(src) {}

  // !CHECK_IF_VALID
  // !FIXME: Remove deep copy and perform shallow copy
  // Construct from another matrix type
  template <class VR, class... PR>
  DynamicMatrix(
      const DynamicMatrix<VR, PR...> &src,
      typename std::enable_if<is_compatible_type<
          DynamicMatrix, DynamicMatrix<VR, PR...>>::value>::type * = nullptr)
      : base(src.name() + "(ShallowCopy)", src.nrows(), src.ncols(),
             src.nnnz()),
        _formats(src.formats()) {}

  // // !CHECK_IF_VALID
  // // !FIXME: Remove deep copy and perform shallow copy
  // // Assignment from another matrix type
  // template <class VR, class... PR>
  // typename std::enable_if<
  //     is_compatible_type<DynamicMatrix, DynamicMatrix<VR, PR...>>::value,
  //     DynamicMatrix &>::type
  // operator=(const DynamicMatrix<VR, PR...> &src) {
  //   if (this != &src) {
  //     set_name(src.name());
  //     set_nrows(src.nrows());
  //     set_ncols(src.ncols());
  //     set_nnnz(src.nnnz());
  //     _formats = src.formats();
  //   }
  //   return *this;
  // }

  template <typename... Args>
  inline void resize(const index_type m, const index_type n,
                     const index_type nnz, Args &&... args) {
    base::resize(m, n, nnz);
    auto f = std::bind(Impl::any_type_resize<ValueType, Properties...>(),
                       std::placeholders::_1, m, n, nnz,
                       std::forward<Args>(args)...);
    return std::visit(f, _formats);
  }

  // template <class VR, class... PR>
  // inline DynamicMatrix &allocate(const std::string name,
  //                                const DynamicMatrix<VR, PR...> &src) {
  //   this->set_name(name);
  //   base::resize(src.nrows(), src.ncols(), src.nnnz());
  //   auto f   = std::bind(Impl::any_type_allocate(), std::placeholders::_1,
  //   src); _formats = std::visit(f, _formats); return *this;
  // }

  template <typename Matrix>
  inline DynamicMatrix &allocate(
      const std::string name, const Matrix &src,
      typename std::enable_if<is_variant_member_v<Matrix, variant_type>>::type
          * = nullptr) {
    this->set_name(name);
    base::resize(src.nrows(), src.ncols(), src.nnnz());
    auto f   = std::bind(Impl::any_type_allocate(), std::cref(src),
                       std::placeholders::_1);
    _formats = Matrix();  // switch to src format
    std::visit(f, _formats);

    return *this;
  }

  inline std::string active_name() const {
    return std::visit(Impl::any_type_get_name(), _formats);
  }

  inline int active_index() const { return _formats.index(); }

  inline void activate(const formats_e index) {
    constexpr int size = std::variant_size_v<
        typename MatrixFormats<ValueType, Properties...>::variant>;
    const int idx = static_cast<int>(index);

    if (idx > size) {
      std::cout << "Warning: There are " << size
                << " available formats to switch to. "
                << "Selecting to switch to format with index " << idx
                << " will default to the format with index 0." << std::endl;
    }
    Impl::activate_impl<size, ValueType, Properties...>::activate(_formats,
                                                                  idx);
  }

  // Enable switching through direct integer indexing
  inline void activate(const int index) {
    activate(static_cast<formats_e>(index));
  }

  // template <typename MatrixType>
  // inline void convert(const MatrixType &matrix) {
  //   _formats = matrix;
  // }

  // inline void convert(const formats_e index) {
  //   Morpheus::CooMatrix<ValueType, Properties...> temp;
  //   Morpheus::copy(*this, temp);
  //   activate(index);
  //   Morpheus::copy(temp, *this);
  // }

  // inline void convert(const int index) {
  //   convert(static_cast<formats_e>(index));
  // }

  inline const variant_type &const_formats() const { return _formats; }
  inline variant_type &formats() { return _formats; }

 private:
  variant_type _formats;
};
}  // namespace Morpheus

#endif  // MORPHEUS_DYNAMICMATRIX_HPP