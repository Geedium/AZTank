project "Server"
    kind "ConsoleApp"
    staticruntime "on"
    language "C++"
    cppdialect "C++17"
    targetdir "%{wks.location}/bin"
    objdir "obj"

    pchheader "stdafx.h"
    pchsource "src/stdafx.cpp"
    
    includedirs {
        "src",
        "%{wks.location}/vendor/enet-1.3.15/include"
    }

    libdirs {
        "%{wks.location}/vendor/enet-1.3.15"
    }

    links {
        "enet.lib",
        "ws2_32.lib",
        "winmm.lib"
    }

    files { 
        "src/**.h",
        "src/**.cpp"
    }

    filter "configurations:Release"
        optimize "On"

        flags {
            "MultiProcessorCompile"
        }
        
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "NDEBUG"
        }

        linkoptions {
            "/SAFESEH:NO"
        }
        