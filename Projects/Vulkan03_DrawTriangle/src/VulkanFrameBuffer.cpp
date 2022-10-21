#include "VulkanIncludes.hpp"


using namespace std;


VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device): mDevice(device) {

}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  destroy();
}

void VulkanFrameBuffer::addColorAttachment(VulkanImagePtr image) {
  mColorAttachments.push_back(image);
}

void VulkanFrameBuffer::destroy() {
  mColorAttachments.clear();
}
