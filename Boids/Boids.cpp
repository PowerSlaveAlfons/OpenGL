#include <iostream>

#include "Renderer.h" // Graphics Implementation


int main() {

    Renderer& gService = Renderer::getInstance();

    gService.initialize();
    gService.run();


    exit(EXIT_SUCCESS);
}
