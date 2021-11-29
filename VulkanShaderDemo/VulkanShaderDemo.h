#pragma once
#include "VkBootstrap.h"
#include "common.h"
#include <GLFW/glfw3.h>
#include <fstream>
static GLFWwindow* os_window;
static VulkanLibrary vk_lib;
static vkb::Instance instance;
static VkSurfaceKHR surface;
static vkb::Device device;
static vkb::Swapchain swapchain;

const int MAX_FRAMES_IN_FLIGHT = 2;
static	VkQueue graphics_queue;
static	VkQueue present_queue;

static	std::vector<VkImage> swapchain_images;
static	std::vector<VkImageView> swapchain_image_views;
static	std::vector<VkFramebuffer> framebuffers;

static	VkRenderPass render_pass;
static	VkPipelineLayout pipeline_layout;
static	VkPipeline graphics_pipeline;

static	VkCommandPool command_pool;
static	std::vector<VkCommandBuffer> command_buffers;

static	std::vector<VkSemaphore> available_semaphores;
static	std::vector<VkSemaphore> finished_semaphore;
static	std::vector<VkFence> in_flight_fences;
static	std::vector<VkFence> image_in_flight;
static	size_t current_frame = 0;



////////////
////////////////

int device_initialization() {
  os_window = create_window_glfw("Vulkan Triangle", true);

  vkb::InstanceBuilder instance_builder;
  auto instance_ret = instance_builder.use_default_debug_messenger().request_validation_layers().build();
  if (!instance_ret) {
    std::cout << instance_ret.error().message() << "\n";
    return -1;
  }
  instance = instance_ret.value();

  vk_lib.init(instance);

  surface = create_surface_glfw(instance, os_window);

  vkb::PhysicalDeviceSelector phys_device_selector(instance);
  auto phys_device_ret = phys_device_selector.set_surface(surface).select();
  if (!phys_device_ret) {
    std::cout << phys_device_ret.error().message() << "\n";
    return -1;
  }
  vkb::PhysicalDevice physical_device = phys_device_ret.value();

  vkb::DeviceBuilder device_builder{ physical_device };
  auto device_ret = device_builder.build();
  if (!device_ret) {
    std::cout << device_ret.error().message() << "\n";
    return -1;
  }
  device = device_ret.value();
  vk_lib.init(device);

  return 0;
}

int create_swapchain() {

  vkb::SwapchainBuilder swapchain_builder{ device };
  auto swap_ret = swapchain_builder.set_old_swapchain(swapchain).build();
  if (!swap_ret) {
    std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
    return -1;
  }
  vkb::destroy_swapchain(swapchain);
  swapchain = swap_ret.value();
  return 0;
}

int get_queues() {
  auto gq = device.get_queue(vkb::QueueType::graphics);
  if (!gq.has_value()) {
    std::cout << "failed to get graphics queue: " << gq.error().message() << "\n";
    return -1;
  }
  graphics_queue = gq.value();

  auto pq = device.get_queue(vkb::QueueType::present);
  if (!pq.has_value()) {
    std::cout << "failed to get present queue: " << pq.error().message() << "\n";
    return -1;
  }
  present_queue = pq.value();
  return 0;
}

int create_render_pass() {
  VkAttachmentDescription color_attachment = {};
  color_attachment.format = swapchain.image_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS) {
    std::cout << "failed to create render pass\n";
    return -1; // failed to create render pass!
  }
  return 0;
}

std::vector<char> readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), static_cast<std::streamsize> (file_size));

  file.close();

  return buffer;
}

VkShaderModule createShaderModule(const std::vector<char>& code) {
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t*> (code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &create_info, nullptr, &shaderModule) != VK_SUCCESS) {
    return VK_NULL_HANDLE; // failed to create shader module
  }

  return shaderModule;
}

int create_graphics_pipeline() {
  auto vert_code = readFile("vert.spv");
  auto frag_code = readFile("frag.spv");

  VkShaderModule vert_module = createShaderModule(vert_code);
  VkShaderModule frag_module = createShaderModule(frag_code);
  if (vert_module == VK_NULL_HANDLE || frag_module == VK_NULL_HANDLE) {
    std::cout << "failed to create shader module\n";
    return -1; // failed to create shader modules
  }

  VkPipelineShaderStageCreateInfo vert_stage_info = {};
  vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_stage_info.module = vert_module;
  vert_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo frag_stage_info = {};
  frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_stage_info.module = frag_module;
  frag_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.vertexAttributeDescriptionCount = 0;

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapchain.extent.width;
  viewport.height = (float)swapchain.extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = { 0, 0 };
  scissor.extent = swapchain.extent;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &colorBlendAttachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 0;
  pipeline_layout_info.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(
    device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
    std::cout << "failed to create pipeline layout\n";
    return -1; // failed to create pipeline layout
  }

  std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamic_info = {};
  dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_info.dynamicStateCount = static_cast<uint32_t> (dynamic_states.size());
  dynamic_info.pDynamicStates = dynamic_states.data();

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = &dynamic_info;
  pipeline_info.layout = pipeline_layout;
  pipeline_info.renderPass = render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(
    device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS) {
    std::cout << "failed to create pipline\n";
    return -1; // failed to create graphics pipeline
  }

  vkDestroyShaderModule(device, frag_module, nullptr);
  vkDestroyShaderModule(device, vert_module, nullptr);
  return 0;
}

int create_framebuffers() {
  swapchain_images = swapchain.get_images().value();
  swapchain_image_views = swapchain.get_image_views().value();

  framebuffers.resize(swapchain_image_views.size());

  for (size_t i = 0; i < swapchain_image_views.size(); i++) {
    VkImageView attachments[] = { swapchain_image_views[i] };

    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = swapchain.extent.width;
    framebuffer_info.height = swapchain.extent.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS) {
      return -1; // failed to create framebuffer
    }
  }
  return 0;
}

int create_command_pool() {
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = device.get_queue_index(vkb::QueueType::graphics).value();

  if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
    std::cout << "failed to create command pool\n";
    return -1; // failed to create command pool
  }
  return 0;
}

int create_command_buffers() {
  command_buffers.resize(framebuffers.size());

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = command_pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)command_buffers.size();

  if (vkAllocateCommandBuffers(device, &allocInfo, command_buffers.data()) != VK_SUCCESS) {
    return -1; // failed to allocate command buffers;
  }

  for (size_t i = 0; i < command_buffers.size(); i++) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffers[i], &begin_info) != VK_SUCCESS) {
      return -1; // failed to begin recording command buffer
    }

    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass;
    render_pass_info.framebuffer = framebuffers[i];
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = swapchain.extent;
    VkClearValue clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchain.extent.width;
    viewport.height = (float)swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.extent;

    vkCmdSetViewport(command_buffers[i], 0, 1, &viewport);
    vkCmdSetScissor(command_buffers[i], 0, 1, &scissor);

    vkCmdBeginRenderPass(command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

    vkCmdDraw(command_buffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffers[i]);

    if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
      std::cout << "failed to record command buffer\n";
      return -1; // failed to record command buffer!
    }
  }
  return 0;
}

int create_sync_objects() {
  available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  finished_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
  in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
  image_in_flight.resize(swapchain.image_count, VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device, &semaphore_info, nullptr, &available_semaphores[i]) != VK_SUCCESS ||
      vkCreateSemaphore(device, &semaphore_info, nullptr, &finished_semaphore[i]) != VK_SUCCESS ||
      vkCreateFence(device, &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
      std::cout << "failed to create sync objects\n";
      return -1; // failed to create synchronization objects for a frame
    }
  }
  return 0;
}

int recreate_swapchain() {
  vkDeviceWaitIdle(device);

  vkDestroyCommandPool(device, command_pool, nullptr);

  for (auto framebuffer : framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  swapchain.destroy_image_views(swapchain_image_views);

  if (0 != create_swapchain()) return -1;
  if (0 != create_framebuffers()) return -1;
  if (0 != create_command_pool()) return -1;
  if (0 != create_command_buffers()) return -1;
  return 0;
}

int draw_frame() {
  vkWaitForFences(device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

  uint32_t image_index = 0;
  VkResult result = vkAcquireNextImageKHR(device,
    swapchain,
    UINT64_MAX,
    available_semaphores[current_frame],
    VK_NULL_HANDLE,
    &image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return recreate_swapchain();
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    std::cout << "failed to acquire swapchain image. Error " << result << "\n";
    return -1;
  }

  if (image_in_flight[image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(device, 1, &image_in_flight[image_index], VK_TRUE, UINT64_MAX);
  }
  image_in_flight[image_index] = in_flight_fences[current_frame];

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = { available_semaphores[current_frame] };
  VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = wait_semaphores;
  submitInfo.pWaitDstStageMask = wait_stages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffers[image_index];

  VkSemaphore signal_semaphores[] = { finished_semaphore[current_frame] };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signal_semaphores;

  vkResetFences(device, 1, &in_flight_fences[current_frame]);

  if (vkQueueSubmit(graphics_queue, 1, &submitInfo, in_flight_fences[current_frame]) != VK_SUCCESS) {
    std::cout << "failed to submit draw command buffer\n";
    return -1; //"failed to submit draw command buffer
  }

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapChains[] = { swapchain };
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapChains;

  present_info.pImageIndices = &image_index;

  result = vkQueuePresentKHR(present_queue, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    return recreate_swapchain();
  } else if (result != VK_SUCCESS) {
    std::cout << "failed to present swapchain image\n";
    return -1;
  }

  current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
  return 0;
}

void cleanup() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, finished_semaphore[i], nullptr);
    vkDestroySemaphore(device, available_semaphores[i], nullptr);
    vkDestroyFence(device, in_flight_fences[i], nullptr);
  }

  vkDestroyCommandPool(device, command_pool, nullptr);

  for (auto framebuffer : framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  vkDestroyPipeline(device, graphics_pipeline, nullptr);
  vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
  vkDestroyRenderPass(device, render_pass, nullptr);

  swapchain.destroy_image_views(swapchain_image_views);

  vkb::destroy_swapchain(swapchain);
  vkb::destroy_device(device);
  vkb::destroy_surface(instance, surface);
  vkb::destroy_instance(instance);
  destroy_window_glfw(os_window);
}






//////////////
/////////////

int init() {
  if (0 != device_initialization()) return -1;
  if (0 != create_swapchain()) return -1;
  if (0 != get_queues()) return -1;
  if (0 != create_render_pass()) return -1;
  if (0 != create_graphics_pipeline()) return -1;
  if (0 != create_framebuffers()) return -1;
  if (0 != create_command_pool()) return -1;
  if (0 != create_command_buffers()) return -1;
  if (0 != create_sync_objects()) return -1;
}

bool running() {
  return !glfwWindowShouldClose(os_window);
}
void update() {

}
void render() {
  try {
  if (draw_frame() != 0)
    throw("failed to draw frame\n");
  } catch (const std::exception& e) {
        
  }
  glfwSwapBuffers(os_window);
}
void pollevents() {
  glfwPollEvents();
}
