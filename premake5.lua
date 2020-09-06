workspace "AZ"
    configurations { "Release" }

project "AZ"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    includedirs {
        "vendor/include"
    }

    libdirs {
        "vendor/lib"
    }

    files { "**.h", "**.c" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        
