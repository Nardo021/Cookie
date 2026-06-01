#include "CSigScan.hpp"
#include "SpreadHooks.hpp"

#include <Client/Game/Patterns.hpp>
#include <CS2/SDK/SDK.hpp>

namespace CookieCore
{
	namespace
	{
		CSigScan s_DrawObjectScan(
			"Sig::DrawObject" ,
			Patterns::sig_DrawObject ,
			SCENESYSTEM_DLL );

		CSigScan s_HandleBulletPenetrationScan(
			"Sig::HandleBulletPenetration" ,
			Patterns::sig_HandleBulletPenetration ,
			CLIENT_DLL );

		CSigScan s_GetSpreadScan(
			"Sig::GetSpread" ,
			Patterns::sig_GetSpread ,
			CLIENT_DLL );

		CSigScan s_GetInaccuracyScan(
			"Sig::GetInaccuracy" ,
			Patterns::sig_GetInaccuracy ,
			CLIENT_DLL );

		CSigScan s_PredictionSimulationScan(
			"Sig::PredictionSimulation" ,
			Patterns::sig_PredictionSimulation ,
			CLIENT_DLL );

		CSigScan s_CalculateWorldSpaceBonesScan(
			"Sig::CalculateWorldSpaceBones" ,
			Patterns::sig_CalculateWorldSpaceBones ,
			CLIENT_DLL );

		struct SpreadHookRegistrar
		{
			SpreadHookRegistrar()
			{
				SpreadHooks::Register( s_GetSpreadScan , s_GetInaccuracyScan );
			}
		};

		inline SpreadHookRegistrar s_spreadHookRegistrar{};
	}
}
