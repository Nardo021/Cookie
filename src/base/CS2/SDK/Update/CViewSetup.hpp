#pragma once

#include <Common/MemoryEngine.hpp>

#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Update/Offsets.hpp>

class CViewSetup
{
public:
	CUSTOM_OFFSET_FIELD( Vector3 , m_vecOrigin , g_CViewSetup_vecOrigin );
	CUSTOM_OFFSET_FIELD( QAngle , m_angView , g_CViewSetup_angView );
};
