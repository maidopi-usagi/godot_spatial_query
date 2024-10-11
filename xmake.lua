add_rules("mode.debug", "mode.release")

add_requires("godotcpp4")

PROJECT_NAME = "godot_spatial_query"
GODOT_PROJECT_DIR = "test/addons/spatial_query/"

target(PROJECT_NAME)
    set_kind("shared")
    set_languages("c++20")
    add_packages("godotcpp4")
    add_files("src/*.cpp")
    set_basename(PROJECT_NAME .. "_$(mode)_$(plat)_$(arch)")
    set_targetdir(GODOT_PROJECT_DIR .. "bin/$(os)")
    after_build(function (target)
        print("aabababab")
    end)
