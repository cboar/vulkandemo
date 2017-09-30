//>>make clean; make; make test
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool is_device_valid(VkPhysicalDevice* device){
	uint32_t count = 0;
	VkQueueFamilyProperties queues[32];
	vkGetPhysicalDeviceQueueFamilyProperties(*device, &count, NULL);
	vkGetPhysicalDeviceQueueFamilyProperties(*device, &count, queues);
	for(uint32_t i = 0; i < count; i++){
		if(queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			return true;
	}
	return false;
}

void init(GLFWwindow** window, VkInstance* instance, VkPhysicalDevice* device){
	uint32_t count;

	/* GLFW WINDOW */
	glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    *window = glfwCreateWindow(800, 600, "Photon", NULL, NULL);

	/* VULKAN INSTANCE */
	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Photon",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0,
	};
	VkInstanceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 0,
	};
	count = 0;
	create_info.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count);
	create_info.enabledExtensionCount = count;
	if(vkCreateInstance(&create_info, NULL, instance) != VK_SUCCESS){
		fprintf(stderr, "Failed to create instance.\n");
	}

	/* VULKAN DEVICE */
	count = 0;
	*device = VK_NULL_HANDLE;
	VkPhysicalDevice devices[32];
	vkEnumeratePhysicalDevices(*instance, &count, NULL);
	vkEnumeratePhysicalDevices(*instance, &count, devices);
	for(uint32_t i = 0; i < count; i++){
		if(is_device_valid(devices + i)){
			*device = devices[i];
			break;
		}
	}
	if(*device == VK_NULL_HANDLE){
		fprintf(stderr, "Failed to find valid physical device.\n");
	}
}

int main(void){
	GLFWwindow* window;
	VkInstance instance;
	VkPhysicalDevice device;
	init(&window, &instance, &device);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
    }

	vkDestroyInstance(instance, NULL);
	glfwDestroyWindow(window);
    glfwTerminate();
}
