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

#ifndef __itkLevelSetBase_hxx
#define __itkLevelSetBase_hxx

#include "itkLevelSetBase.h"

#define UNDEFINED_REGION NumericTraits< RegionType >::One

namespace itk
{

template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::LevelSetBase()
  {}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
bool
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::IsInside( const InputType& iP ) const
{
  return ( this->Evaluate( iP ) <= NumericTraits< OutputType >::Zero );
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::Initialize()
{
  Superclass::Initialize();
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::EvaluateGradientNorm( const InputType& iP, LevelSetDataType& ioData ) const
{
  if( !ioData.GradientNorm.m_Computed )
    {
    if( !ioData.Gradient.m_Computed )
      {
      this->EvaluateGradient( iP, ioData );
      }

    ioData.GradientNorm.m_Computed = true;
    ioData.GradientNorm.m_Value = ioData.Gradient.m_Value.GetNorm();
    }
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
typename
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::OutputRealType
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::EvaluateGradientNorm( const InputType& iP ) const
{
  GradientType grad = this->EvaluateGradient( iP );
  return grad.GetNorm();
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
typename
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::OutputRealType
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::EvaluateMeanCurvature( const InputType& iP ) const
{
  OutputRealType oValue = NumericTraits< OutputRealType >::Zero;

  HessianType   hessian = this->EvaluateHessian( iP );
  GradientType  grad = this->EvaluateGradient( iP );

  for( unsigned int i = 0; i < Dimension; i++ )
    {
    for( unsigned int j = 0; j < Dimension; j++ )
      {
      if( j != i )
        {
        oValue -= grad[i] * grad[j] * hessian[i][j];
        oValue += hessian[j][j] * grad[i] * grad[i];
        }
      }
    }

  OutputRealType gradNorm = grad.GetNorm();

  if( gradNorm > vnl_math::eps )
    {
    oValue /= ( gradNorm * gradNorm * gradNorm );
    }
  else
    {
    oValue /= ( NumericTraits< OutputRealType >::One + gradNorm );
    }

  return oValue;
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::EvaluateMeanCurvature( const InputType& iP, LevelSetDataType& ioData ) const
{
  if( !ioData.MeanCurvature.m_Computed )
    {
    if( !ioData.Hessian.m_Computed )
      {
      EvaluateHessian( iP, ioData );
      }

    if( !ioData.Gradient.m_Computed )
      {
      EvaluateGradient( iP, ioData );
      }

    if( !ioData.GradientNorm.m_Computed )
      {
      EvaluateGradientNorm( iP, ioData );
      }

    ioData.MeanCurvature.m_Computed = true;
    ioData.MeanCurvature.m_Value = NumericTraits< OutputRealType >::Zero;

    for( unsigned int i = 0; i < Dimension; i++ )
      {
      for( unsigned int j = 0; j < Dimension; j++ )
        {
        if( j != i )
          {
          ioData.MeanCurvature.m_Value -= ioData.Gradient.m_Value[i]
              * ioData.Gradient.m_Value[j] * ioData.Hessian.m_Value[i][j];
          ioData.MeanCurvature.m_Value += ioData.Hessian.m_Value[j][j]
              * ioData.Gradient.m_Value[i] * ioData.Gradient.m_Value[i];
          }
        }
      }

    OutputRealType temp = ioData.GradientNorm.m_Value;

    if( temp > vnl_math::eps )
      {
      ioData.MeanCurvature.m_Value /= ( temp * temp * temp );
      }
    else
      {
      ioData.MeanCurvature.m_Value /= ( NumericTraits< OutputRealType >::One + temp );
      }
    }
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::UpdateOutputInformation()
{
  if( this->GetSource() )
    {
    this->GetSource()->UpdateOutputInformation();
    }

  // Now we should know what our largest possible region is. If our
  // requested region was not set yet, (or has been set to something
  // invalid - with no data in it ) then set it to the largest
  // possible region.
  if ( m_RequestedNumberOfRegions == 0 )
    {
    this->SetRequestedRegionToLargestPossibleRegion();
    }
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetRequestedRegionToLargestPossibleRegion()
{
  m_RequestedNumberOfRegions  = NumericTraits< RegionType >::One;
  m_RequestedRegion           = NumericTraits< RegionType >::Zero;
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::CopyInformation(const DataObject *data)
{
  const LevelSetBase *levelSet = NULL;

  try
    {
    levelSet = dynamic_cast< const LevelSetBase * >( data );
    }
  catch ( ... )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( LevelSetBase * ).name() );
    }

  if ( !levelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( LevelSetBase * ).name() );
    }

  m_MaximumNumberOfRegions = levelSet->GetMaximumNumberOfRegions();

  m_NumberOfRegions = levelSet->m_NumberOfRegions;
  m_RequestedNumberOfRegions = levelSet->m_RequestedNumberOfRegions;
  m_BufferedRegion  = levelSet->m_BufferedRegion;
  m_RequestedRegion = levelSet->m_RequestedRegion;
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::Graft(const DataObject *data)
{
  // Copy Meta Data
  this->CopyInformation(data);

  const Self * levelSet = NULL;

  try
    {
    levelSet = dynamic_cast< const Self * >( data );
    }
  catch ( ... )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }

  if ( !levelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
bool
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::RequestedRegionIsOutsideOfTheBufferedRegion()
{
  if ( m_RequestedRegion != m_BufferedRegion
       || m_RequestedNumberOfRegions != m_NumberOfRegions )
    {
    return true;
    }

  return false;
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
bool
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::VerifyRequestedRegion()
{
  bool retval = true;

  // Are we asking for more regions than we can get?
  if ( m_RequestedNumberOfRegions > m_MaximumNumberOfRegions )
    {
    itkExceptionMacro(<< "Cannot break object into "
                      << m_RequestedNumberOfRegions << ". The limit is "
                      << m_MaximumNumberOfRegions);
    }

  if ( m_RequestedRegion >= m_RequestedNumberOfRegions )
    {
    itkExceptionMacro(<< "Invalid update region " << m_RequestedRegion
                      << ". Must be between 0 and "
                      << m_RequestedNumberOfRegions - 1);
    }

  return retval;
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetRequestedRegion(const DataObject *data)
{
  const Self *levelSet = dynamic_cast< const Self * >( data );

  if ( levelSet )
    {
    // only copy the RequestedRegion if the parameter is another PointSet
    m_RequestedRegion = levelSet->m_RequestedRegion;
    m_RequestedNumberOfRegions = levelSet->m_RequestedNumberOfRegions;
    }
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetRequestedRegion(const RegionType & region)
{
  if ( m_RequestedRegion != region )
    {
    m_RequestedRegion = region;
    }
}

// ----------------------------------------------------------------------------
template< typename TInput, unsigned int VDimension, typename TOutput, typename TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetBufferedRegion(const RegionType & region)
{
  if ( m_BufferedRegion != region )
    {
    m_BufferedRegion = region;
    this->Modified();
    }
}

} // end namespace itk

#endif // __itkLevelSetBase_hxx
