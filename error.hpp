#pragma once
#include "common.hpp"

enum class Error {
    Success,
    WouldBlock,
    NoBytes,
    Interrupt,
    Shutdown,
    Fatal,
};