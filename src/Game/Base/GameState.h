#ifndef FINALDAYONEARTH_GAMESTATE_H
#define FINALDAYONEARTH_GAMESTATE_H


#include "glm/fwd.hpp"
#include <vector>

class Input;

class GameState {
public:
    virtual void run(double deltaTime);

    virtual void render(double deltaTime);

    virtual std::vector<Input *> getInputs();

    virtual void updateScreenSize(unsigned int width, unsigned int height);

protected:
    unsigned int SCR_WIDTH = -1;
    unsigned int SCR_HEIGHT = -1;
};


#endif //FINALDAYONEARTH_GAMESTATE_H
