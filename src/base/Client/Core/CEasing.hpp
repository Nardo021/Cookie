#pragma once

#include <Common/Common.hpp>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace CookieCore
{
	namespace Easing
	{
		inline auto InSine( double t ) -> double
		{
			return std::sin( 1.5707963 * t );
		}

		inline auto OutSine( double t ) -> double
		{
			return 1.0 + std::sin( 1.5707963 * ( --t ) );
		}

		inline auto InOutSine( double t ) -> double
		{
			return 0.5 * ( 1.0 + std::sin( M_PI * ( t - 0.5 ) ) );
		}

		inline auto InQuad( double t ) -> double
		{
			return t * t;
		}

		inline auto OutQuad( double t ) -> double
		{
			return t * ( 2.0 - t );
		}

		inline auto InOutQuad( double t ) -> double
		{
			return t < 0.5 ? 2.0 * t * t : t * ( 4.0 - 2.0 * t ) - 1.0;
		}

		inline auto InCubic( double t ) -> double
		{
			return t * t * t;
		}

		inline auto OutCubic( double t ) -> double
		{
			return 1.0 + ( --t ) * t * t;
		}

		inline auto InOutCubic( double t ) -> double
		{
			return t < 0.5 ? 4.0 * t * t * t : 1.0 + ( --t ) * ( 2.0 * ( --t ) ) * ( 2.0 * t );
		}

		inline auto InQuart( double t ) -> double
		{
			t *= t;
			return t * t;
		}

		inline auto OutQuart( double t ) -> double
		{
			t = ( --t ) * t;
			return 1.0 - t * t;
		}

		inline auto InOutQuart( double t ) -> double
		{
			if ( t < 0.5 )
			{
				t *= t;
				return 8.0 * t * t;
			}

			t = ( --t ) * t;
			return 1.0 - 8.0 * t * t;
		}

		inline auto InQuint( double t ) -> double
		{
			const double t2 = t * t;
			return t * t2 * t2;
		}

		inline auto OutQuint( double t ) -> double
		{
			const double t2 = ( --t ) * t;
			return 1.0 + t * t2 * t2;
		}

		inline auto InOutQuint( double t ) -> double
		{
			double t2 = 0.0;
			if ( t < 0.5 )
			{
				t2 = t * t;
				return 16.0 * t * t2 * t2;
			}

			t2 = ( --t ) * t;
			return 1.0 + 16.0 * t * t2 * t2;
		}

		inline auto InExpo( double t ) -> double
		{
			return ( std::pow( 2.0 , 8.0 * t ) - 1.0 ) / 255.0;
		}

		inline auto OutExpo( double t ) -> double
		{
			return 1.0 - std::pow( 2.0 , -8.0 * t );
		}

		inline auto InOutExpo( double t ) -> double
		{
			if ( t < 0.5 )
				return ( std::pow( 2.0 , 16.0 * t ) - 1.0 ) / 510.0;

			return 1.0 - 0.5 * std::pow( 2.0 , -16.0 * ( t - 0.5 ) );
		}

		inline auto InCirc( double t ) -> double
		{
			return 1.0 - std::sqrt( 1.0 - t );
		}

		inline auto OutCirc( double t ) -> double
		{
			return std::sqrt( t );
		}

		inline auto InOutCirc( double t ) -> double
		{
			if ( t < 0.5 )
				return ( 1.0 - std::sqrt( 1.0 - 2.0 * t ) ) * 0.5;

			return ( 1.0 + std::sqrt( 2.0 * t - 1.0 ) ) * 0.5;
		}

		inline auto InBack( double t ) -> double
		{
			return t * t * ( 2.70158 * t - 1.70158 );
		}

		inline auto OutBack( double t ) -> double
		{
			return 1.0 + ( --t ) * t * ( 2.70158 * t + 1.70158 );
		}

		inline auto InOutBack( double t ) -> double
		{
			if ( t < 0.5 )
				return t * t * ( 7.0 * t - 2.5 ) * 2.0;

			return 1.0 + ( --t ) * t * 2.0 * ( 7.0 * t + 2.5 );
		}

		inline auto InElastic( double t ) -> double
		{
			const double t2 = t * t;
			return t2 * t2 * std::sin( t * M_PI * 4.5 );
		}

		inline auto OutElastic( double t ) -> double
		{
			const double t2 = ( t - 1.0 ) * ( t - 1.0 );
			return 1.0 - t2 * t2 * std::cos( t * M_PI * 4.5 );
		}

		inline auto InOutElastic( double t ) -> double
		{
			double t2 = 0.0;
			if ( t < 0.45 )
			{
				t2 = t * t;
				return 8.0 * t2 * t2 * std::sin( t * M_PI * 9.0 );
			}

			if ( t < 0.55 )
				return 0.5 + 0.75 * std::sin( t * M_PI * 4.0 );

			t2 = ( t - 1.0 ) * ( t - 1.0 );
			return 1.0 - 8.0 * t2 * t2 * std::sin( t * M_PI * 9.0 );
		}

		inline auto InBounce( double t ) -> double
		{
			return std::pow( 2.0 , 6.0 * ( t - 1.0 ) ) * std::abs( std::sin( t * M_PI * 3.5 ) );
		}

		inline auto OutBounce( double t ) -> double
		{
			return 1.0 - std::pow( 2.0 , -6.0 * t ) * std::abs( std::cos( t * M_PI * 3.5 ) );
		}

		inline auto InOutBounce( double t ) -> double
		{
			if ( t < 0.5 )
				return 8.0 * std::pow( 2.0 , 8.0 * ( t - 1.0 ) ) * std::abs( std::sin( t * M_PI * 7.0 ) );

			return 1.0 - 8.0 * std::pow( 2.0 , -8.0 * t ) * std::abs( std::sin( t * M_PI * 7.0 ) );
		}
	}
}

namespace CEasing = CookieCore::Easing;
