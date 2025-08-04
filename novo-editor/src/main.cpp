#include "Application.hpp"

int main(int argc, char const *argv[]) {
    Application app;
    app.init();
    app.on_update();
    return 0;
}
