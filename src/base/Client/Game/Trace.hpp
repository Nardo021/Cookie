#pragma once
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "Game.hpp"
#include "Offsets.hpp"
#include "Patterns.hpp"

// Visibility via client.dll TraceShape 鈥?same path the game uses for bullet traces.
namespace Trace
{
    struct Ray_t
    {
        Game::Vector3 start;
        Game::Vector3 end;
        Game::Vector3 mins;
        Game::Vector3 maxs;
    };

    // bool TraceShape(CGameTraceManager*, Ray_t*, Vector3*, Vector3*, TraceFilter_t*, GameTrace_t*)
    using TraceShapeFn = bool(__fastcall*)(
        void* traceMgr,
        Ray_t* ray,
        Game::Vector3* start,
        Game::Vector3* end,
        void* filter,
        void* trace);

    using InitTraceFilterFn = void*(__fastcall*)(
        void* filter,
        void* skipEntity,
        uint64_t mask,
        int layer,
        int16_t uk);

    inline TraceShapeFn fnTraceShape = nullptr;
    inline InitTraceFilterFn fnInitTraceFilter = nullptr;
    inline void** pTraceManager = nullptr;
    inline bool ready = false;

    inline constexpr uint64_t MASK_SHOT = 0x1C3003;
    inline constexpr float kVisibleFraction = 0.97f;
    inline constexpr size_t kFilterSize = 0x40;
    inline constexpr size_t kTraceSize = 0xB8;
    inline constexpr std::ptrdiff_t kTraceHitEntity = 0x90;
    inline constexpr std::ptrdiff_t kTraceFraction = 0xAC;

    inline uintptr_t ResolveClientPattern(const char* pattern, int relOffset = 3, int insnSize = 7)
    {
        uintptr_t insn = Game::FindPattern(L"client.dll", pattern);
        if (!insn)
            return 0;
        return Game::ResolveRelativeAddress(insn, relOffset, insnSize);
    }

    inline void SetupFilterManual(uint8_t* filter, uint32_t localPawnHandle)
    {
        std::memset(filter, 0, kFilterSize);
        *reinterpret_cast<uint64_t*>(filter + 0x8) = MASK_SHOT;
        *reinterpret_cast<int16_t*>(filter + 0x34) = 7;
        *reinterpret_cast<int32_t*>(filter + 0x20) = static_cast<int32_t>(localPawnHandle);
        filter[0x36] = 4;
    }

    inline void SetupFilter(uint8_t* filter, uintptr_t localPawn, uint32_t localPawnHandle)
    {
        std::memset(filter, 0, kFilterSize);

        if (fnInitTraceFilter && localPawn)
        {
            __try
            {
                fnInitTraceFilter(filter, reinterpret_cast<void*>(localPawn), MASK_SHOT, 4, 15);
                return;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                std::memset(filter, 0, kFilterSize);
            }
        }

        SetupFilterManual(filter, localPawnHandle);
    }

    inline bool Init()
    {
        if (ready)
            return true;

        if (!Game::clientBase)
            return false;

        uintptr_t traceShapeAddr = Game::FindPattern(L"client.dll", Patterns::sig_TraceShape);
        if (!traceShapeAddr)
        {
            printf("[Trace] FAIL: TraceShape pattern not found\n");
            return false;
        }
        fnTraceShape = reinterpret_cast<TraceShapeFn>(traceShapeAddr);

        uintptr_t mgrGlobal = ResolveClientPattern(Patterns::sig_TraceManager);
        if (!mgrGlobal)
            mgrGlobal = Game::FindRipTarget(traceShapeAddr, 0x120);

        if (!mgrGlobal)
        {
            printf("[Trace] FAIL: CGameTraceManager global not found\n");
            return false;
        }

        pTraceManager = reinterpret_cast<void**>(mgrGlobal);

        uintptr_t initFilterAddr = Game::FindPattern(L"client.dll", Patterns::sig_InitFilter);
        if (initFilterAddr)
            fnInitTraceFilter = reinterpret_cast<InitTraceFilterFn>(initFilterAddr);

        void* mgr = pTraceManager ? Game::Read<void*>(reinterpret_cast<uintptr_t>(pTraceManager)) : nullptr;
        ready = fnTraceShape && pTraceManager && mgr;

        printf("[Trace] TraceShape=0x%IX mgrGlobal=0x%IX mgr=0x%p initFilter=0x%IX ready=%d\n",
            traceShapeAddr, mgrGlobal, mgr, initFilterAddr, ready);

        return ready;
    }

    inline bool IsVisible(
        const Game::Vector3& eyePos,
        const Game::Vector3& targetPos,
        uintptr_t localPawn,
        uint32_t localPawnHandle,
        uintptr_t targetPawn)
    {
        if (!ready && !Init())
            return false;

        void* traceMgr = pTraceManager ? Game::Read<void*>(reinterpret_cast<uintptr_t>(pTraceManager)) : nullptr;
        if (!traceMgr || !fnTraceShape)
            return false;

        Ray_t ray{};
        alignas(16) uint8_t filter[kFilterSize]{};
        alignas(16) uint8_t traceResult[kTraceSize]{};

        SetupFilter(filter, localPawn, localPawnHandle);

        Game::Vector3 start = eyePos;
        Game::Vector3 end = targetPos;

        bool ok = false;
        __try
        {
            ok = fnTraceShape(traceMgr, &ray, &start, &end, filter, traceResult);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }

        if (!ok)
            return false;

        uintptr_t hitEntity = *reinterpret_cast<uintptr_t*>(traceResult + kTraceHitEntity);
        float fraction = *reinterpret_cast<float*>(traceResult + kTraceFraction);

        return hitEntity == targetPawn && fraction >= kVisibleFraction;
    }
}
