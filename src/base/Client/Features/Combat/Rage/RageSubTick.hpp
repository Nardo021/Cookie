#pragma once

#include <array>
#include <cstdint>
#include <utility>
#include <vector>

#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

class CUserCmd;

namespace RageSubTick
{
	enum class CommandMsg : int
	{
		None = 0 ,
		RapidFire = 1 ,
		Silent = 3 ,
	};

	enum class ResponseMsg : int
	{
		Empty = 0 ,
		ValidatedViewAngles = 1 ,
	};

	struct State
	{
		QAngle     bestPoint{};
		Vector3    bestPointVec{};
		CommandMsg command = CommandMsg::None;
		ResponseMsg response = ResponseMsg::Empty;
		bool       useBacktrack = false;
		int        backtrackTick = 0;
		float      backtrackFraction = 0.f;

		auto Reset() noexcept -> void
		{
			bestPoint = {};
			bestPointVec = {};
			command = CommandMsg::None;
			response = ResponseMsg::Empty;
			useBacktrack = false;
			backtrackTick = 0;
			backtrackFraction = 0.f;
		}
	};

	inline State s_state{};
	inline std::vector<std::pair<float, float>> s_computedSeeds;

	auto BuildSeedTable() noexcept -> void;
	auto ResetTick() noexcept -> void;
	auto SetSilentAim( const QAngle& angles ) noexcept -> void;
	auto SetRapidFire( const QAngle& angles ) noexcept -> void;
	auto SetBacktrack( int tick , float fraction ) noexcept -> void;
	auto ApplyInputParser(
		CUserCmd* cmd ,
		void* inputMessage ,
		CSGOInputHistoryEntryPB* historyEntry ) noexcept -> void;

} // namespace RageSubTick
