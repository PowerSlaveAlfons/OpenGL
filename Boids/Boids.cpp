#include <iostream>

#include "Renderer.h" // Graphics Implementation


int main() {

    Renderer& gService = Renderer::getInstance();



    gService.initialize();
    gService.loadModel("Hammer.obj");
    gService.run();


    exit(EXIT_SUCCESS);
}
