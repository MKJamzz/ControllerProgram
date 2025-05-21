//Whoever reads this is smelly
//C+ = smelly
//erm its c++ akshully

//Run this line in terminal to test the build
// ./build/RCCarController.exe

#include <cpr/cpr.h>
#include <SDL.h>
#include <iostream>
using namespace std;

//Function Decloration
void controllerInputs(SDL_GameController* contInp, double & xAxis,double& rightTrigger);

//Main function to run everything
int main(){

    //Checks if the gamecontroller is connected to the program, returns 1 if none are found
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    //Creates the variable for the gamecontroller
    SDL_GameController* bunga = nullptr;

    //Outputs the name of the controller
    if (SDL_IsGameController(0)) {
        bunga = SDL_GameControllerOpen(0);
        if (bunga) {
            std::cout << "PS5 Controller connected: " << SDL_GameControllerName(bunga) << std::endl;
        }
        else{
        std::cerr << "No PS5 controller detected.\n";
        SDL_Quit();
        return 1;
        }
    }

    SDL_Event e;
    bool running = true;

    double leftX = 0;
    double rightTrigger = 0;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        // Get joystick positions
        controllerInputs(bunga, leftX, rightTrigger);
        std::cout << "Throttle: " << rightTrigger << " | Steering: " << leftX << "\r";

        SDL_Delay(50);
    }

    SDL_GameControllerClose(bunga);
    SDL_Quit();

    return 0;
}   

//This functions returns all of the important information from the controller
void controllerInputs(SDL_GameController* contInp, double& xAxis, double& rightTrigger){
        Sint16 rawLeftX = SDL_GameControllerGetAxis(contInp, SDL_CONTROLLER_AXIS_LEFTX);
        xAxis = rawLeftX / 32768;

        Sint16 rawThrottle = SDL_GameControllerGetAxis(contInp, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        rightTrigger = rawThrottle / 32768;

    return;
}