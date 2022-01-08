project "Engine"
    kind "ConsoleApp"
    staticruntime "off"
    language "C++"
    cppdialect "C++17"
    targetdir "%{wks.location}/bin"
    objdir "obj"
    
    pchheader "System.h"
    pchsource "Source/System.cpp"

    includedirs {
        "src",
        "Source",
        "%{wks.location}/vendor/glfw-3.3.2.bin.WIN32/include",
        "%{wks.location}/vendor/libvorbis/include",
        "%{wks.location}/vendor/FreeImage/include",
        "%{wks.location}/vendor/GLEW/include",
        "%{wks.location}/vendor/libogg/include",
        "%{wks.location}/vendor/OpenAL/include",
        "%{wks.location}/vendor/enet-1.3.15/include",
        "%{wks.location}/vendor/freetype/include"
    }

    libdirs {
        "%{wks.location}/vendor/glfw-3.3.2.bin.WIN32/lib-vc2019",
        "%{wks.location}/vendor/libvorbis/bin",
        "%{wks.location}/vendor/FreeImage/lib",
        "%{wks.location}/vendor/GLEW/lib",
        "%{wks.location}/vendor/libogg/bin",
        "%{wks.location}/vendor/OpenAL/libs/Win32",
        "%{wks.location}/vendor/enet-1.3.15",
        "%{wks.location}/vendor/freetype/objs/Win32/Release"
    }

    links {
        "opengl32",
        "glfw3.lib",
        "glew32s.lib",
        "OpenAL32.lib",
        "FreeImage.lib",
        "freetype.lib",
        "ws2_32.lib",
        "winmm.lib",
        "libvorbis.lib",
        "libogg.lib",
        "enet.lib"
    }

    files { 
        "src/**.h",
        "src/**.cpp",
        "Source/**.h",
        "Source/**.cpp"
    }

    filter "configurations:Release"
        optimize "On"
        buildoptions "/MD"

        flags {
            "MultiProcessorCompile"
        }
        
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_DEPRECATE",
            "GLEW_STATIC",
            "NDEBUG"
        }

        linkoptions {
            "/SAFESEH:NO",
            "/NODEFAULTLIB:library"
        }
        