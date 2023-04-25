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
IncludeDir["glm"] = "Peakforge/vendor/glm"

startproject "Sandbox"

include "Peakforge/vendor/imgui"

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
        "%{prj.name}/source/**.hlsl",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
    }

    filter { "files:**.hlsl" }
        flags "ExcludeFromBuild"
        shadermodel "5.0"
    
    filter { "files:**PixelShader.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Pixel"
        -- shaderentry "ForPixel"
    filter { "files:**VertexShader.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Vertex"
        -- shaderentry "ForVertex"
    filter {}

    includedirs
    {
        "%{prj.name}/resources",
        "%{prj.name}/source",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
    }

    links
    {
        "ImGui"
    }

    filter "system:windows"
        systemversion "latest"
        debugdir "%{cfg.buildtarget.directory}"

        links
        {   
            "DirectXTK.lib",
            "d3d11.lib",
            "dxgi.lib",
            "dxguid.lib",
            "uuid.lib",
            "kernel32.lib",
            "user32.lib",
            "comdlg32.lib",
            "advapi32.lib",
            "shell32.lib",
            "ole32.lib",
            "oleaut32.lib"
        }

        includedirs
        {
            "%{prj.name}/vendor/DirectXTK/Inc",
        }

        defines
        {
            "PF_PLATFORM_WINDOWS",
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.directory}**.cso ../bin/" .. outputdir .. "/Sandbox/Engine/Shaders"),
            ("{COPYDIR} %{prj.location}content %{cfg.buildtarget.directory}../Sandbox/Engine/")
        }

    filter "configurations:Debug"
        defines 
        {
            "PF_DEBUG",
            "PF_ENABLE_ASSERTS"
        }
        symbols "on"

        libdirs 
        { 
            "%{prj.name}/vendor/DirectXTK/Bin/Windows10_2022/x64/Debug",
        }
        
    filter "configurations:Release"
        defines "PF_RELEASE"
        optimize "on"

        libdirs 
        { 
            "%{prj.name}/vendor/DirectXTK/Bin/Windows10_2022/x64/Release",
        }
    
    filter "configurations:Shipping"
        defines "PF_SHIPPING"
        optimize "on"

        libdirs 
        { 
            "%{prj.name}/vendor/DirectXTK/Bin/Windows10_2022/x64/Release",
        }


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
        "%{prj.name}/resources/Resource.rc",
        "%{prj.name}/resources/AppIcon.ico",

        "%{prj.name}/source/**.h",
        "%{prj.name}/source/**.cpp"
    }

    includedirs
    {
        "Peakforge/vendor/spdlog/include",
        "Peakforge/vendor",
        "Peakforge/source",
        "%{IncludeDir.glm}",
    }

    links
    {
        "Peakforge",
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