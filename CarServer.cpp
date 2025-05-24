#define SDL_MAIN_HANDLED   
#include <cpr/cpr.h>
#include <SDL.h>
#include <arpack-ng/inet.h>
#include <gpiod.h>
#include <iostream>
using namespace std;

//Function decloration
void manageOutputs(int xAxis, int yAxis, int throttle,
                   gpiod_line* up, gpiod_line* down, gpiod_line* left,
                   gpiod_line* right, gpiod_line* throt1, gpiod_line* throt2);

int main(){

    gpiod_chip* chip = gpiod_chip_open_by_name("gpiochip0");
    gpiod_line* left = gpiod_chip_get_line(chip,22);
    gpiod_line* right = gpiod_chip_get_line(chip,23);
    gpiod_line* throt1 = gpiod_chip_get_line(chip,5);
    gpiod_line_request_output(left, "controller", 0);
    gpiod_line_request_output(right, "controller", 0);
    gpiod_line_request_output(throt1, "controller", 0);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address(AF_INET, htons(8000), INADDR_ANY);

    bind(server_ffd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    cout << "Waiting for connection" << endl;
    int client_fd = accept(server_fd, nullptr, nullptr);
    cout << "Connected!?!?" << endl;

    char buffer[2];
    while(read(clinet_fd, buffer, 1) > 0){
        if(buffer[0] == '1') gpiod_line_set_value(line,1);
        else if(buffer[0] == '0') gpiod_line_set_vaule(line, 0);
    }


    //GPIO Cleanup
    gpiod_line_release(left);
    gpiod_line_release(right);
    gpiod_line_release(throt1);
    gpiod_chip_close(chip);

    SDL_GameControllerClose(bunga);
    SDL_Quit();

    close(client_fd);
    close(server_fd);
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