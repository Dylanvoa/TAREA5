#pragma once
#include <iostream>
#include <memory>

// ---------- Productos abstractos ----------
class Window {
public:
    virtual ~Window() = default;
    virtual void draw() const = 0;
};

class Button {
public:
    virtual ~Button() = default;
    virtual void draw() const = 0;
};

class Shader {
public:
    virtual ~Shader() = default;
    virtual void compile() const = 0;
};

// ---------- Fabrica abstracta ----------
class RenderFactory {
public:
    virtual ~RenderFactory() = default;
    virtual std::unique_ptr<Window> createWindow() const = 0;
    virtual std::unique_ptr<Button> createButton() const = 0;
    virtual std::unique_ptr<Shader> createShader() const = 0;
};

// ---------- Familia OpenGL ----------
class OpenGLWindow : public Window {
public:
    void draw() const override { std::cout << "[OpenGL] Ventana: glfwCreateWindow + contexto GL\n"; }
};
class OpenGLButton : public Button {
public:
    void draw() const override { std::cout << "[OpenGL] Boton: quad dibujado con glDrawArrays\n"; }
};
class OpenGLShader : public Shader {
public:
    void compile() const override { std::cout << "[OpenGL] Shader: GLSL compilado con glCompileShader\n"; }
};
class OpenGLFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() const override { return std::make_unique<OpenGLWindow>(); }
    std::unique_ptr<Button> createButton() const override { return std::make_unique<OpenGLButton>(); }
    std::unique_ptr<Shader> createShader() const override { return std::make_unique<OpenGLShader>(); }
};

// ---------- Familia Vulkan ----------
class VulkanWindow : public Window {
public:
    void draw() const override { std::cout << "[Vulkan] Ventana: VkSurfaceKHR + swapchain\n"; }
};
class VulkanButton : public Button {
public:
    void draw() const override { std::cout << "[Vulkan] Boton: command buffer con vkCmdDraw\n"; }
};
class VulkanShader : public Shader {
public:
    void compile() const override { std::cout << "[Vulkan] Shader: GLSL -> SPIR-V, vkCreateShaderModule\n"; }
};
class VulkanFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() const override { return std::make_unique<VulkanWindow>(); }
    std::unique_ptr<Button> createButton() const override { return std::make_unique<VulkanButton>(); }
    std::unique_ptr<Shader> createShader() const override { return std::make_unique<VulkanShader>(); }
};
