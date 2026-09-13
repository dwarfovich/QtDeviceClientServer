#pragma once

template <typename T>
concept Enum = std::is_enum_v<T>;

template <Enum T>
std::underlying_type_t<T> toUnderlyingType(T value)
{
    return static_cast<std::underlying_type_t<T>>(value);
}