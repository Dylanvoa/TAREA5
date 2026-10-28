#include "render.hpp"

// El cliente solo conoce RenderFactory y las interfaces de los productos.
void renderScene(const RenderFactory& factory) {
    auto window = factory.createWindow();
    auto button = factory.createButton();
    auto shader = factory.createShader();
    window->draw();
    button->draw();
    shader->compile();
}

int main() {
    std::unique_ptr<RenderFactory> factory = std::make_unique<OpenGLFactory>();

    auto window = factory->createWindow();
    auto button = factory->createButton();
    window->draw();
    button->draw();

    // Cambio de plataforma en tiempo de ejecucion
    factory = std::make_unique<VulkanFactory>();
    auto shader = factory->createShader();
    shader->compile();

    std::cout << "\n--- Escena completa con la fabrica actual (Vulkan) ---\n";
    renderScene(*factory);
}
