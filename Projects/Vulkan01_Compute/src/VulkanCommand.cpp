#include "VulkanIncludes.hpp"


using namespace std;


VulkanCommand::VulkanCommand(VulkanQueue* queue):
  mQueue(queue) {

}

VulkanCommand::~VulkanCommand() {
  destroy();
}

void VulkanCommand::destroy() {

}
