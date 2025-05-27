//Whoever reads this is smelly
//C+ = smelly
//erm its c++ akshully

//Run this line in terminal to test the build
// ./build/ControllerProgram.exe

#define SDL_MAIN_HANDLED    
#include <SDL.h>
#include <iostream>
#include <cstdint>
#include <csignal>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

bool running = true;
bool ctrlCPressed = false;

// Ctrl+C handler
void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n[INFO] Ctrl+C detected — exiting...\n";
        ctrlCPressed = true;
        running = false;
    }
}

int clampToPWM(double value) {
    return std::max(0, std::min(255, static_cast<int>((value + 1.0) * 127.5)));
}

int main() {
    signal(SIGINT, handleSignal);  // Bind Ctrl+C

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

    // === Socket Setup ===
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed\n";
        SDL_Quit();
        return 1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8000);
    inet_pton(AF_INET, "192.168.1.100", &server.sin_addr);  // ← update this IP

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Failed to connect to RC car server\n";
        close(sock);
        SDL_Quit();
        return 1;
    }

    std::cout << "[INFO] Connected to RC car\n";

    SDL_Event e;
    double leftX = 0;
    double rightTrigger = 0;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        bool circlePressed = SDL_GameControllerGetButton(bunga, SDL_CONTROLLER_BUTTON_B);

        if (circlePressed) {
            std::cout << "\n[INFO] Circle button pressed — exiting...\n";
            running = false;
        }

        Sint16 rawLeftX = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_LEFTX);
        leftX = rawLeftX / 32767.0;
        if (abs(leftX) < 0.012) leftX = 0.0;
        if (leftX < -1) leftX = -1;

        Sint16 rawThrottle = SDL_GameControllerGetAxis(bunga, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        rightTrigger = rawThrottle / 32767.0;

        uint8_t steer = clampToPWM(leftX);
        uint8_t throttle = clampToPWM(rightTrigger);
        uint8_t buffer[4] = {
            steer,               // buffer[0] = steering
            throttle,            // buffer[1] = throttle
            circlePressed ? 1 : 0, // buffer[2] = Circle button flag
            ctrlCPressed ? 1 : 0   // buffer[3] = Ctrl+C flag
        };

        if (send(sock, buffer, 4, 0) < 0) {
            std::cerr << "\n[ERROR] Failed to send data to car!\n";
            break;
        }

        std::cout << "Throttle: " << rightTrigger
                  << " | Steering: " << leftX
                  << " | Circle: " << buffer[2]
                  << " | Ctrl+C: " << buffer[3] << "\r";

        SDL_Delay(50);
    }

    // Send final kill buffer to signal car to stop
    uint8_t killBuffer[4] = {0, 0, 1, 1};
    send(sock, killBuffer, 4, 0);

    SDL_GameControllerClose(bunga);
    SDL_Quit();
    close(sock);

    std::cout << "\n[INFO] Program exited cleanly.\n";
    return 0;
}
