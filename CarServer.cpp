#define SDL_MAIN_HANDLED   
#include <pigpio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstdint>
using namespace std;

float remap(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return toLow     + (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
}

int main() {
    gpioInitialise();

    // PWM pins
    const int stickPin = 12;
    const int RPWM = 18;
    const int LPWM = 19;
    const int L_EN = 23;
    const int R_EN = 24;

    //Sets the outputs of all of the GPIO pins
    gpioSetMode(RPWM, PI_OUTPUT);
    gpioSetMode(LPWM, PI_OUTPUT);
    gpioSetMode(R_EN, PI_OUTPUT);
    gpioSetMode(L_EN, PI_OUTPUT);


    //Enables both of the motors
    gpioWrite(R_EN, 1);
    gpioWrite(L_EN, 1);

    gpioSetPWMfrequency(RPWM, 1000);
    gpioSetPWMfrequency(LPWM, 1000);


    gpioPWM(RPWM, 0);
    gpioPWM(LPWM, 0);
    gpioWrite(R_EN, 0);
    gpioWrite(L_EN, 0);
    //gpioSetPWMfrequency(servoPin, 50); // aparently thats the normal hz

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{AF_INET, htons(8000), INADDR_ANY};
    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    cout << "Waiting for connection" << endl;
    int client_fd = accept(server_fd, nullptr, nullptr);
    cout << "Connected!" << endl;

    uint8_t buffer[6];

    while (read(client_fd, buffer, 6) == 6) {
        gpioPWM(stickPin, buffer[0]);

        // Servo pulse width mapping
       // int pulseWidth = 500 + ((int)buffer[4] * 2000 / 255);
    

        int input = buffer[4];
        int pulseWidth = remap(input, 0, 255, 1088, 1872);
        int reverseSpeed = remap(buffer[5], 0, 255, 0 , 127);   //moves the car backward
        int forwardSpeed = remap(buffer[1], 0, 255, 0 , 127);   //moves the car forward

        if (forwardSpeed > 10 && reverseSpeed <= 10) {
            gpioPWM(RPWM, forwardSpeed);  // Forward
            gpioPWM(LPWM, 0);
        } 
        else if (reverseSpeed > 10 && forwardSpeed <= 10) {
            gpioPWM(RPWM, 0);
            gpioPWM(LPWM, reverseSpeed);  // Reverse
        }
        else {
            gpioPWM(RPWM, 0);  // Stop
            gpioPWM(LPWM, 0);
        }


        gpioServo(stickPin, pulseWidth);



        if (buffer[2] == 1 || buffer[3] == 1) {
            std::cout << "\n[INFO] Stop signal received\n";
            break;
        }

        cout << "Trigger: " << (int)buffer[0] 
             << " | Steering: " << (int)buffer[1]
             << " | Servo: " << (int)pulseWidth
             << " | Circle: " << (int)buffer[2]
             << " | Ctrl+C: " << (int)buffer[3] << "\r";
    }
        //Turning pins off
    gpioPWM(RPWM, 0);
    gpioPWM(LPWM, 0);
    gpioWrite(R_EN, 0);
    gpioWrite(L_EN, 0);
    gpioServo(stickPin, 0);


    close(client_fd);
    close(server_fd);
    return 0;
}


