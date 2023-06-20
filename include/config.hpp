#pragma once
#ifndef KNC_CONFIG_HPP
#define KNC_CONFIG_HPP
#include <cstdint>
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;
struct Vec2u16 {
    u16 x;
    u16 y;
};
namespace Consts {
    inline constexpr Vec2u16 WindowSize = {1049u, 811u};
};
#endif