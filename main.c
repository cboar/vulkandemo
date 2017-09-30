//>>make clean; make; make test
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct VulkanBase {
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice phys_device;
	VkDevice device;
	VkQueue graphics_queue, present_queue;
	uint32_t graphics_index, present_index;
} VulkanBase;

void init_window(GLFWwindow** window){
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	*window = glfwCreateWindow(800, 600, "Photon", NULL, NULL);
}

void cleanup(VulkanBase base){
	vkDestroyDevice(base.device, NULL);
	vkDestroySurfaceKHR(base.instance, base.surface, NULL);
	vkDestroyInstance(base.instance, NULL);
}

bool create_instance(VulkanBase* base){
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
	uint32_t count;
	create_info.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count);
	create_info.enabledExtensionCount = count;
	return vkCreateInstance(&create_info, NULL, &(base->instance)) == VK_SUCCESS;
}

bool valid_phys_device(VulkanBase* base, VkPhysicalDevice phys_device){
	VkBool32 present_support;
	uint32_t count, graphics_index = -1, present_index = -1;
	VkQueueFamilyProperties queues[32];
	vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &count, NULL);
	vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &count, queues);
	for(uint32_t i = 0; i < count; i++){
		vkGetPhysicalDeviceSurfaceSupportKHR(phys_device, i, base->surface, &present_support);
		if(present_support)
			present_index = i;
		if(queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			graphics_index = i;
	}
	if(graphics_index != -1 && present_index != -1){
		base->phys_device = phys_device;
		base->graphics_index = graphics_index;
		base->present_index = present_index;
		return true;
	}
	return false;
}

bool pick_phys_device(VulkanBase* base){
	uint32_t count;
	VkPhysicalDevice devices[32];
	vkEnumeratePhysicalDevices(base->instance, &count, NULL);
	vkEnumeratePhysicalDevices(base->instance, &count, devices);
	for(uint32_t i = 0; i < count; i++){
		if(valid_phys_device(base, devices[i]))
			return true;
	}
	return false;
}

bool create_logical_device(VulkanBase* base){
	uint32_t family_indices[2] = { base->graphics_index, base->present_index };
	VkDeviceQueueCreateInfo queue_create_infos[2] = {0};
	for(uint32_t i = 0; i < 2; i++){
		float priority = 1.0f;
		queue_create_infos[i] = (VkDeviceQueueCreateInfo){
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family_indices[i],
			.pQueuePriorities = &priority,
			.queueCount = 1,
		};
	}
	VkPhysicalDeviceFeatures features = {0};
	VkDeviceCreateInfo dev_create_info = {0};
	dev_create_info = (VkDeviceCreateInfo){
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pQueueCreateInfos = queue_create_infos,
		.pEnabledFeatures = &features,
		.queueCreateInfoCount = 2,
		.enabledLayerCount = 0,
	};
	return vkCreateDevice(base->phys_device, &dev_create_info, NULL, &(base->device)) == VK_SUCCESS;
}

void init_vulkan(GLFWwindow* window, VulkanBase* base){
	create_instance(base);
	glfwCreateWindowSurface(base->instance, window, NULL, &(base->surface));
	pick_phys_device(base);
	create_logical_device(base);
	vkGetDeviceQueue(base->device, base->graphics_index, 0, &(base->graphics_queue));
	vkGetDeviceQueue(base->device, base->present_index, 0, &(base->present_queue));
}

int main(void){
	GLFWwindow* window;
	init_window(&window);

	VulkanBase base;
	init_vulkan(window, &base);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	cleanup(base);
	glfwDestroyWindow(window);
	glfwTerminate();
}
