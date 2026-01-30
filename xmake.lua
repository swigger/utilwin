set_project("utilwin")
set_version("0.1.11")

set_languages("c11")

add_rules("mode.debug", "mode.release")

if is_plat("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
end

target("utilwin")
    set_kind("static")
    
    add_includedirs("src/include", {public = true})
    add_includedirs("src/__make", "inc_win")
    
    add_files("src/dirent/*.c")
    add_files("src/misc/*.c")
    add_files("src/process/*.c")
    add_files("src/regex/*.c")
    add_files("src/win32u/*.c")
    
    add_headerfiles("src/include/*.h", {prefixdir = ""})
    add_headerfiles("src/process/unistd_uw.h", {prefixdir = ""})
    add_headerfiles("src/process/disasmlen.h", {prefixdir = ""})
    add_headerfiles("src/win32u/win32_u.h", {prefixdir = ""})
