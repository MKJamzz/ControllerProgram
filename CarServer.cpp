#define SDL_MAIN_HANDLED   
#include <pigpio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstdint>
using namespace std;

int main() {
    gpioInitialise();

    // PWM pins
    const int throtPin = 18;
    const int steerPin = 19;
    const int servoPin = 13;

    gpioSetPWMfrequency(throtPin, 1000);
    gpioSetPWMfrequency(steerPin, 1000);
    //gpioSetPWMfrequency(servoPin, 50); // aparently thats the normal hz

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{AF_INET, htons(8000), INADDR_ANY};
    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    cout << "Waiting for connection" << endl;
    int client_fd = accept(server_fd, nullptr, nullptr);
    cout << "Connected!" << endl;

    uint8_t buffer[5];

    while (read(client_fd, buffer, 5) == 5) {
        gpioPWM(steerPin, buffer[0]);
        gpioPWM(throtPin, buffer[1]);

        // Servo pulse width mapping
        int pulseWidth = 500 + ((int)buffer[4] * 2000 / 255);
        gpioServo(servoPin, pulseWidth);

        if (buffer[2] == 1 || buffer[3] == 1) {
            std::cout << "\n[INFO] Stop signal received\n";
            break;
        }
    }
     //Turning pins off
    gpioPWM(steerPin, 0);
    gpioPWM(throtPin, 0);
    gpioServo(servoPin, 0); 

    close(client_fd);
    close(server_fd);
    return 0;
}
