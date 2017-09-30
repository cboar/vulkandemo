//>>make clean; make; make test
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void init_window(GLFWwindow** window){
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	*window = glfwCreateWindow(800, 600, "Photon", NULL, NULL);
}

bool create_instance(VkInstance* instance){
	uint32_t count;
	VkApplicationInfo app_info = {0};
	app_info = (VkApplicationInfo){
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Photon",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0,
	};
	VkInstanceCreateInfo create_info = {0};
	create_info = (VkInstanceCreateInfo){
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 0,
	};
	create_info.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count);
	create_info.enabledExtensionCount = count;
	return vkCreateInstance(&create_info, NULL, instance) == VK_SUCCESS;
}

uint32_t graphics_queue_index(VkPhysicalDevice* device){
	uint32_t count;
	VkQueueFamilyProperties queues[32];
	vkGetPhysicalDeviceQueueFamilyProperties(*device, &count, NULL);
	vkGetPhysicalDeviceQueueFamilyProperties(*device, &count, queues);
	for(uint32_t i = 0; i < count; i++){
		if(queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			return i;
	}
	return -1;
}

bool pick_phys_device(VkPhysicalDevice* phys_device, uint32_t* queue_index, VkInstance instance){
	uint32_t count;
	VkPhysicalDevice devices[32];
	vkEnumeratePhysicalDevices(instance, &count, NULL);
	vkEnumeratePhysicalDevices(instance, &count, devices);
	for(uint32_t i = 0; i < count; i++){
		uint32_t temp = graphics_queue_index(devices + i);
		if(temp != -1){
			*phys_device = devices[i];
			*queue_index = temp;
			return true;
		}
	}
	return false;
}

bool create_logical_device(VkDevice* logical_device, VkPhysicalDevice phys_device, uint32_t queue_index){
	float priority = 1.0f;
	VkDeviceQueueCreateInfo queue_create_info = { 0 };
	queue_create_info = (VkDeviceQueueCreateInfo){
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queue_index,
		.pQueuePriorities = &priority,
		.queueCount = 1,
	};
	VkPhysicalDeviceFeatures features = { 0 };
	VkDeviceCreateInfo dev_create_info = { 0 };
	dev_create_info = (VkDeviceCreateInfo){
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pQueueCreateInfos = &queue_create_info,
		.pEnabledFeatures = &features,
		.queueCreateInfoCount = 1,
		.enabledLayerCount = 0,
	};
	return vkCreateDevice(phys_device, &dev_create_info, NULL, logical_device) == VK_SUCCESS;
}

void init_vulkan(VkInstance* instance, VkPhysicalDevice* phys_device, VkDevice* logical_device, VkQueue* graphics_queue){
	uint32_t queue_index;
	create_instance(instance);
	pick_phys_device(phys_device, &queue_index, *instance);
	create_logical_device(logical_device, *phys_device, queue_index);
	vkGetDeviceQueue(*logical_device, queue_index, 0, graphics_queue);
}

int main(void){
	GLFWwindow* window;
	init_window(&window);

	VkInstance instance;
	VkPhysicalDevice phys_device;
	VkDevice logical_device;
	VkQueue graphics_queue;
	init_vulkan(&instance, &phys_device, &logical_device, &graphics_queue);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	vkDestroyInstance(instance, NULL);
	vkDestroyDevice(logical_device, NULL);
	glfwDestroyWindow(window);
	glfwTerminate();
}
