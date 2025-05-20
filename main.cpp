//Whoever reads this is smelly
//C+ = smelly
//erm its c++ akshully
#include <cpr/cpr.h>
#include <SDL.h>
#include <iostream>
using namespace std;

//Function Decloration
void controllerInputs(double & xAxis, double& yAxis, double& rightTrigger, bool& handbrake);


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
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        // Get joystick positions
        int leftX = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_LEFTX);   //For steering
        int rightTrigger = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);  //Fpr throttle

        std::cout << "Throttle: " << rightTrigger << " | Steering: " << leftX << "\r";

        SDL_Delay(50);
    }

    SDL_GameControllerClose(bunga);
    SDL_Quit();
    return 0;
}   

//This functions returns all of the important information from the controller
void controllerInputs(double& xAxis, double& yAxis, double& rightTrigger, bool& handbrake){
    





    return;
}