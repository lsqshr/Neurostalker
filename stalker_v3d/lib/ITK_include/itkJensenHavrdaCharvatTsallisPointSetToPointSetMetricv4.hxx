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
#ifndef __itkJensenHavrdaCharvatTsallisPointSetToPointSetMetricv4_hxx
#define __itkJensenHavrdaCharvatTsallisPointSetToPointSetMetricv4_hxx

#include "itkJensenHavrdaCharvatTsallisPointSetToPointSetMetricv4.h"

namespace itk {

/** Constructor */
template<typename TPointSet>
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4() :
  m_UseAnisotropicCovariances( false ),
  m_PointSetSigma( static_cast<RealType>( 1.0 ) ),
  m_KernelSigma( static_cast<RealType>( 10.0 ) ),
  m_CovarianceKNeighborhood( static_cast<unsigned int>( 5 ) ),
  m_EvaluationKNeighborhood( static_cast<unsigned int>( 50 ) ),
  m_Alpha( static_cast<RealType>(1.0) )
{
}

/** Destructor */
template<typename TPointSet>
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::~JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4()
{
}

/** Initialize the metric */
template<typename TPointSet>
void
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::Initialize( void ) throw ( ExceptionObject )
{
  Superclass::Initialize();

  // Initialize the fixed density function
  this->m_FixedDensityFunction = DensityFunctionType::New();
  this->m_FixedDensityFunction->SetKernelSigma( this->m_KernelSigma );
  this->m_FixedDensityFunction->SetRegularizationSigma( this->m_PointSetSigma );
  this->m_FixedDensityFunction->SetNormalize( true );

  this->m_FixedDensityFunction->SetUseAnisotropicCovariances( this->m_UseAnisotropicCovariances );

  this->m_FixedDensityFunction->SetCovarianceKNeighborhood( this->m_CovarianceKNeighborhood );

  this->m_FixedDensityFunction->SetEvaluationKNeighborhood( this->m_EvaluationKNeighborhood );

  this->m_FixedDensityFunction->SetInputPointSet( this->m_FixedTransformedPointSet );

  // Initialize the moving density function
  this->m_MovingDensityFunction = DensityFunctionType::New();
  this->m_MovingDensityFunction->SetKernelSigma( this->m_KernelSigma );
  this->m_MovingDensityFunction->SetRegularizationSigma( this->m_PointSetSigma );

  this->m_MovingDensityFunction->SetNormalize( true );

  this->m_MovingDensityFunction->SetUseAnisotropicCovariances( this->m_UseAnisotropicCovariances );

  this->m_MovingDensityFunction->SetCovarianceKNeighborhood( this->m_CovarianceKNeighborhood );

  this->m_MovingDensityFunction->SetEvaluationKNeighborhood( this->m_EvaluationKNeighborhood );

  this->m_MovingDensityFunction->SetInputPointSet( this->m_MovingTransformedPointSet );
}

template<typename TPointSet>
void
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::InitializeForIteration() const
{
  Superclass::InitializeForIteration();
  if( this->m_NumberOfValidPoints == 0 )
    {
    itkExceptionMacro("There are no fixed points within the virtual domain.");
    }

  // Pre-calc some values for efficiency
  this->m_TotalNumberOfPoints = static_cast<RealType>( this->m_NumberOfValidPoints + this->m_MovingDensityFunction->GetInputPointSet()->GetNumberOfPoints() );
  this->m_Prefactor0 = -1.0 / static_cast<RealType>( this->m_TotalNumberOfPoints );
  if( this->m_Alpha != 1.0 )
    {
    this->m_Prefactor0 /= ( this->m_Alpha - 1.0 );
    }
  this->m_Prefactor1 = 1.0 / ( this->m_TotalNumberOfPoints * this->m_TotalNumberOfPoints );
}

template<typename TPointSet>
typename JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::MeasureType
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::GetLocalNeighborhoodValue( const PointType & point, const PixelType & itkNotUsed( pixel ) ) const
{
  MeasureType value;
  LocalDerivativeType derivative;
  this->ComputeValueAndDerivative( point, value, derivative, true, false );
  return value;
}

/** Get both the match Measure and the Derivative Measure  */
template<typename TPointSet>
void
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::GetLocalNeighborhoodValueAndDerivative( const PointType & point, MeasureType & value, LocalDerivativeType & derivative, const PixelType & itkNotUsed( pixel ) ) const
{
  this->ComputeValueAndDerivative( point, value, derivative, true, true );
}

template<typename TPointSet>
void
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::ComputeValueAndDerivative( const PointType & samplePoint, MeasureType & value, LocalDerivativeType & derivativeReturn, bool calcValue, bool calcDerivative ) const
{
  if( calcDerivative )
    {
    derivativeReturn.Fill( NumericTraits<DerivativeValueType>::Zero );
    }
  value = NumericTraits<MeasureType>::Zero;

  /**
   * first term only
   */
  typename PointSetType::PointIdentifier numberOfMovingPoints = this->m_MovingDensityFunction->GetInputPointSet()->GetNumberOfPoints();
  RealType probabilityStar = this->m_MovingDensityFunction->Evaluate( samplePoint ) * static_cast<RealType>( numberOfMovingPoints );

  probabilityStar /= this->m_TotalNumberOfPoints;

  if( probabilityStar == NumericTraits<RealType>::Zero )
    {
    return;
    }

  if( calcValue )
    {
    RealType realOne = NumericTraits<RealType>::One;
    if( this->m_Alpha == realOne )
      {
      value = ( vcl_log( probabilityStar ) );
      }
    else
      {
      value = -realOne * ( vcl_pow( probabilityStar, static_cast<RealType>( this->m_Alpha - realOne ) ) );
      }
    value *= this->m_Prefactor0;
    }

  if( calcDerivative )
    {
    RealType probabilityStarFactor = vcl_pow( probabilityStar, static_cast<RealType>( 2.0 - this->m_Alpha ) );

    typename DensityFunctionType::NeighborsIdentifierType neighbors;
    this->m_MovingDensityFunction->GetPointsLocator()->FindClosestNPoints( samplePoint, this->m_EvaluationKNeighborhood, neighbors );

    for( SizeValueType n = 0; n < neighbors.size(); n++ )
      {
      RealType gaussian = this->m_MovingDensityFunction->GetGaussian( neighbors[n] )->Evaluate( samplePoint );

      if( gaussian == NumericTraits<RealType>::Zero )
        {
        continue;
        }

      typename GaussianType::MeanVectorType mean = this->m_MovingDensityFunction->GetGaussian( neighbors[n] )->GetMean();

      Array<CoordRepType> diffMean( PointDimension );
      for( unsigned int i = 0; i < PointDimension; i++ )
        {
        diffMean[i] = mean[i] - samplePoint[i];
        }

      if( this->m_UseAnisotropicCovariances )
        {
        typename GaussianType::CovarianceMatrixType Ci = this->m_MovingDensityFunction->GetGaussian( neighbors[n] )->GetInverseCovariance();
        diffMean = Ci * diffMean;
        }
      else
        {
        diffMean /= this->m_FixedDensityFunction->GetGaussian( neighbors[n] )->GetCovariance()(0, 0);
        }

      DerivativeValueType factor = this->m_Prefactor1 * gaussian / probabilityStarFactor;
      for( unsigned int i = 0; i < PointDimension; i++ )
        {
        derivativeReturn[i] -= diffMean[i] * factor;
        }
      }
    }
}

template<typename TPointSet>
::itk::LightObject::Pointer
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::Clone() const
{
  ::itk::LightObject::Pointer smartPtr;
  Pointer copyPtr = Self::New();

  copyPtr->m_MovingPointSet = this->m_MovingPointSet;
  copyPtr->m_FixedPointSet = this->m_FixedPointSet;
  copyPtr->m_Alpha = this->m_Alpha;
  copyPtr->m_PointSetSigma = this->m_PointSetSigma;
  copyPtr->m_KernelSigma = this->m_KernelSigma;
  copyPtr->m_CovarianceKNeighborhood = this->m_CovarianceKNeighborhood;
  copyPtr->m_EvaluationKNeighborhood = this->m_EvaluationKNeighborhood;
  copyPtr->m_UseAnisotropicCovariances = this->m_UseAnisotropicCovariances;

  smartPtr = static_cast<Pointer>( copyPtr );

  return smartPtr;
}

template<typename TPointSet>
void
JensenHavrdaCharvatTsallisPointSetToPointSetMetricv4<TPointSet>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Alpha: " << this->m_Alpha << std::endl;
  os << indent << "Point set sigma: " << this->m_PointSetSigma << std::endl;
  os << indent << "Evaluation k-neighborhood: " << this->m_EvaluationKNeighborhood << std::endl;

  if( this->m_UseAnisotropicCovariances )
    {
    os << indent << "Kernel sigma: " << this->m_KernelSigma << std::endl;
    os << indent << "Covariance k-neighborhood: " << this->m_CovarianceKNeighborhood << std::endl;
    }
  else
    {
    os << indent << "Isotropic covariances are used." << std::endl;
    }

}
} // end namespace itk

#endif
