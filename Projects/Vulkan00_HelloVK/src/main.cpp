#include "VulkanIncludes.hpp"


using namespace std;

int main() {
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("HelloVK");
  instance.setEngineName("No Engine");
  instance.enableValidation();
  if (instance.init()) {
    cout << "It Works!" << endl;
  }

  return 0;
}
