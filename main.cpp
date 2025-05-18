//Whoever reads this is smelly
//C+ = smelly
//erm its c++ akshully
#include <cpr/cpr.h>
#include <iostream>

//Function Decloration
void controllerInputs(double & xAxis, double& yAxis, double& rightTrigger, bool& handbrake);


//Main function to run everything
int main(){

    auto response = cpr::Get(cpr::Url{"https://httpbin.org/get"});
    std::cout << "Status: " << response.status_code << std::endl;
    std::cout << "Body:\n" << response.text << std::endl;
    return 0;
}

//This functions returns all of the important information from the controller
void controllerInputs(double& xAxis, double& yAxis, double& rightTrigger, bool& handbrake){
    





    return;
}