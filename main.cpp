#include <k5/k5.h>

#include "Lorenz.h"

int main() {
    CEngine& game = CEngine::Instance();

    game.CreateWindow("mvis", 1600, 900, false);
    game.ChangeFrame(&Lorenz::Instance());

    while (game.Running())
        game.Tick();
    game.Cleanup();

    return 0;
}