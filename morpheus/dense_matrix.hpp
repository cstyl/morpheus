/*****************************************************************************
 *
 *  dense_matrix.hpp
 *
 *  Edinburgh Parallel Computing Centre (EPCC)
 *
 *  (c) 2020 The University of Edinburgh
 *
 *  Contributing authors:
 *  Christodoulos Stylianou (s1887443@ed.ac.uk)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

/*! \file dense_matrix.hpp
 *  \brief Description
 */

#ifndef MORPHEUS_DENSE_MATRIX_HPP
#define MORPHEUS_DENSE_MATRIX_HPP

#include <cusp/array2d.h>

namespace morpheus
{
	using row_major = cusp::row_major;
	using column_major = cusp::column_major;

	// Currently using the Cusp Interface
	template<typename ValueType, typename MemorySpace, typename Orientation = row_major>
	class dense_matrix : public cusp::array2d<ValueType, MemorySpace, Orientation>
	{
	private:
		using parent_t = cusp::array2d<ValueType, MemorySpace, Orientation>;
	public:
		using size_type = std::size_t;
		using value_type = ValueType;

//		using view = ...;
//		using const_view = ...;

		using orientation = Orientation;

		dense_matrix() = default;

		dense_matrix(size_t num_rows, size_t num_cols)
		: parent_t(num_rows, num_cols)
		{}

		dense_matrix(size_t num_rows, size_t num_cols, const ValueType& value)
		: parent_t(num_rows, num_cols, value)
		{}

		template <typename MatrixType>
		dense_matrix(const MatrixType& matrix);

		void resize(const size_t num_rows, const size_t num_cols)
		{
			parent_t::resize(num_rows, num_cols);
		}

		void swap(dense_matrix& matrix)
		{
			parent_t::swap(matrix);
		}

		dense_matrix& operator=(const dense_matrix& matrix)
		{
			parent_t::operator=(matrix);
			return *this;
		}

		template <typename MatrixType>
		dense_matrix& operator=(const MatrixType& matrix)
		{
			parent_t::operator=(matrix);
			return *this;
		}

		size_t nrows()
		{
			return parent_t::num_rows;
		}

		size_t ncols()
		{
			return parent_t::num_cols;
		}

		size_t nnz()
		{
			return parent_t::num_entries;
		}

	};

}

#endif //MORPHEUS_DENSE_MATRIX_HPP
