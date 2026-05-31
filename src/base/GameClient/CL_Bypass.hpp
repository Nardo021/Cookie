#pragma once

#include <Common/Common.hpp>

#include <string>
#include <vector>

#include <CS2/Protobuf/cs_usercmd.pb.h>

class QAngle;
class CCSGOInput;
class CUserCmd;

class CL_Bypass final
{
public:
	auto PreClientCreateMove( CUserCmd* pUserCmd ) -> void;
	auto PostClientCreateMove( CCSGOInput* pCCSGOInput , CUserCmd* pUserCmd ) -> void;

public:
	auto SetViewAngles( QAngle* ViewAngles , CCSGOInput* pInput , CUserCmd* pUserCmd , bool AddSetViewAngles = true , bool OnlyInputHistory = false ) -> void;
	auto SetAttack( CUserCmd* pUserCmd , bool AddSubTIck = false ) -> void;
	auto SetDontAttack( CUserCmd* pUserCmd , bool AddSubTIck = false ) -> void;
	auto SetButton( CUserCmd* pUserCmd , uint64_t button , bool pressed , bool addSubTick = false , float when = 0.99f ) -> void;
	auto ApplyJumpBug( CUserCmd* pUserCmd , float landFraction ) -> void;

public:
	auto AddProcessSubTick( const uint64_t Button , const bool Pressed ) -> void;
	auto AddSubTickMove( CUserCmd* pUserCmd , const uint64_t button , const bool pressed , const float when ) -> void;
	auto AddSubtickStrafeStep( CUserCmd* pUserCmd , float yawDelta , float pitchDelta = 0.f , float when = 1.f ) -> void;

public:
	auto OnCBaseUserCmdPB( CBaseUserCmdPB* pBaseUserCmdPB ) -> void;

private:
	struct PostProcessScope
	{
		explicit PostProcessScope( CL_Bypass& bypass ) : m_bypass( bypass ) { m_bypass.m_inPostProcess = true; }
		~PostProcessScope() { m_bypass.m_inPostProcess = false; }

		CL_Bypass& m_bypass;
	};

	auto CalculateWhenValue() -> float;
	auto AddSubtickMoveStep( CUserCmd* pUserCmd , const uint64_t button , const bool pressed , const float when ) -> void;
	auto SyncModifiedCmdToBackup( CUserCmd* pUserCmd ) -> void;
	auto SpoofCrc() -> std::string;

private:
	CBaseUserCmdPB m_Backup;
	bool m_backupReady = false;
	bool m_needsPostProcess = false;
	bool m_inPostProcess = false;

private:
	struct InternalSubTick_t
	{
		uint64_t m_Button = 0;
		bool m_Pressed = false;
		float m_flWhen = 0.f;
	};

private:
	std::vector<InternalSubTick_t> m_InternalSubTickList;
};

auto GetCL_Bypass() -> CL_Bypass*;
