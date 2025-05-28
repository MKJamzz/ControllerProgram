//Whoever reads this is smelly
//C+ = smelly
//erm its c++ akshully

//Run this line in terminal to test the build
// ./build/ControllerProgram.exe

#define SDL_MAIN_HANDLED    //BRO THIS WAS THE ONLY LINE THAT I NEEDED
#include <cpr/cpr.h>
#include <SDL.h>
#include <gpiod.h>
#include <iostream>
using namespace std;

//Function Decloration
int main();
void manageOutputs(int xAxis, int yAxis, int throttle,
                   gpiod_line* up, gpiod_line* down, gpiod_line* left,
                   gpiod_line* right, gpiod_line* throt1, gpiod_line* throt2);
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

    //Variable initialization
    double leftX = 0;
    double leftY = 0;
    double rightTrigger = 0;

    //GPIO Initialization -- Make better later
    gpiod_chip* chip = gpiod_chip_open_by_name("gpiochip0");
    gpiod_line* up = gpiod_chip_get_line(chip,17);
    gpiod_line* down = gpiod_chip_get_line(chip,27);
    gpiod_line* left = gpiod_chip_get_line(chip,22);
    gpiod_line* right = gpiod_chip_get_line(chip,23);
    gpiod_line* throt1 = gpiod_chip_get_line(chip,5);
    gpiod_line* throt2 = gpiod_chip_get_line(chip,6);
    gpiod_line_request_output(up, "controller", 0);
    gpiod_line_request_output(down, "controller", 0);
    gpiod_line_request_output(left, "controller", 0);
    gpiod_line_request_output(right, "controller", 0);
    gpiod_line_request_output(throt1, "controller", 0);
    gpiod_line_request_output(throt2, "controller", 0);




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

        Sint16 rawLeftY = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_LEFTY);
        leftY = rawLeftY /32767.0;

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
        
        manageOutputs(leftX, leftY, rightTrigger, up, down, left, right, throt1, throt2);

        SDL_Delay(50);
    }

    SDL_GameControllerClose(bunga);
    SDL_Quit();

    //GPIO Cleanup
    gpiod_line_release(up);
    gpiod_line_release(down);
    gpiod_line_release(left);
    gpiod_line_release(right);
    gpiod_line_release(throt1);
    gpiod_line_release(throt2);
    gpiod_chip_close(chip);

    return 0;
}   


void manageOutputs(int xAxis, int yAxis, int throttle,
                   gpiod_line* up, gpiod_line* down, gpiod_line* left,
                   gpiod_line* right, gpiod_line* throt1, gpiod_line* throt2) {
    if(xAxis > 0.75) gpiod_line_set_value(right, 1);
    else gpiod_line_set_value(right, 0);

    if(xAxis < -0.75) gpiod_line_set_value(left, 1);
    else gpiod_line_set_value(left, 0);

    if(yAxis > 0.75) gpiod_line_set_value(up, 1);
    else gpiod_line_set_value(up, 0);

    if(yAxis < -0.75) gpiod_line_set_value(down, 1);
    else gpiod_line_set_value(down, 0);

    if(throttle > 0.75) gpiod_line_set_value(throt2, 1);
    else gpiod_line_set_value(throt2, 0);

    if(throttle < 0.25) gpiod_line_set_value(throt1, 1);
    else gpiod_line_set_value(throt1, 0);

    return;
}
