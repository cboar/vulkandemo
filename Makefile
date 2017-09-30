VULKAN_SDK = ./sdk/1.0.61.1/x86_64
CFLAGS = -O3 -std=c11 -I$(VULKAN_SDK)/include
LDFLAGS = -L$(VULKAN_SDK)/lib `PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/ pkg-config --libs --static glfw3` -lvulkan

VulkanTest: main.c
	gcc $(CFLAGS) -o VulkanTest main.c $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	LD_LIBRARY_PATH=$(VULKAN_SDK)/lib VK_LAYER_PATH=$(VULKAN_SDK)/etc/explicit_layer.d ./VulkanTest

clean:
	rm -f VulkanTest
