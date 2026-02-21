#pragma once

// Keep these includes if you want to expose OWC enums directly to C#
// (Requires those headers to be reachable via Additional Include Directories)
#include <LedMode.h>
#include <RumbleMode.h>

namespace OpenWinControlsCli
{
    public ref class GpdController sealed
    {
    public:
        GpdController();
        ~GpdController();   // IDisposable (deterministic cleanup)
        !GpdController();   // finalizer (GC cleanup)

        bool Init();        // tries V2 then V1
        bool ReadConfig();
        bool WriteConfig();

        void SetLedMode(OWC::LedMode mode);
        void SetLedColor(int r, int g, int b);
        void SetRumbleMode(OWC::RumbleMode mode);

    private:
        System::IntPtr _native; // stores CtrlPtr* (allocated with new)
    };
}