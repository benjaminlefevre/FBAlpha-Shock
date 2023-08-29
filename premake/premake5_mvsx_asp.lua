makesettings [[
BINPATH  := ../../../fba-toolchains/linux-host/arm-none-linux-gnueabihf/bin
CC       := ${BINPATH}/arm-none-linux-gnueabihf-gcc
CXX      := ${BINPATH}/arm-none-linux-gnueabihf-g++
STRIP    := ${BINPATH}/arm-none-linux-gnueabihf-strip -s
]]

function set_location()
   location "../projects/mvsx_asp"
end

function set_targetdir()
   targetdir "../projects/mvsx_asp/bin/%{cfg.buildcfg}"
end

function set_defines()
   defines { "INLINE = static inline", "SH2_INLINE = static inline", "LSB_FIRST", "MVSX_ASP" }
   defines { "USE_SPEEDHACKS" }
end

function set_kind()
   kind "ConsoleApp"
end

function set_architecture()
   architecture "ARM"
end

function set_system()
   system "linux"
end

function set_links()
   linkoptions { "-static",
   "-mfloat-abi=hard",
   "-mfpu=neon-vfpv4"
   }

   links {
      "m",
      "pthread",
      "rt",
   }
end

function set_buildoptions()
   buildoptions
   {
      "-static",
      "-mfloat-abi=hard",
         "-mfpu=neon-vfpv4",
       "-s",
      "-fsigned-char",
      "-fsigned-char",
      "-Wno-write-strings",
   }

   filter {"files:**.c"}
      buildoptions {"-std=gnu99"}

   filter {"configurations:Debug"}
      buildoptions { "-O0", "-g" }
   filter {"configurations:Release"}
      buildoptions { "-O3", "-fomit-frame-pointer" }
   filter {}
end

function set_additional_includedirs()
   includedirs
   {
      "../src/burner/shock/core/platform/mvsx_asp",
      "../src/burner/shock/core/platform/posix",
   }
end

function set_exclude_files()
   local excludedirs = 
   {
         "../src/burner/shock/core/platform/lubuntu",
         "../src/burner/shock/core/platform/windows",
         "../src/burn/drv/midway",
         "../src/cpu/tms34",
         "../src/cpu/i386"
   }

   for _, excludedir in ipairs(excludedirs) do
      removefiles { excludedir .. "/**.cpp", excludedir .. "/**.c", excludedir .. "/**.h" }
   end
   
   removefiles { "../src/burner/winmain.cpp" }
end

include ("premake_shared.lua")
