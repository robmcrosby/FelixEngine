#include <gtest/gtest.h>
#include <VulkanIncludes.hpp>


TEST(GraphicsTests, TestHelloWorld) {
    const auto expected = 1;
    const auto actual = 1;
    ASSERT_EQ(expected, actual);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    auto& instance = VulkanInstance::Get();
    instance.setApplicationName("Graphics Unit Tests");
    instance.setEngineName("No Engine");
    instance.enableValidation();

    return RUN_ALL_TESTS();
}
