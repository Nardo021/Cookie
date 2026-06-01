#pragma once

#include <array>
#include <cstdint>

#include <CS2/SDK/Math/Vector3.hpp>

namespace AutoWallTrace
{
	inline constexpr uint64_t kPenMask = 0x1C300B;

	struct TraceArrElement
	{
		std::uint8_t pad[0x30]{};
	};

	struct UpdateValue
	{
		float previousLength = 0.f;
		float currentLength = 0.f;
		std::uint8_t pad0[0x8]{};
		std::int16_t handleIdx = 0;
		std::uint8_t pad1[0x6]{};
	};

	struct TraceFilter
	{
		std::uint8_t storage[0x40]{};
	};

	struct HitboxData
	{
		std::uint8_t pad[0x38]{};
		int hitGroup = 0;
	};

	struct GameTrace
	{
		void* surface = nullptr;
		void* hitEntity = nullptr;
		HitboxData* hitboxData = nullptr;
		std::uint8_t pad0[0x38]{};
		std::uint32_t contents = 0;
		std::uint8_t pad1[0x24]{};
		Vector3 startPos{};
		Vector3 endPos{};
		Vector3 normal{};
		Vector3 position{};
		std::uint8_t pad2[0x4]{};
		float fraction = 1.f;
		std::uint8_t pad3[0x6]{};
		bool allSolid = false;
		std::uint8_t pad4[0x51]{};
	};

	struct TraceData
	{
		std::int32_t uk1 = 0;
		float uk2 = 52.f;
		void* arrPointer = nullptr;
		std::int32_t uk3 = 128;
		std::int32_t uk4 = static_cast<std::int32_t>( 0x80000000 );
		std::array<TraceArrElement , 0x80> arr{};
		std::uint8_t pad0[0x8]{};
		std::int64_t numUpdate = 0;
		void* pointerUpdateValue = nullptr;
		std::uint8_t pad1[0xC8]{};
		Vector3 start{};
		Vector3 end{};
		std::uint8_t pad2[0x50]{};
	};

	struct HandleBulletData
	{
		float damage = 0.f;
		float penetration = 0.f;
		float rangeModifier = 0.f;
		float range = 0.f;
		int penCount = 0;
		bool failed = false;

		HandleBulletData(
			float dmg ,
			float pen ,
			float rangeMod ,
			float weaponRange ,
			int maxPen ,
			bool fail ) :
			damage( dmg ) ,
			penetration( pen ) ,
			rangeModifier( rangeMod ) ,
			range( weaponRange ) ,
			penCount( maxPen ) ,
			failed( fail )
		{
		}
	};

	using CreateTraceFn = void ( __fastcall* )(
		TraceData* trace ,
		Vector3 start ,
		Vector3 end ,
		TraceFilter filter ,
		int penetrationCount );

	using InitTraceInfoFn = void ( __fastcall* )( GameTrace* hit );

	using GetTraceInfoFn = void ( __fastcall* )(
		TraceData* trace ,
		GameTrace* hit ,
		float unknownFloat ,
		void* unknown );

	using HandleBulletPenetrationFn = bool ( __fastcall* )(
		TraceData* trace ,
		HandleBulletData* stats ,
		UpdateValue* modValue ,
		void* a4 ,
		void* a5 ,
		void* a6 ,
		void* a7 ,
		void* a8 ,
		bool drawShowImpacts );

	using InitTraceFilterFn = void ( __fastcall* )(
		TraceFilter* filter ,
		void* skipEntity ,
		std::uint64_t mask ,
		std::uint8_t layer ,
		std::uint16_t uk );

}
