// This program sends the data from the laptop like the controller inputs over to the 
// server that is being run by the Raspberry PI in order to achieve the 
// desired GPIO outputs 

#define SDL_MAIN_HANDLED   
#include <SDL.h>
#include <cpr/cpr.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <algorithm>
using namespace std;


int main(){

    //Connecting the gametroller on start up
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    //Creates the variable to the controller
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

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{AF_INET, htons(8000)};
    inet_pton(AF_INET, "192.168.5.72", &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    SDL_Event e;
    bool running = true;

    //Variable initialization
    double leftX = 0;
    double leftY = 0;
    double rightTrigger = 0;

    while(running){
        SDL_GameControllerUpdate();
        
        Sint16 rawLeftX = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_LEFTX);
        leftX = rawLeftX /32767.0;

        //Implements the deadzones
        if (leftX <= 0.013 && leftX >= -0.013) leftX = 0.0;
        leftX = std::clamp(leftX,-1.0,1.0);       

        Sint16 rawThrottle = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        rightTrigger = rawThrottle / 32767.0;
        rightTrigger = std::clamp(rightTrigger, 0.0, 1.0);

        uint8_t pwmLeftX = static_cast <uint8_t>((leftX + 1)/2 * 255); // Map -1 -> 1, into 0 -> 255
        uint8_t pwmTrig = static_cast <uint8_t>(rightTrigger * 255);

        //Combines both signals and sends them
        uint8_t buffer[2] = {pwmLeftX, pwmTrig};
        send(sock, reinterpret_cast<const char*>(buffer), 2, 0);

        cout << "Trigger Value: " << pwmTrig << " || Steering Value: " << pwmLeftX << endl;

        usleep(5000);
    }

    close(sock);
    SDL_GameControllerClose(bunga);
    SDL_Quit();
    return 0;
}
