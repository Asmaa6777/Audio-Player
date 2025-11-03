/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   album_jpg;
    const int            album_jpgSize = 12837;

    extern const char*   back10_jpg;
    const int            back10_jpgSize = 2113;

    extern const char*   forward10_jpg;
    const int            forward10_jpgSize = 2085;

    extern const char*   gotoend_jpg;
    const int            gotoend_jpgSize = 2102;

    extern const char*   loop_jpg;
    const int            loop_jpgSize = 2040;

    extern const char*   mute_jpg;
    const int            mute_jpgSize = 2088;

    extern const char*   pause_jpg;
    const int            pause_jpgSize = 2115;

    extern const char*   play_jpg;
    const int            play_jpgSize = 2015;

    extern const char*   restart_jpg;
    const int            restart_jpgSize = 2112;

    extern const char*   unmute_jpg;
    const int            unmute_jpgSize = 2089;

    extern const char*   AudioGUI_cpp;
    const int            AudioGUI_cppSize = 276;

    extern const char*   AudioGUI_h;
    const int            AudioGUI_hSize = 925;

    extern const char*   Main_cpp;
    const int            Main_cppSize = 1186;

    extern const char*   MainComponent_cpp;
    const int            MainComponent_cppSize = 5433;

    extern const char*   MainComponent_h;
    const int            MainComponent_hSize = 1728;

    extern const char*   PlayerAudio_cpp;
    const int            PlayerAudio_cppSize = 4441;

    extern const char*   PlayerAudio_h;
    const int            PlayerAudio_hSize = 1895;

    extern const char*   PlayerGUI_cpp;
    const int            PlayerGUI_cppSize = 14177;

    extern const char*   PlayerGUI_h;
    const int            PlayerGUI_hSize = 3095;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 19;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
