#include "application.h"


int main(int argc, char** argv)
{

    setlocale(LC_ALL, "Russian");

    std::string rom_path = "roms/metroid.gb";
    if (argc == 2) rom_path = argv[1];

    application app(rom_path);

    app.init();
    app.start_loop();
    app.close();

    return 0;
}