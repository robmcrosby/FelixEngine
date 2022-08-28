#include "VulkanIncludes.hpp"


using namespace std;

int main() {
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("HelloVK");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  if (instance.init()) {
    cout << instance << endl;

    auto device = instance.pickDevice();
    if (device) {
      cout << "Picked Device: " << device << endl;
    }
  }
  return 0;
}
