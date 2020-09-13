workspace "AZ"
    startproject "Engine"
    configurations { "Release" }
    platforms { "Win32" }

    filter "platforms:Win32"
        architecture "x86"

    filter "configurations:Release"
        symbols "Off"
        runtime "Release"

output_dir = "%{cfg.system}_%{cfg.buildcfg}-%{cfg.architecture}"

include "Server"
include "Engine"