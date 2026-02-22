#include "OpenWinControlsCli.h"

#include <memory>

// IMPORTANT: lib headers rely on these but aren't fully self-contained in all TUs.
#include <string>
#include <tuple>
#include <utility>

#include <msclr/marshal_cppstd.h>

#include <controller/Controller.h>
#include <controller/ControllerV1.h>
#include <controller/ControllerV2.h>

#include <include/Button.h>
#include <include/LedMode.h>
#include <include/RumbleMode.h>
#include <include/BackButtonMode.h>
#include <include/EmulationMode.h>

namespace
{
    using CtrlPtr = std::unique_ptr<OWC::Controller>;

    static OWC::Controller* GetNativeController(System::IntPtr p)
    {
        if (p == System::IntPtr::Zero)
            return nullptr;

        auto holder = reinterpret_cast<CtrlPtr*>(p.ToPointer());
        return holder->get();
    }

    static OWC::ControllerV1* AsV1(OWC::Controller* c)
    {
        return dynamic_cast<OWC::ControllerV1*>(c);
    }

    static OWC::ControllerV2* AsV2(OWC::Controller* c)
    {
        return dynamic_cast<OWC::ControllerV2*>(c);
    }
}

namespace OpenWinControlsCli
{
    GpdController::GpdController()
        : _native(System::IntPtr::Zero)
    {
    }

    GpdController::~GpdController()
    {
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

    bool GpdController::IsInitialized()
    {
        return _native != System::IntPtr::Zero && GetNativeController(_native) != nullptr;
    }

    void GpdController::EnsureInit()
    {
        if (!IsInitialized())
            throw gcnew System::InvalidOperationException("Controller not initialized. Call InitAuto() first.");
    }

    void GpdController::EnsureV1()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        if (!AsV1(c))
            throw gcnew System::InvalidOperationException("This call is only valid for ControllerV1 devices.");
    }

    void GpdController::EnsureV2()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        if (!AsV2(c))
            throw gcnew System::InvalidOperationException("This call is only valid for ControllerV2 devices.");
    }

    bool GpdController::InitAuto()
    {
        // allocate holder (unique_ptr) and try init V2 then V1
        auto holder = new CtrlPtr();

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
        _native = System::IntPtr::Zero;
        return false;
    }

    int GpdController::GetControllerType()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return c->getControllerType();
    }

    System::ValueTuple<int, int> GpdController::GetV2Version()
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        auto ver = v2->getVersion(); // pair<int,int>
        return System::ValueTuple<int, int>(ver.first, ver.second);
    }

    System::ValueTuple<int, int> GpdController::GetV1XVersion()
    {
        EnsureV1();
        auto v1 = AsV1(GetNativeController(_native));
        auto ver = v1->getXVersion();
        return System::ValueTuple<int, int>(ver.first, ver.second);
    }

    System::ValueTuple<int, int> GpdController::GetV1KVersion()
    {
        EnsureV1();
        auto v1 = AsV1(GetNativeController(_native));
        auto ver = v1->getKVersion();
        return System::ValueTuple<int, int>(ver.first, ver.second);
    }

    bool GpdController::ReadConfig()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return c->readConfig();
    }

    bool GpdController::WriteConfig()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return c->writeConfig();
    }

    bool GpdController::SetButton(int btn, System::String^ key)
    {
        EnsureInit();
        auto c = GetNativeController(_native);

        auto k = msclr::interop::marshal_as<std::string>(key);
        return c->setButton(static_cast<OWC::Button>(btn), k);
    }

    System::String^ GpdController::GetButton(int btn)
    {
        EnsureInit();
        auto c = GetNativeController(_native);

        auto k = c->getButton(static_cast<OWC::Button>(btn));
        return gcnew System::String(k.c_str());
    }

    bool GpdController::SetBackButton(int num, int slot, System::String^ key)
    {
        EnsureInit();
        auto c = GetNativeController(_native);

        auto k = msclr::interop::marshal_as<std::string>(key);
        return c->setBackButton(num, slot, k);
    }

    System::String^ GpdController::GetBackButton(int num, int slot)
    {
        EnsureInit();
        auto c = GetNativeController(_native);

        auto k = c->getBackButton(num, slot);
        return gcnew System::String(k.c_str());
    }

    void GpdController::SetBackButtonStartTime(int num, int slot, int timeMs)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        c->setBackButtonStartTime(num, slot, timeMs);
    }

    int GpdController::GetBackButtonStartTime(int num, int slot)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return c->getBackButtonStartTime(num, slot);
    }

    void GpdController::SetBackButtonMode(int num, int mode)
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        v2->setBackButtonMode(num, static_cast<OWC::BackButtonMode>(mode));
    }

    int GpdController::GetBackButtonMode(int num)
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        return static_cast<int>(v2->getBackButtonMode(num));
    }

    void GpdController::SetBackButtonActiveSlots(int num, int count)
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        v2->setBackButtonActiveSlots(num, static_cast<uint8_t>(count));
    }

    int GpdController::GetBackButtonActiveSlots(int num)
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        return v2->getBackButtonActiveSlots(num);
    }

    void GpdController::SetBackButtonHoldTime(int num, int slot, int timeMs)
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        v2->setBackButtonHoldTime(num, slot, timeMs);
    }

    int GpdController::GetBackButtonHoldTime(int num, int slot)
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        return v2->getBackButtonHoldTime(num, slot);
    }

    void GpdController::SetRumbleMode(int mode)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        c->setRumble(static_cast<OWC::RumbleMode>(mode));
    }

    int GpdController::GetRumbleMode()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return static_cast<int>(c->getRumbleMode());
    }

    void GpdController::SetLedMode(int mode)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        c->setLedMode(static_cast<OWC::LedMode>(mode));
    }

    int GpdController::GetLedMode()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return static_cast<int>(c->getLedMode());
    }

    void GpdController::SetLedColor(int r, int g, int b)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        c->setLedColor(r, g, b);
    }

    System::ValueTuple<int, int, int> GpdController::GetLedColor()
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        auto t = c->getLedColor();
        return System::ValueTuple<int, int, int>(std::get<0>(t), std::get<1>(t), std::get<2>(t));
    }

    void GpdController::SetAnalogCenter(int center, bool left)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        c->setAnalogCenter(center, left);
    }

    int GpdController::GetAnalogCenter(bool left)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return c->getAnalogCenter(left);
    }

    void GpdController::SetAnalogBoundary(int boundary, bool left)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        c->setAnalogBoundary(boundary, left);
    }

    int GpdController::GetAnalogBoundary(bool left)
    {
        EnsureInit();
        auto c = GetNativeController(_native);
        return c->getAnalogBoundary(left);
    }

    int GpdController::GetEmulationMode()
    {
        EnsureV2();
        auto v2 = AsV2(GetNativeController(_native));
        return static_cast<int>(v2->getEmulationMode());
    }
}