#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <AL/al.h>
#include <AL/alc.h>
#include <algorithm>
#include <array>
#include <bits/stdc++.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <map>
#include <memory_resource>
#include <optional>
#include <set>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

static const uint32_t CUBEMAP_SIDES = 6;