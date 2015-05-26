/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkSinRegularizedHeavisideStepFunction_hxx
#define __itkSinRegularizedHeavisideStepFunction_hxx

#include "itkSinRegularizedHeavisideStepFunction.h"
#include "vnl/vnl_math.h"

namespace itk
{
template< typename TInput, typename TOutput >
SinRegularizedHeavisideStepFunction< TInput, TOutput >
::SinRegularizedHeavisideStepFunction() : Superclass()
{}

template< typename TInput, typename TOutput >
SinRegularizedHeavisideStepFunction< TInput, TOutput >
::~SinRegularizedHeavisideStepFunction()
{}


template< typename TInput, typename TOutput >
typename SinRegularizedHeavisideStepFunction< TInput, TOutput >::OutputType
SinRegularizedHeavisideStepFunction< TInput, TOutput >
::Evaluate( const InputType& input ) const
{
  if ( static_cast< RealType >( input ) >= this->GetEpsilon() )
    {
    return NumericTraits< OutputType >::One;
    }
  else
    {
    if ( static_cast< RealType >( input ) <= -this->GetEpsilon() )
      {
      return NumericTraits< OutputType >::Zero;
      }
    else
      {
      const RealType angleFactor = 0.5 * vnl_math::pi * this->GetOneOverEpsilon();
      const RealType angle = input * angleFactor;

      return static_cast< OutputType >( 0.5 * ( 1.0 + vcl_sin( angle ) ) );
      }
    }
}

template< typename TInput, typename TOutput >
typename SinRegularizedHeavisideStepFunction< TInput, TOutput >::OutputType
SinRegularizedHeavisideStepFunction< TInput, TOutput >
::EvaluateDerivative(const InputType & input) const
{
  if ( vnl_math_abs( static_cast< RealType >( input ) ) >= this->GetEpsilon() )
    {
    return NumericTraits< OutputType >::Zero;
    }
  else
    {
    const RealType angleFactor = 0.5 * vnl_math::pi * this->GetOneOverEpsilon();
    const RealType angle = input * angleFactor;

    return static_cast< OutputType >( 0.5 * angleFactor * vcl_cos(angle) );
    }
}

}

#endif
