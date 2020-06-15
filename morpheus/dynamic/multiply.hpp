/*****************************************************************************
 *
 *  multiply.hpp
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

/*! \file multiply.hpp
 *  \brief Description
 */

#ifndef MORPHEUS_DYNAMIC_MULTIPLY_HPP
#define MORPHEUS_DYNAMIC_MULTIPLY_HPP

#include <morpheus/dynamic/matrix.hpp>
#include <morpheus/dynamic/apply_operation.hpp>

#include <morpheus/multiply.hpp>

namespace morpheus
{
	struct multiply_fn
	{
		using result_type = void;

		template <typename T1, typename T2, typename T3>
		result_type operator()(T1 const& A, T2 const& B, T3 & C) const
		{
			morpheus::multiply(A, B, C);
		}

		template <typename Policy, typename T1, typename T2, typename T3>
		result_type operator()(Policy const& exec, T1 const& A, T2 const& B, T3 & C) const
		{
			morpheus::multiply(exec, A, B, C);
		}
	};

	template <typename DerivedPolicy,
			typename Types,
			typename Vector1,
			typename Vector2>
	void multiply(const thrust::detail::execution_policy_base<DerivedPolicy> &exec,
	              matrix<Types> const& A,
	              Vector1 const& B,
	              Vector2 &C)
	{
		apply_operation(A, std::bind(multiply_fn(), std::cref(exec), std::placeholders::_1,
		                             std::cref(B), std::ref(C)));
	}

	template <typename Types, typename Vector1, typename Vector2>
	void multiply(matrix<Types> const& A,
				  Vector1 const& B,
				  Vector2 &C)
	{
		apply_operation(A, std::bind(multiply_fn(), std::placeholders::_1, std::cref(B), std::ref(C)));
	}

}
#endif //MORPHEUS_DYNAMIC_MULTIPLY_HPP
