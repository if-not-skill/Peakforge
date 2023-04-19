workspace "Peakforge"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Shipping"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["ImGui"] = "Peakforge/vendor/imgui"

startproject "Sandbox"

include "Peakforge/vendor/ImGui"

project "Peakforge"
    location "Peakforge"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pfpch.h"
    pchsource "Peakforge/source/pfpch.cpp"

    files
    {
        "%{prj.name}/source/**.h",
        "%{prj.name}/source/**.cpp",
    }

    includedirs
    {
        "%{prj.name}/source",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
    }

    links
    {
        "ImGui",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PF_PLATFORM_WINDOWS",
        }

    filter "configurations:Debug"
        defines 
        {
            "PF_DEBUG",
            "PF_ENABLE_ASSERTS"
        }
        symbols "on"
        
    filter "configurations:Release"
        defines "PF_RELEASE"
        optimize "on"
    
    filter "configurations:Shipping"
        defines "PF_SHIPPING"
        optimize "on"


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/source/**.h",
        "%{prj.name}/source/**.cpp"
    }

    includedirs
    {
        "Peakforge/source",
        "Peakforge/vendor/spdlog/include"
    }

    links
    {
        "Peakforge"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PF_PLATFORM_WINDOWS",
        }

        filter "configurations:Debug"
            defines "PF_DEBUG"
            runtime "Debug"
            symbols "on"
            
        filter "configurations:Release"
            defines "PF_RELEASE"
            runtime "Release"
            optimize "on"

        filter "configurations:Shipping"
            defines "PF_SHIPPING"
            runtime "Release"
            optimize "on"