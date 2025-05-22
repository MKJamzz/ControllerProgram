//Whoever reads this is smelly
//C+ = smelly
//erm its c++ akshully

//Run this line in terminal to test the build
// ./build/RCCarController.exe

#define SDL_MAIN_HANDLED    //BRO THIS WAS THE ONLY LINE THAT I NEEDED
#include <cpr/cpr.h>
#include <SDL.h>
#include <iostream>
using namespace std;

//Function Decloration
int main();
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

        //std::cout << "Throttle: " << rightTrigger << " | Steering: " << leftX << "\r";
        std::cout << "Throttle: " << rightTrigger << " | Steering: " << leftX << endl;
        // Get joystick positions

        Sint16 rawLeftX = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_LEFTX);
        leftX = rawLeftX /32767.0;

        //Implements the deadzones
        if (leftX <= 0.012 && leftX >= -0.012){
            leftX = 0.000000;
        }
        if(leftX<-1){    //Here so it turning left doesn't turn faster than it turning right
            leftX = -1;
        }


        Sint16 rawThrottle = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        rightTrigger = rawThrottle / 32767.0;
        
        std::cout << "Throttle: " << rightTrigger << " | Steering: " << leftX << "\r";
        

        SDL_Delay(50);
    }

    SDL_GameControllerClose(bunga);
    SDL_Quit();

    return 0;
}   