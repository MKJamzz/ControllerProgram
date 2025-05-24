// This program sends the data from the laptop like the controller inputs over to the 
// server that is being run by the Raspberry PI in order to achieve the 
// desired GPIO outputs 

#include <SDL.h>
#include <cpr/cpr.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

//Function decloration
void manageOutputs(int xAxis, int yAxis, int throttle,
                   gpiod_line* up, gpiod_line* down, gpiod_line* left,
                   gpiod_line* right, gpiod_line* throt1, gpiod_line* throt2);

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

    int sock = socket(AF_NET, SOCK_STREAM, 0);
    sockaddr_in server_adder{AF_INET, htons(8000)};
    inet_pton(AF_NET, "192.168.5.72", &server_addr.sin_addr);

    connet(sock, (sockaddr*)&server_addr, sizeof(server_addr));




    SDL_Event e;
    bool running = true;

    //Variable initialization
    double leftX = 0;
    double leftY = 0;
    double rightTrigger = 0;

    //GPIO Initialization -- Make better later
    gpiod_chip* chip = gpiod_chip_open_by_name("gpiochip0");
    gpiod_line* left = gpiod_chip_get_line(chip,22);
    gpiod_line* right = gpiod_chip_get_line(chip,23);
    gpiod_line* throt1 = gpiod_chip_get_line(chip,5);
    gpiod_line_request_output(left, "controller", 0);
    gpiod_line_request_output(right, "controller", 0);
    gpiod_line_request_output(throt1, "controller", 0);


    //GPIO Cleanup
    gpiod_line_release(left);
    gpiod_line_release(right);
    gpiod_line_release(throt1);
    gpiod_chip_close(chip);

    SDL_GameControllerClose(bunga);
    SDL_Quit();
    return 0;
}


//Helper function that takes all of the variable values and changes them as requested
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