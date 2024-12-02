#pragma once

#ifdef _WIN32
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <winsock2.h>
#endif // _WIN32

#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <regex>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "nlohmann/json.hpp"
#include <fmt/core.h>
#include <gfxapi/gfxapi.h>
#include <gsl/gsl>

#include <simplege/references.h>