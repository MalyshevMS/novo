#include <novo-core/Window.hpp>

int main(int argc, char const *argv[]) {
    Novo::Window window = Novo::Window("Hello World", {800, 600});

    while (!window.shouldClose()) {
        window.update();
    }
    return 0;
}
