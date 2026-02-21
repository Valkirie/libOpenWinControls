#include "OpenWinControlsCli.h"

#include <memory>

// IMPORTANT: lib headers rely on these but don't always include them themselves.
// Including them here prevents the cascade of parse errors.
#include <string>
#include <tuple>
#include <utility>

#include <controller/Controller.h>
#include <controller/ControllerV1.h>
#include <controller/ControllerV2.h>

namespace
{
    using CtrlPtr = std::unique_ptr<OWC::Controller>;
}

namespace OpenWinControlsCli
{
    GpdController::GpdController()
        : _native(System::IntPtr::Zero)
    {
    }

    GpdController::~GpdController()
    {
        // deterministic cleanup
        this->!GpdController();
    }

    GpdController::!GpdController()
    {
        if (_native != System::IntPtr::Zero)
        {
            auto holder = reinterpret_cast<CtrlPtr*>(_native.ToPointer());
            delete holder;
            _native = System::IntPtr::Zero;
        }
    }

    bool GpdController::Init()
    {
        // Allocate a holder for a polymorphic controller instance (V1 or V2)
        auto holder = new CtrlPtr();

        // Try V2 first (newer devices), then fall back to V1
        {
            auto v2 = std::make_unique<OWC::ControllerV2>();
            if (v2->init())
            {
                *holder = std::move(v2);
                _native = System::IntPtr(holder);
                return true;
            }
        }

        {
            auto v1 = std::make_unique<OWC::ControllerV1>();
            if (v1->init())
            {
                *holder = std::move(v1);
                _native = System::IntPtr(holder);
                return true;
            }
        }

        delete holder;
        return false;
    }

    bool GpdController::ReadConfig()
    {
        if (_native == System::IntPtr::Zero)
            return false;

        auto& ctrl = *reinterpret_cast<CtrlPtr*>(_native.ToPointer());
        return ctrl->readConfig();
    }

    bool GpdController::WriteConfig()
    {
        if (_native == System::IntPtr::Zero)
            return false;

        auto& ctrl = *reinterpret_cast<CtrlPtr*>(_native.ToPointer());
        return ctrl->writeConfig();
    }

    void GpdController::SetLedMode(OWC::LedMode mode)
    {
        if (_native == System::IntPtr::Zero)
            return;

        auto& ctrl = *reinterpret_cast<CtrlPtr*>(_native.ToPointer());
        ctrl->setLedMode(mode);
    }

    void GpdController::SetLedColor(int r, int g, int b)
    {
        if (_native == System::IntPtr::Zero)
            return;

        auto& ctrl = *reinterpret_cast<CtrlPtr*>(_native.ToPointer());
        ctrl->setLedColor(r, g, b);
    }

    void GpdController::SetRumbleMode(OWC::RumbleMode mode)
    {
        if (_native == System::IntPtr::Zero)
            return;

        auto& ctrl = *reinterpret_cast<CtrlPtr*>(_native.ToPointer());
        ctrl->setRumble(mode);
    }
}