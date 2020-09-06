workspace "AZ"
    startproject "AZ"
    configurations { "Release" }
    platforms { "Win32" }

    filter "platforms:Win32"
        architecture "x86"

    filter "configurations:Release"
        symbols "Off"

include "Server"
include "Engine"