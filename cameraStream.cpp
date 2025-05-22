//add includes needed here

//false on default still have to create helper functions to test if camera/vr are connected; turns true if connected
bool cameraIsConnected = false;
bool vrIsConnected = false;

bool checkCameraConnected(){
// checks if camera is properly connected
  return true;
}

bool checkVrConnected(){
// checks if vr is properly connected
  return true;
}

void startCamera(){
// initalizes the Rasberry Pi Camera
}

void startVR(){
//initalizes the VR SDK
}

void streamCameraToVR(){
  while(true){
  //captures frames from camera
  //sends frame to vr display
  //handles fps you want to set
  }

}
