#define SDL_MAIN_HANDLED
#include <pigpio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstdint>
using namespace std;

int main() {
    if (gpioInitialise() < 0) {
        cerr << "Failed to initialize pigpio." << endl;
        return 1;
    }

    const int throtPin = 18;
    const int steerPin = 19;

    gpioSetPWMfrequency(throtPin, 1000);  // You can adjust frequency if needed
    gpioSetPWMfrequency(steerPin, 1000);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{AF_INET, htons(8000), INADDR_ANY};

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    cout << "Waiting for connection..." << endl;
    int client_fd = accept(server_fd, nullptr, nullptr);
    cout << "Client connected!" << endl;

    uint8_t buffer[2];

    while (read(client_fd, buffer, 2) == 2) {
        uint8_t steerVal = buffer[0];
        uint8_t throtVal = buffer[1];

        gpioPWM(steerPin, steerVal);
        gpioPWM(throtPin, throtVal);

        cout << "Steering: " << static_cast<int>(steerVal)
             << " | Throttle: " << static_cast<int>(throtVal) << endl;
    }

    gpioPWM(steerPin, 0);
    gpioPWM(throtPin, 0);

    close(client_fd);
    close(server_fd);
    gpioTerminate();
    return 0;
}
