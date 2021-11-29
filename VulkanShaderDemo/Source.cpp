#include "VulkanShaderDemo.h"
int main(){
  int ret = init();
  while(running()){
    update();
    render();
    pollevents();
  }
  vkDeviceWaitIdle(device);
  cleanup();
  return ret;
}