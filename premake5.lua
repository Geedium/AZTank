workspace "AZ"
    startproject "AZ"
    configurations { "Release" }
    platforms { "Win32" }

    filter "platforms:Win32"
        architecture "x86"

include "Server"
include "Engine"