#pragma once

#include <Common/Common.hpp>

#include <cstddef>
#include <vector>

#include <CS2/SDK/Types/CHandle.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

struct CachedEntity_t
{
    enum Type
    {
        UNKNOWN = 0 ,
        PLAYER_CONTROLLER ,
        BASE_WEAPON ,
        PLANTED_C4 ,
        GRENADE_PROJECTILE
    };

    CHandle m_Handle = { INVALID_EHANDLE_INDEX };
    Type m_Type = UNKNOWN;

    Rect_t m_Bbox = { 0.f , 0.f , 0.f , 0.f };

    bool m_bDraw = false;
    bool m_bVisible = false;
};

class IEntityCache
{
public:
    virtual void OnAddEntity( CEntityInstance* pInst , CHandle handle ) = 0;
    virtual void OnRemoveEntity( CEntityInstance* pInst , CHandle handle ) = 0;
};

class CEntityCache final : public IEntityCache
{
public:
    using CachedEntityVec_t = std::vector<CachedEntity_t>;
    using Lock_t = std::recursive_mutex;
    using LifeCallbackFn = void ( * )( CHandle handle , bool added );

public:
    virtual void OnAddEntity( CEntityInstance* pInst , CHandle handle ) override;
    virtual void OnRemoveEntity( CEntityInstance* pInst , CHandle handle ) override;

public:
    auto GetEntityType( C_BaseEntity* pBaseEntity ) -> CachedEntity_t::Type;

    auto RegisterLifeCallback( LifeCallbackFn callback ) noexcept -> bool;

public:
    inline auto GetCachedEntity() -> CachedEntityVec_t*
    {
        return &m_CachedEntity;
    }

    inline auto GetLock() -> Lock_t&
    {
        return m_Lock;
    }

private:
    static constexpr std::size_t kMaxLifeCallbacks = 8;

    void DispatchLifeCallbacks( CHandle handle , bool added ) noexcept;

    CachedEntityVec_t m_CachedEntity;
    Lock_t m_Lock;
    LifeCallbackFn m_lifeCallbacks[kMaxLifeCallbacks]{};
    std::size_t m_lifeCallbackCount = 0;
};

auto GetEntityCache() -> CEntityCache*;
