// main.cpp — exo1 : la fenêtre nue
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKTime/NkClock.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

static void ConfigureAppData(NkAppData &d)
{
    d.appName = "FenetreNue";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state)
{
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "Ma fenetre nue";
    cfg.width = 800;
    cfg.height = 600;

    NkWindow window(cfg);
    if (!window.IsValid())
    {
        logger.Error("Creation de la fenetre impossible");
        return 1;
    }

    bool running = true;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *)
                                                { running = false; });

    while (running && window.IsOpen())
    {
        events.PollEvents();
        NkClock::Sleep((int64)10);
    }

    window.Close();
    return 0;
}