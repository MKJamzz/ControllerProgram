#define SDL_MAIN_HANDLED   
#include <pigpio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstdint>
using namespace std;

//Function decloration

int main(){
    gpioInitialise();

    //PWM PINS
    const int throtPin = 18;
    const int steerPin = 19;

    gpioSetPWMfrequency(throtPin, 1000);
    gpioSetPWMfrequency(steerPin, 1000);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{AF_INET, htons(8000), INADDR_ANY};

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    cout << "Waiting for connection" << endl;
    int client_fd = accept(server_fd, nullptr, nullptr);
    cout << "Connected!?!?" << endl;

    uint8_t buffer[2];

    while(read(client_fd, buffer, 2) >= 2){
        gpioPWM(steerPin, buffer[0]);
        gpioPWM(throtPin, buffer[1]);
    }

    //Turning pins off
    gpioPWM(steerPin, 0);
    gpioPWM(throtPin, 0);

    close(client_fd);
    close(server_fd);
    return 0;
}