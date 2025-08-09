#include "Application.hpp"

int main(int argc, char const *argv[]) {
    NovoEditor app;
    app.init();
    app.on_update();
    return 0;
}
