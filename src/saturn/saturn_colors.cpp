#include "saturn/saturn_colors.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/imgui/saturn_imgui.h"

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"

#include "pc/configfile.h"

extern "C" {
#include "game/camera.h"
#include "game/level_update.h"
#include "sm64.h"
}

using namespace std;
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
#include <array>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

// arrays are ordered like this: [{RLight, RDark}, {GLight, GDark}, {BLight, BDark}]
// NOTE: This will only work with C++ standards above C++14!
// There are most likely ways to further minimize repetition and overall complexity,
// However i will not be doing that here

// Default
std::array<std::array<unsigned int, 2>, 3> defaultColorHat {{255, 127}, {0, 0}, {0, 0}};
std::array<std::array<unsigned int, 2>, 3> defaultColorOveralls {{0, 0}, {0, 0}, {255, 127}};
std::array<std::array<unsigned int, 2>, 3> defaultColorGloves {{255, 127}, {255, 127}, {255, 127}};
std::array<std::array<unsigned int, 2>, 3> defaultColorShoes {{114, 57}, {28, 14}, {14, 7}};
std::array<std::array<unsigned int, 2>, 3> defaultColorSkin {{254, 127}, {193, 96}, {121, 60}};
std::array<std::array<unsigned int, 2>, 3> defaultColorHair {{115, 57}, {6, 3}, {0, 0}};

// CometSPARK
std::array<std::array<unsigned int, 2>, 3> sparkColorShirt {{255, 127}, {255, 127}, {0, 0}};
std::array<std::array<unsigned int, 2>, 3> sparkColorShoulders {{0, 0}, {255 127}, {255, 127}};
std::array<std::array<unsigned int, 2>, 3> sparkColorArms {{0, 0}, {255, 127}, {127, 64}};
std::array<std::array<unsigned int, 2>, 3> sparkColorOverallsBottom {{255, 127}, {0, 0}, {255, 127}}; 
std::array<std::array<unsigned int, 2>, 3> sparkColorLegTop {{255, 127}, {0, 0}, {127, 64}};
std::array<std::array<unsigned int, 2>, 3> sparkColorLegBottom {{127, 64}, {0, 0}, {255, 127}};

std::array<std::array<unsigned int, 2>, 3> chromaColor {{0, 0}, {255, 127}, {0, 0}};

// Color Codes

bool cc_model_support = true;
bool cc_spark_support = false;

std::vector<string> cc_array;
string colorCodeDir;

bool modelCcLoaded;

std::vector<string> model_cc_array;
string modelColorCodeDir;

std::string formatColumn(std::array<std::array<unsigned int, 2>, 3> &colorBodyPart, int shade) {
    char column[64];
    ImFormatString(column, IM_ARRAYSIZE(column), "%02X%02X%02X"
            , ImClamp(colorBodyPart[0][shade], 0, 255)
            , ImClamp(colorBodyPart[1][shade], 0, 255)
            , ImClamp(colorBodyPart[2][shade], 0, 255));
    std::string col = column;

    return col;
} // this should help remove a lot of the repetition,
  // however it's still very repetitive and should be worked on further
  // shade can either be 0 or 1, 0 for light, 1 for dark

/*
    The currently active GameShark code in string format.
*/
std::string global_gs_code() {
    std::string gameshark;

    // TODO: Clean this code up, I dont think it needs 36 similar blocks of code lol

    std::string col1 = formatColumn(defaultColorHat, 0);
    std::string col2 = formatColumn(defaultColorHat, 1);

    std::string col3 = formatColumn(defaultColorOveralls, 0);
    std::string col4 = formatColumn(defaultColorOveralls, 1);

    std::string col5 = formatColumn(defaultColorGloves, 0);
    std::string col6 = formatColumn(defaultColorGloves, 1);

    std::string col7 = formatColumn(defaultColorShoes, 0);
    std::string col8 = formatColumn(defaultColorShoes, 1);

    std::string col9 = formatColumn(defaultColorSkin, 0);
    std::string col10 = formatColumn(defaultColorSkin, 1);

    std::string col11 = formatColumn(defaultColorHair, 0);
    std::string col12 = formatColumn(defaultColorHair, 1);

    gameshark += "8107EC40 " + col1.substr(0, 2) + col1.substr(2, 2) + "\n";
    gameshark += "8107EC42 " + col1.substr(4, 2) + "00\n";
    gameshark += "8107EC38 " + col2.substr(0, 2) + col2.substr(2, 2) + "\n";
    gameshark += "8107EC3A " + col2.substr(4, 2) + "00\n";
    gameshark += "8107EC28 " + col3.substr(0, 2) + col3.substr(2, 2) + "\n";
    gameshark += "8107EC2A " + col3.substr(4, 2) + "00\n";
    gameshark += "8107EC20 " + col4.substr(0, 2) + col4.substr(2, 2) + "\n";
    gameshark += "8107EC22 " + col4.substr(4, 2) + "00\n";
    gameshark += "8107EC58 " + col5.substr(0, 2) + col5.substr(2, 2) + "\n";
    gameshark += "8107EC5A " + col5.substr(4, 2) + "00\n";
    gameshark += "8107EC50 " + col6.substr(0, 2) + col6.substr(2, 2) + "\n";
    gameshark += "8107EC52 " + col6.substr(4, 2) + "00\n";
    gameshark += "8107EC70 " + col7.substr(0, 2) + col7.substr(2, 2) + "\n";
    gameshark += "8107EC72 " + col7.substr(4, 2) + "00\n";
    gameshark += "8107EC68 " + col8.substr(0, 2) + col8.substr(2, 2) + "\n";
    gameshark += "8107EC6A " + col8.substr(4, 2) + "00\n";
    gameshark += "8107EC88 " + col9.substr(0, 2) + col9.substr(2, 2) + "\n";
    gameshark += "8107EC8A " + col9.substr(4, 2) + "00\n";
    gameshark += "8107EC80 " + col10.substr(0, 2) + col10.substr(2, 2) + "\n";
    gameshark += "8107EC82 " + col10.substr(4, 2) + "00\n";
    gameshark += "8107ECA0 " + col11.substr(0, 2) + col11.substr(2, 2) + "\n";
    gameshark += "8107ECA2 " + col11.substr(4, 2) + "00\n";
    gameshark += "8107EC98 " + col12.substr(0, 2) + col12.substr(2, 2) + "\n";
    if (!cc_spark_support) {
        gameshark += "8107EC9A " + col12.substr(4, 2) + "00";
    } else {
        gameshark += "8107EC9A " + col12.substr(4, 2) + "00\n";

        std::string col13 = formatColumn(sparkColorShirt, 0);
        std::string col14 = formatColumn(sparkColorShirt, 1);

        std::string col15 = formatColumn(sparkColorShoulders, 0);
        std::string col16 = formatColumn(sparkColorShoulders, 1);

        std::string col17 = formatColumn(sparkColorArms, 0);
        std::string col18 = formatColumn(sparkColorArms, 1);

        std::string col19 = formatColumn(sparkColorOverallsBottom, 0);
        std::string col20 = formatColumn(sparkColorOverallsBottom, 1);

        std::string col21 = formatColumn(sparkColorLegTop, 0);
        std::string col22 = formatColumn(sparkColorLegTop, 1);

        std::string col23 = formatColumn(sparkColorLegBottom, 0);
        std::string col24 = formatColumn(sparkColorLegBottom, 1);

        gameshark += "8107ECB8 " + col13.substr(0, 2) + col13.substr(2, 2) + "\n";
        gameshark += "8107ECBA " + col13.substr(4, 2) + "00\n";
        gameshark += "8107ECB0 " + col14.substr(0, 2) + col14.substr(2, 2) + "\n";
        gameshark += "8107ECB2 " + col14.substr(4, 2) + "00\n";
        gameshark += "8107ECD0 " + col15.substr(0, 2) + col15.substr(2, 2) + "\n";
        gameshark += "8107ECD2 " + col15.substr(4, 2) + "00\n";
        gameshark += "8107ECC8 " + col16.substr(0, 2) + col16.substr(2, 2) + "\n";
        gameshark += "8107ECCA " + col16.substr(4, 2) + "00\n";
        gameshark += "8107ECE8 " + col17.substr(0, 2) + col17.substr(2, 2) + "\n";
        gameshark += "8107ECEA " + col17.substr(4, 2) + "00\n";
        gameshark += "8107ECE0 " + col18.substr(0, 2) + col18.substr(2, 2) + "\n";
        gameshark += "8107ECE2 " + col18.substr(4, 2) + "00\n";
        gameshark += "8107ED00 " + col19.substr(0, 2) + col19.substr(2, 2) + "\n";
        gameshark += "8107ED02 " + col19.substr(4, 2) + "00\n";
        gameshark += "8107ECF8 " + col20.substr(0, 2) + col20.substr(2, 2) + "\n";
        gameshark += "8107ECFA " + col20.substr(4, 2) + "00\n";
        gameshark += "8107ED18 " + col21.substr(0, 2) + col21.substr(2, 2) + "\n";
        gameshark += "8107ED1A " + col21.substr(4, 2) + "00\n";
        gameshark += "8107ED10 " + col22.substr(0, 2) + col22.substr(2, 2) + "\n";
        gameshark += "8107ED12 " + col22.substr(4, 2) + "00\n";
        gameshark += "8107ED30 " + col23.substr(0, 2) + col23.substr(2, 2) + "\n";
        gameshark += "8107ED32 " + col23.substr(4, 2) + "00\n";
        gameshark += "8107ED28 " + col24.substr(0, 2) + col24.substr(2, 2) + "\n";
        gameshark += "8107ED2A " + col24.substr(4, 2) + "00";
    }

    return gameshark;
}

/*
    Returns true if a defined address is for the CometSPARK format.
*/
bool is_spark_address(string address) {
    // The unholy address table
    if (address == "07ECB8" || address == "07ECBA" || address == "07ECB0" || address == "07ECB2")
        return true;
    if (address == "07ECD0" || address == "07ECD2" || address == "07ECC8" || address == "07ECCA")
        return true;
    if (address == "07ECE8" || address == "07ECEA" || address == "07ECE0" || address == "07ECE2")
        return true;
    if (address == "07ED00" || address == "07ED02" || address == "07ECF8"|| address == "07ECFA")
        return true;
    if (address == "07ED18" || address == "07ED1A" || address == "07ED10" || address == "07ED12")
        return true;
    if (address == "07ED30" || address == "07ED32" || address == "07ED28" || address == "07ED2A")
        return true;

    // Nope
    return false;
}

/*
    Loads the dynos/colorcodes/ folder into the CC array.
*/
void saturn_load_cc_directory() {
    cc_array.clear();
    cc_array.push_back("Mario.gs");

#ifdef __MINGW32__
    // windows moment
    colorCodeDir = "dynos\\colorcodes\\";
#else
    colorCodeDir = "dynos/colorcodes/";
#endif

    if (!fs::exists(colorCodeDir))
        return;

    for (const auto & entry : fs::directory_iterator(colorCodeDir)) {
        fs::path path = entry.path();

        if (path.filename().u8string() != "Mario") {
            if (path.extension().u8string() == ".gs")
                cc_array.push_back(path.filename().u8string());
        }
    }
}

/*
    Preferably used in a while loop, sets a global color with a defined address and value.
*/

// NOTE:    Past this point I only changed the functions to work with the new system.
//          Look at it only if you dare.
void run_cc_replacement(string address, int value1, int value2) {
    // Hat
    if (address == "07EC40") {
        defaultColorHat[0][0] = value1;
        defaultColorHat[1][0] = value2;
    }
    if (address == "07EC42") {
        defaultColorHat[2][0] = value1;
    }
    if (address == "07EC38") {
        defaultColorHat[0][1] = value1;
        defaultColorHat[1][1] = value2;
    }
    if (address == "07EC3A") {
        defaultColorHat[2][1] = value1;
    }

    // Overalls
    if (address == "07EC28") {
        defaultColorOveralls[0][0] = value1;
        defaultColorOveralls[1][0] = value2;
    }
    if (address == "07EC2A") {
        defaultColorOveralls[2][0] = value1;
    }
    if (address == "07EC20") {
        defaultColorOveralls[0][1] = value1;
        defaultColorOveralls[1][1] = value2;
    }
    if (address == "07EC22") {
        defaultColorOveralls[2][1] = value1;
    }

    // Gloves
    if (address == "07EC58") {
        defaultColorGloves[0][0] = value1;
        defaultColorGloves[1][0] = value2;
    }
    if (address == "07EC5A") {
        defaultColorGloves[2][0] = value1;
    }
    if (address == "07EC50") {
        defaultColorGloves[0][1] = value1;
        defaultColorGloves[1][1] = value2;
    }
    if (address == "07EC52") {
        defaultColorGloves[2][1] = value1;
    }

    // Shoes
    if (address == "07EC70") {
        defaultColorShoes[0][0] = value1;
        defaultColorShoes[1][0] = value2;
    }
    if (address == "07EC72") {
        defaultColorShoes[2][0] = value1;
    }
    if (address == "07EC68") {
        defaultColorShoes[0][1] = value1;
        defaultColorShoes[1][1] = value2;
    }
    if (address == "07EC6A") {
        defaultColorShoes[2][1] = value1;
    }

    // Skin
    if (address == "07EC88") {
        defaultColorSkin[0][0] = value1;
        defaultColorSkin[1][0] = value2;
    }
    if (address == "07EC8A") {
        defaultColorSkin[2][0] = value1;
    }
    if (address == "07EC80") {
        defaultColorSkin[0][1] = value1;
        defaultColorSkin[1][1] = value2;
    }
    if (address == "07EC82") {
        defaultColorSkin[2][1] = value1;
    }

    // Hair
    if (address == "07ECA0") {
        defaultColorHair[0][0] = value1;
        defaultColorHair[1][0] = value2;
    }
    if (address == "07ECA2") {
        defaultColorHair[2][0] = value1;
    }
    if (address == "07EC98") {
        defaultColorHair[0][1] = value1;
        defaultColorHair[1][1] = value2;
    }
    if (address == "07EC9A") {
        defaultColorHair[2][1] = value1;
    }

    // --------
    // EXPERIMENTAL: CometSPARK Support
    // --------
    // Things MIGHT break. This is unfinished and not fully compatible with Saturn yet.
    // Pull requests and detailed GH issues are much appreciated...
    // --------

    if (!cc_spark_support)
        return;

    // Shirt
    if (address == "07ECB8") {
        sparkColorShirt[0][0] = value1;
        sparkColorShirt[1][0] = value2;
    }
    if (address == "07ECBA") {
        sparkColorShirt[2][0] = value1;
    }
    if (address == "07ECB0") {
        sparkColorShirt[0][1] = value1;
        sparkColorShirt[1][1] = value2;
    }
    if (address == "07ECB2") {
        sparkColorShirt[2][1] = value1;
    }

    // Shoulders
    if (address == "07ECD0") {
        sparkColorShoulders[0][0] = value1;
        sparkColorShoulders[1][0] = value2;
    }
    if (address == "07ECD2") {
        sparkColorShoulders[2][0] = value1;
    }
    if (address == "07ECC8") {
        sparkColorShoulders[0][1] = value1;
        sparkColorShoulders[1][1] = value2;
    }
    if (address == "07ECCA") {
        sparkColorShoulders[2][1] = value1;
    }

    // Arms
    if (address == "07ECE8") {
        sparkColorArms[0][0] = value1;
        sparkColorArms[1][0] = value2;
    }
    if (address == "07ECEA") {
        sparkColorArms[2][0] = value1;
    }
    if (address == "07ECE0") {
        sparkColorArms[0][1] = value1;
        sparkColorArms[1][1] = value2;
    }
    if (address == "07ECE2") {
        sparkColorArms[2][1] = value1;
    }

    // OverallsBottom
    if (address == "07ED00") {
        sparkColorOverallsBottom[0][0] = value1;
        sparkColorOverallsBottom[1][0] = value2;
    }
    if (address == "07ED02") {
        sparkColorOverallsBottom[2][0] = value1;
    }
    if (address == "07ECF8") {
        sparkColorOverallsBottom[0][1] = value1;
        sparkColorOverallsBottom[1][1] = value2;
    }
    if (address == "07ECFA") {
        sparkColorOverallsBottom[2][1] = value1;
    }

    // LegTop
    if (address == "07ED18") {
        sparkColorLegTop[0][0] = value1;
        sparkColorLegTop[1][0] = value2;
    }
    if (address == "07ED1A") {
        sparkColorLegTop[2][0] = value1;
    }
    if (address == "07ED10") {
        sparkColorLegTop[0][1] = value1;
        sparkColorLegTop[1][1] = value2;
    }
    if (address == "07ED12") {
        sparkColorLegTop[2][1] = value1;
    }

    // LegBottom
    if (address == "07ED30") {
        sparkColorLegBottom[0][0] = value1;
        sparkColorLegBottom[1][0] = value2;
    }
    if (address == "07ED32") {
        sparkColorLegBottom[2][0] = value1;
    }
    if (address == "07ED28") {
        sparkColorLegBottom[0][1] = value1;
        sparkColorLegBottom[1][1] = value2;
    }
    if (address == "07ED2A") {
        sparkColorLegBottom[2][1] = value1;
    }
}

/*
    Resets Mario's colors to default.
*/
void reset_colors(std::array<std::array<unsigned int, 2>, 3> &ccColor, int val1, int val2, int val3, int val4, int val5, int val6) {
    ccColor[0][0] = val1;
    ccColor[0][1] = val2;

    ccColor[1][0] = val3;
    ccColor[1][1] = val4;

    ccColor[2][0] = val5;
    ccColor[2][1] = val6;
}
// Please save me

void reset_cc_colors() {
    reset_colors(defaultColorHat, 255, 127, 0, 0, 0, 0);
    reset_colors(defaultColorOveralls, 0, 0, 0, 0, 255, 127);
    reset_colors(defaultColorGloves, 255, 127, 255, 127, 255, 127);
    reset_colors(defaultColorShoes, 114, 57, 28, 14, 14, 7);
    reset_colors(defaultColorSkin, 254, 127, 193, 96, 121, 60);
    reset_colors(defaultColorHair = 115, 57, 6, 3, 0, 0);

    // CometSPARK
    reset_colors(sparkColorShirt, 255, 127, 255, 127, 0, 0);
    reset_colors(sparkColorShoulders, 0, 0, 255, 127, 255, 127);
    reset_colors(sparkColorArms, 0, 0, 255, 127, 127, 64);
    reset_colors(sparkColorOverallsBottom, 255, 127, 0, 0, 255, 127);
    reset_colors(sparkColorLegTop, 255, 127, 0, 0, 127, 64);
    reset_colors(sparkColorLegBottom, 127, 64, 0, 0, 255, 127);
}

bool is_default_cc(string gameshark) {
    std::string default_cc =    "8107EC40 FF00\n8107EC42 0000\n8107EC38 7F00\n8107EC3A 0000\n8107EC28 0000\n8107EC2A FF00\n8107EC20 0000\n8107EC22 7F00\n8107EC58 FFFF\n8107EC5A FF00\n8107EC50 7F7F\n8107EC52 7F00\n8107EC70 721C\n8107EC72 0E00\n8107EC68 390E\n8107EC6A 0700\n8107EC88 FEC1\n8107EC8A 7900\n8107EC80 7F60\n8107EC82 3C00\n8107ECA0 7306\n8107ECA2 0000\n8107EC98 3903\n8107EC9A 0000";
    std::string default_spark_cc =  "8107EC40 FF00\n8107EC42 0000\n8107EC38 7F00\n8107EC3A 0000\n8107EC28 0000\n8107EC2A FF00\n8107EC20 0000\n8107EC22 7F00\n8107EC58 FFFF\n8107EC5A FF00\n8107EC50 7F7F\n8107EC52 7F00\n8107EC70 721C\n8107EC72 0E00\n8107EC68 390E\n8107EC6A 0700\n8107EC88 FEC1\n8107EC8A 7900\n8107EC80 7F60\n8107EC82 3C00\n8107ECA0 7306\n8107ECA2 0000\n8107EC98 3903\n8107EC9A 0000\n8107ECB8 FFFF\n8107ECBA 0000\n8107ECB0 7F7F\n8107ECB2 0000\n8107ECD0 00FF\n8107ECD2 FF00\n8107ECC8 007F\n8107ECCA 7F00\n8107ECE8 00FF\n8107ECEA 7F00\n8107ECE0 007F\n8107ECE2 4000\n8107ED00 FF00\n8107ED02 FF00\n8107ECF8 7F00\n8107ECFA 7F00\n8107ED18 FF00\n8107ED1A 7F00\n8107ED10 7F00\n8107ED12 4000\n8107ED30 7F00\n8107ED32 FF00\n8107ED28 4000\n8107ED2A 7F00";

    if (gameshark == default_cc || gameshark == default_spark_cc)
        return true;

    return false;
}

/*
    Sets Mario's colors using a GameShark code string.
*/
void paste_gs_code(string content) {
    std::istringstream f(content);
    std::string line;
        
    while (std::getline(f, line)) {
        std::string address = line.substr(2, 6);
        int value1 = std::stoi(line.substr(9, 2), 0, 16);
        int value2 = std::stoi(line.substr(11, 2), 0, 16);

        run_cc_replacement(address, value1, value2);
    }
}

/*
    Saves the global_gs_code() to a GS file.
*/
void save_cc_file(std::string name, std::string gameshark) {
#ifdef __MINGW32__
    std::ofstream file("dynos\\colorcodes\\" + name + ".gs");
#else
    std::ofstream file("dynos/colorcodes/" + name + ".gs");
#endif
    file << gameshark;
}

/*
    Saves the global_gs_code() to a GS file.
*/
void save_cc_model_file(std::string name, std::string gameshark, std::string modelFolder) {
    // Create colorcodes folder if it doesn't already exist
    fs::create_directory("dynos/packs/" + modelFolder + "/colorcodes");

#ifdef __MINGW32__
    std::ofstream file("dynos\\packs\\" + modelFolder + "\\colorcodes\\" + name + ".gs");
#else
    std::ofstream file("dynos/packs/" + modelFolder + "/colorcodes/" + name + ".gs");
#endif
    file << gameshark;
}

/*
    Loads a GS file and sets Mario's colors.
*/
void load_cc_file(char* cc_char_filename) {
    string cc_filename = cc_char_filename;
    if (cc_filename == "Mario.gs") {
        reset_cc_colors();
        return;
    }

    std::ifstream file(colorCodeDir + cc_filename, std::ios::in | std::ios::binary);

    // If the color code was previously deleted, reload the list and cancel.
    if (!file.good()) {
        saturn_load_cc_directory();
        return;
    }

    const std::size_t& size = std::filesystem::file_size(colorCodeDir + cc_filename);
    std::string content(size, '\0');
    file.read(content.data(), size);

    file.close();

    paste_gs_code(content);
    modelCcLoaded = false;
}

/*
    Deletes a GS file.
*/
void delete_cc_file(std::string name) {
    if (name == "Mario")
        name = "Sample";

#ifdef __MINGW32__
    string cc_path = "dynos\\colorcodes\\" + name + ".gs";
#else
    string cc_path = "dynos/colorcodes/" + name + ".gs";
#endif

    remove(cc_path.c_str());
    saturn_load_cc_directory();
}

void delete_model_cc_file(std::string name, std::string modelFolder) {
    if (name == "Mario")
        name = "Sample";

#ifdef __MINGW32__
    string cc_path = "dynos\\packs\\" + modelFolder + "\\colorcodes\\" + name + ".gs";
#else
    string cc_path = "dynos/packs/" + modelFolder + "/colorcodes/" + name + ".gs";
#endif

    remove(cc_path.c_str());
    saturn_load_cc_directory();
}

void get_ccs_from_model(std::string modelPath) {
    model_cc_array.clear();

    // Old path
    if (fs::exists(modelPath + "\\default.gs")) {
        model_cc_array.push_back("../default.gs");
    }

#ifdef __MINGW32__
    // windows moment
    modelColorCodeDir = modelPath + "\\colorcodes\\";
#else
    modelColorCodeDir = modelPath + "/colorcodes/";
#endif

    if (!fs::exists(modelColorCodeDir))
        return;

    for (const auto & entry : fs::directory_iterator(modelColorCodeDir)) {
        fs::path path = entry.path();

        if (path.filename().u8string() != "Mario") {
            if (path.extension().u8string() == ".gs")
                model_cc_array.push_back(path.filename().u8string());
        }
    }
}

/*
    Sets Mario's colors using a defined model path.
*/
void set_cc_from_model(std::string ccPath) {
    std::ifstream file(ccPath, std::ios::in | std::ios::binary);

    if (!file.good()) {
        // No file exists, cancel
        return;
    }

    const std::size_t& size = std::filesystem::file_size(ccPath);
    std::string content(size, '\0');
    file.read(content.data(), size);
    file.close();

    std::istringstream f(content);
    std::string line;
        
    while (std::getline(f, line)) {
        std::string address = line.substr(2, 6);
        int value1 = std::stoi(line.substr(9, 2), 0, 16);
        int value2 = std::stoi(line.substr(11, 2), 0, 16);

        if (!cc_spark_support) cc_spark_support = is_spark_address(address);
        run_cc_replacement(address, value1, value2);
    }
}
