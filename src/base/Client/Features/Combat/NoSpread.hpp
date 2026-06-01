#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>
#include <cfloat>

#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/TIER0/TIER0_Functions.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace NoSpread
{
    inline constexpr float PI = 3.14159265358979323846f;

    struct Config
    {
        bool enabled = false;
    };

    inline Config config;

    namespace detail
    {
        struct Sha1Context
        {
            uint32_t state[5];
            uint32_t count[2];
            uint8_t  buffer[64];
        };

        inline uint32_t Rol( uint32_t value , int bits )
        {
            return ( value << bits ) | ( value >> ( 32 - bits ) );
        }

        inline void Sha1Transform( uint32_t state[5] , const uint8_t block[64] )
        {
            uint32_t w[80];
            for ( int i = 0; i < 16; ++i )
                w[i] = ( block[i * 4] << 24 ) | ( block[i * 4 + 1] << 16 ) | ( block[i * 4 + 2] << 8 ) | block[i * 4 + 3];
            for ( int i = 16; i < 80; ++i )
                w[i] = Rol( w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16] , 1 );

            uint32_t a = state[0];
            uint32_t b = state[1];
            uint32_t c = state[2];
            uint32_t d = state[3];
            uint32_t e = state[4];

            for ( int i = 0; i < 80; ++i )
            {
                uint32_t f = 0;
                uint32_t k = 0;
                if ( i < 20 ) { f = ( b & c ) | ( ( ~b ) & d ); k = 0x5A827999; }
                else if ( i < 40 ) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
                else if ( i < 60 ) { f = ( b & c ) | ( b & d ) | ( c & d ); k = 0x8F1BBCDC; }
                else { f = b ^ c ^ d; k = 0xCA62C1D6; }

                const uint32_t temp = Rol( a , 5 ) + f + e + k + w[i];
                e = d;
                d = c;
                c = Rol( b , 30 );
                b = a;
                a = temp;
            }

            state[0] += a;
            state[1] += b;
            state[2] += c;
            state[3] += d;
            state[4] += e;
        }

        inline void Sha1Init( Sha1Context* ctx )
        {
            ctx->count[0] = ctx->count[1] = 0;
            ctx->state[0] = 0x67452301;
            ctx->state[1] = 0xEFCDAB89;
            ctx->state[2] = 0x98BADCFE;
            ctx->state[3] = 0x10325476;
            ctx->state[4] = 0xC3D2E1F0;
        }

        inline void Sha1Update( Sha1Context* ctx , const void* data , size_t len )
        {
            const auto* input = static_cast<const uint8_t*>( data );
            size_t i = 0;
            size_t j = ( ctx->count[0] >> 3 ) & 63;

            if ( ( ctx->count[0] += static_cast<uint32_t>( len << 3 ) ) < ( len << 3 ) )
                ctx->count[1]++;
            ctx->count[1] += static_cast<uint32_t>( len >> 29 );

            if ( ( j + len ) > 63 )
            {
                i = 64 - j;
                std::memcpy( &ctx->buffer[j] , input , i );
                Sha1Transform( ctx->state , ctx->buffer );
                for ( ; i + 63 < len; i += 64 )
                    Sha1Transform( ctx->state , &input[i] );
                j = 0;
            }
            else
            {
                i = 0;
            }

            std::memcpy( &ctx->buffer[j] , &input[i] , len - i );
        }

        inline void Sha1Final( Sha1Context* ctx , uint8_t digest[20] )
        {
            uint8_t finalCount[8];
            for ( int i = 0; i < 8; ++i )
                finalCount[i] = static_cast<uint8_t>( ( ctx->count[( i >= 4 ) ? 1 : 0] >> ( ( 3 - ( i & 3 ) ) * 8 ) ) & 255 );

            uint8_t c = 0x80;
            Sha1Update( ctx , &c , 1 );
            while ( ( ctx->count[0] & 504 ) != 448 )
            {
                c = 0;
                Sha1Update( ctx , &c , 1 );
            }
            Sha1Update( ctx , finalCount , 8 );

            for ( int i = 0; i < 20; ++i )
                digest[i] = static_cast<uint8_t>( ( ctx->state[i >> 2] >> ( ( 3 - ( i & 3 ) ) * 8 ) ) & 255 );
        }

        inline float RoundFloat( float value )
        {
            return floorf( value + 0.5f );
        }

        inline uint32_t ComputeRandomSeed( const QAngle& angles , int renderTick )
        {
            struct HashBuffer
            {
                float pitch;
                float yaw;
                int   tick;
            } buffer{};

            float pitch = Math::AngleNormalize( angles.m_x );
            pitch += pitch;
            buffer.pitch = RoundFloat( pitch ) * 0.5f;

            float yaw = Math::AngleNormalize( angles.m_y );
            yaw += yaw;
            buffer.yaw = RoundFloat( yaw ) * 0.5f;

            buffer.tick = renderTick;

            Sha1Context ctx{};
            Sha1Init( &ctx );
            Sha1Update( &ctx , &buffer , sizeof( buffer ) );

            uint8_t digest[20]{};
            Sha1Final( &ctx , digest );
            uint32_t seed = 0;
            std::memcpy( &seed , digest , sizeof( seed ) );
            return seed;
        }

        inline int GetRenderTick( CUserCmd* cmd )
        {
#if DISABLE_PROTOBUF == 0
            if ( cmd && cmd->cmd.input_history_size() > 0 )
            {
                const auto& history = cmd->cmd.input_history( cmd->cmd.input_history_size() - 1 );
                if ( history.has_render_tick_count() )
                    return history.render_tick_count();
            }
#endif
            return 0;
        }

        inline void ApplyRadiusCurve( float& density , int itemIndex , int weaponMode , float recoilIndex )
        {
            if ( itemIndex == 64 && weaponMode == 1 )
                density = 1.0f - density * density;
            else if ( itemIndex == 28 && recoilIndex < 3.f )
            {
                for ( int i = 3; i > static_cast<int>( recoilIndex ); --i )
                    density *= density;
                density = 1.0f - density;
            }
        }

        inline void CalcSpreadValues( int randomSeed , float inaccuracy , float spread , int itemIndex , int weaponMode , float recoilIndex , float& outX , float& outY )
        {
            auto* tier0 = GetTIER0Functions();
            if ( !tier0 || !tier0->RandomSeed_o || !tier0->RandomFloat_o )
            {
                outX = outY = 0.f;
                return;
            }

            tier0->RandomSeed_o( static_cast<uint32_t>( randomSeed + 1 ) );

            float radiusCurveDensity = tier0->RandomFloat_o( 0.f , 1.f );
            ApplyRadiusCurve( radiusCurveDensity , itemIndex , weaponMode , recoilIndex );

            const float theta0 = tier0->RandomFloat_o( 0.f , 2.f * PI );
            const float radius0 = radiusCurveDensity * inaccuracy;
            const float x0 = radius0 * cosf( theta0 );
            const float y0 = radius0 * sinf( theta0 );

            float spreadCurveDensity = tier0->RandomFloat_o( 0.f , 1.f );
            ApplyRadiusCurve( spreadCurveDensity , itemIndex , weaponMode , recoilIndex );

            const float theta1 = tier0->RandomFloat_o( 0.f , 2.f * PI );
            const float radius1 = spreadCurveDensity * spread;
            const float x1 = radius1 * cosf( theta1 );
            const float y1 = radius1 * sinf( theta1 );

            outX = static_cast<float>( ( static_cast<double>( x1 ) * radius0 ) + ( static_cast<double>( x0 ) * radius1 ) );
            outY = static_cast<float>( ( static_cast<double>( y1 ) * radius0 ) + ( static_cast<double>( y0 ) * radius1 ) );
        }

        inline QAngle ApplySpread( const QAngle& view , float spreadX , float spreadY )
        {
            Vector3 forward , right , up;
            Math::AngleVectors( view , forward , right , up );

            Vector3 direction(
                forward.m_x + right.m_x * spreadX + up.m_x * spreadY ,
                forward.m_y + right.m_y * spreadX + up.m_y * spreadY ,
                forward.m_z + right.m_z * spreadX + up.m_z * spreadY );

            direction.Normalize();

            QAngle result;
            Math::VectorAngles( direction , result );
            return result;
        }

        inline float AngularError( const QAngle& a , const QAngle& b )
        {
            float dp = a.m_x - b.m_x;
            float dy = a.m_y - b.m_y;
            while ( dy > 180.f ) dy -= 360.f;
            while ( dy < -180.f ) dy += 360.f;
            return sqrtf( dp * dp + dy * dy );
        }

        inline bool GetWeaponSpreadData( C_CSWeaponBaseGun* weapon , float& inaccuracy , float& spread , int& itemIndex , int& weaponMode , float& recoilIndex )
        {
            if ( !weapon )
                return false;

            inaccuracy = C_CSWeaponBaseGun_GetInaccuracy( weapon );
            spread = C_CSWeaponBaseGun_GetSpread( weapon );
            recoilIndex = weapon->m_flRecoilIndex();
            weaponMode = weapon->m_bBurstMode() ? 1 : 0;

            itemIndex = GetCL_Weapons()->GetLocalWeaponDefinitionIndex();
            if ( itemIndex < 0 )
                itemIndex = 0;

            return spread >= 0.f && inaccuracy >= 0.f;
        }
    }

    inline bool CompensateAngles( const QAngle& desired , CUserCmd* cmd , QAngle& outShootAngles )
    {
        if ( !config.enabled || !cmd )
            return false;

        auto* weapon = GetCL_Weapons()->GetLocalActiveWeapon();
        float inaccuracy = 0.f;
        float spread = 0.f;
        int itemIndex = 0;
        int weaponMode = 0;
        float recoilIndex = 0.f;

        if ( !detail::GetWeaponSpreadData( weapon , inaccuracy , spread , itemIndex , weaponMode , recoilIndex ) )
            return false;

        if ( inaccuracy <= 0.0001f && spread <= 0.0001f )
        {
            outShootAngles = desired;
            return true;
        }

        const int renderTick = detail::GetRenderTick( cmd );
        const float totalSpread = inaccuracy + spread;
        const float searchRange = totalSpread * ( 180.f / PI ) * 3.5f + 1.5f;
        const float tolerance = 0.08f + totalSpread * ( 180.f / PI ) * 0.35f;

        QAngle bestCandidate = desired;
        float bestError = FLT_MAX;
        bool found = false;

        constexpr int kGrid = 24;
        for ( int py = 0; py < kGrid; ++py )
        {
            for ( int px = 0; px < kGrid; ++px )
            {
                QAngle candidate(
                    desired.m_x + ( ( px - kGrid / 2 ) / static_cast<float>( kGrid / 2 ) ) * searchRange ,
                    desired.m_y + ( ( py - kGrid / 2 ) / static_cast<float>( kGrid / 2 ) ) * searchRange ,
                    0.f );

                Math::NormalizeAngles( candidate );
                Math::ClampAngles( candidate );

                const uint32_t seed = detail::ComputeRandomSeed( candidate , renderTick );
                float spreadX = 0.f;
                float spreadY = 0.f;
                detail::CalcSpreadValues( static_cast<int>( seed ) , inaccuracy , spread , itemIndex , weaponMode , recoilIndex , spreadX , spreadY );

                const QAngle result = detail::ApplySpread( candidate , spreadX , spreadY );
                const float error = detail::AngularError( result , desired );
                if ( error < bestError )
                {
                    bestError = error;
                    bestCandidate = candidate;
                    found = true;
                    if ( error <= tolerance )
                        break;
                }
            }

            if ( found && bestError <= tolerance )
                break;
        }

        if ( !found )
            return false;

        outShootAngles = bestCandidate;
        Math::NormalizeAngles( outShootAngles );
        Math::ClampAngles( outShootAngles );
        return bestError <= tolerance * 2.5f;
    }
}
