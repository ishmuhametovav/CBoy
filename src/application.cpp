#include "application.h"


application::application(std::string rom_name, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT) : cp(cpu::instance()), gp(gpu::instance()), cart{rom_name},
    tmr(timer::instance()), inpt(input::instance()), m(memory::instance()), ir(interrupt_registers::instance()), window(nullptr), ap(apu::instance()), r(renderer::instance()),
        _oam(oam::instance()), is(instruction_set::instance()), SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), debug_mode(false)
{

}

void application::init_glfw()
{
    if (!glfwInit()) throw std::runtime_error("Cannot init glfw");

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CBoy", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Cannot open window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 60 FPS
    glfwSetKeyCallback(window, input::key_callback);
}

void application::init_glad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Cannot init glad");
    }
}

void application::init_dear_imgui()
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void application::init()
{
    init_glfw();
    init_glad();
    init_dear_imgui();

    //if (!audio.init(44100, 1)) throw std::runtime_error("Cannot init audio");
    //if (!audio.start()) throw std::runtime_error("Cannot start audio engine");

    r.init("./resources/shaders/test.vs", "./resources/shaders/test.fs");
}

void application::loop()
{
    int32_t m_cycles = frequency_per_frame / 4;

    while (m_cycles > 0)
    {
        int32_t opcycles = cp.cycle(cart);
        opcycles = opcycles == 0 ? 4 : opcycles;

        tmr.cycle(opcycles);
        ap.cycle(opcycles);
        gp.cycle(opcycles, cart);
        inpt.cycle();
        m_cycles -= opcycles;

        if (opcycles > 0) debugger.increment_instr_count();
    }

}

void application::loop_debug()
{
    unsigned long instr_to_execute = debugger.menu(cart);

    while (instr_to_execute)
    {
        int32_t opcycles = cp.cycle(cart);
        opcycles = opcycles == 0 ? 4 : opcycles;

        tmr.cycle(opcycles);
        ap.cycle(opcycles);
        gp.cycle(opcycles, cart);
        inpt.cycle();

        if (opcycles > 0) debugger.increment_instr_count();

        instr_to_execute--;
    }
}

void application::start_loop()
{

    while (!glfwWindowShouldClose(window))
    {
        clock.wait_end_of_frame();
        clock.start();

        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_main_menu();

        if(debug_mode) loop_debug();
        else loop();

        gp.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);

        clock.stop();
        clock.calculate_delta_time();
    }
}

void application::close_glfw()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void application::close_dear_imgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void application::close()
{
    close_dear_imgui();
    close_glfw();
}

void application::draw_main_menu()
{
    ImGui::Begin("Main menu");

    ImGui::Checkbox("Debug mode", &debug_mode);

    ImGui::End();
}