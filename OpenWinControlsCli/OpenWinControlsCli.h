#pragma once

namespace OpenWinControlsCli
{
    public ref class GpdController sealed
    {
    public:
        GpdController();
        ~GpdController();
        !GpdController();

        // Init / identity
        bool InitAuto();                 // tries V2 then V1
        bool IsInitialized();
        int  GetControllerType();        // 1 or 2

        // Versions (V1/V2 specific)
        System::ValueTuple<int, int> GetV2Version();     // throws if not V2
        System::ValueTuple<int, int> GetV1XVersion();    // throws if not V1
        System::ValueTuple<int, int> GetV1KVersion();    // throws if not V1

        // Config I/O
        bool ReadConfig();
        bool WriteConfig();

        // Buttons (string key mapping, as per native API)
        bool SetButton(int btn, System::String^ key);
        System::String^ GetButton(int btn);

        // Back buttons (base + V2 extensions)
        bool SetBackButton(int num, int slot, System::String^ key);
        System::String^ GetBackButton(int num, int slot);

        void SetBackButtonStartTime(int num, int slot, int timeMs);
        int  GetBackButtonStartTime(int num, int slot);

        // V2 only:
        void SetBackButtonMode(int num, int mode);           // BackButtonMode as int
        int  GetBackButtonMode(int num);

        void SetBackButtonActiveSlots(int num, int count);
        int  GetBackButtonActiveSlots(int num);

        void SetBackButtonHoldTime(int num, int slot, int timeMs);
        int  GetBackButtonHoldTime(int num, int slot);

        // Rumble / LED
        void SetRumbleMode(int mode);                        // RumbleMode as int
        int  GetRumbleMode();

        void SetLedMode(int mode);                           // LedMode as int
        int  GetLedMode();
        void SetLedColor(int r, int g, int b);
        System::ValueTuple<int, int, int> GetLedColor();

        // Analog
        void SetAnalogCenter(int center, bool left);
        int  GetAnalogCenter(bool left);
        void SetAnalogBoundary(int boundary, bool left);
        int  GetAnalogBoundary(bool left);

        // V2 only:
        int  GetEmulationMode();                             // EmulationMode as int

    private:
        System::IntPtr _native; // CtrlPtr* (std::unique_ptr<OWC::Controller>)

        // Internal helpers
        void EnsureInit();
        void EnsureV1();
        void EnsureV2();
    };
}