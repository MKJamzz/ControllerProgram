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
#include <csignal>
#include <winsock2.h>
#include <ws2tcpip.h>    
using namespace std;

bool running = true;
bool ctrlCPressed = false;

// Handle Ctrl+C
void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n[INFO] Ctrl+C detected. Exiting...\n";
        ctrlCPressed = true;
        running = false;
    }
}

int main() {
    signal(SIGINT, handleSignal);  // Bind Ctrl+C handler

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // Connecting the gametroller on start up
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GameController* bunga = nullptr;

    if (SDL_IsGameController(0)) {
        bunga = SDL_GameControllerOpen(0);
        if (bunga) {
            std::cout << "PS5 Controller connected: " << SDL_GameControllerName(bunga) << std::endl;
        } else {
            std::cerr << "No PS5 controller detected.\n";
            SDL_Quit();
            return 1;
        }
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{AF_INET, htons(8999)};
    inet_pton(AF_INET, "100.90.207.61", &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to server.\n";
        return 1;
    }

    std::cout << "Connected to server.\n";

    int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));

    SDL_Event e;

    // Variables
    double leftX = 0;
    double rightTrigger = 0;
    double leftTrigger = 0;
    double servoControl = 0;

    while (running) {
        SDL_GameControllerUpdate();

        Sint16 rawLeftX = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_LEFTX);
        leftX = rawLeftX / 32767.0;

        //Implement the deadzones 
        if (leftX <= 0.013 && leftX >= -0.013) leftX = 0.0;
        leftX = std::clamp(leftX, -1.0, 1.0);

        Sint16 rawRT = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        rightTrigger = rawRT / 32767.0;
        rightTrigger = std::clamp(rightTrigger, 0.0, 1.0);

        Sint16 rawLT = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        leftTrigger = rawLT / 32767.0;
        leftTrigger = std::clamp(leftTrigger, 0.0, 1.0);

        Sint16 rawServo = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_LEFTY);
        servoControl = (rawServo / 32767.0 + 1.0) / 2.0;  // Normalized to [0, 1]

        uint8_t pwmLeftX = static_cast<uint8_t>((leftX + 1) / 2 * 255);
        uint8_t pwmRT = static_cast<uint8_t>(rightTrigger * 255);
        uint8_t pwmLT = static_cast<uint8_t>(leftTrigger * 255);
        uint8_t pwmServo = static_cast<uint8_t>(servoControl * 255);

        // Check if circle button is pressed
        bool circlePressed = SDL_GameControllerGetButton(bunga, SDL_CONTROLLER_BUTTON_B);

        if (circlePressed) {
            std::cout << "\n[INFO] Circle button pressed. Exiting...\n";
            running = false;
        }

        // Combine and send
        uint8_t buffer[6] = {
            pwmLeftX,
            pwmRT,
            static_cast<uint8_t>(circlePressed ? 1 : 0),
            static_cast<uint8_t>(ctrlCPressed ? 1 : 0),
            pwmServo,
            pwmLT
        };

        send(sock, reinterpret_cast<const char*>(buffer), 6, 0);

        cout << "RT: " << (int)pwmRT 
             << "LT: " << (int)pwmLT 
             << " | Steering: " << (int)pwmLeftX
             << " | Servo: " << (int)pwmServo
             << " | Circle: " << (int)buffer[2]
             << " | Ctrl+C: " << (int)buffer[3] << "\r";

        usleep(50000);
    }

    // Final buffer to stop everything on exit
    uint8_t killBuffer[6] = {0, 0, 1, 1, 0, 0};
    send(sock, reinterpret_cast<const char*>(killBuffer), 6, 0);

    close(sock);
    WSACleanup();
    SDL_GameControllerClose(bunga);
    SDL_Quit();

    std::cout << "\n[INFO] Exited cleanly.\n";
    return 0;
}
