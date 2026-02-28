#include "debug.h"

debug::debug() : execution(false), logging(false), instr_count(0), show_memory_map(false)
{
    last_time = std::chrono::steady_clock::now();
}

void debug::draw_oam_menu()
{
	oam& _oam = oam::instance();
    std::stringstream log;

	ImGui::Begin("OAM");

    ImGui::BeginTable("OAM", 4);
    for(uint16_t i = 0; i < 160; i+=4)
    {
        uint8_t Y = _oam.get_value(0xFE00 + i), X = _oam.get_value(0xFE00 + i + 1),
            Pattern = _oam.get_value(0xFE00 + i + 2), Flags = _oam.get_value(0xFE00 + i + 3);

        ImGui::TableNextColumn();
        ImGui::Text("Y: %X", Y);
        ImGui::TableNextColumn();
        ImGui::Text("X: %X", X);
        ImGui::TableNextColumn();
        ImGui::Text("Pattern: %X", Pattern);
        ImGui::TableNextColumn();
        ImGui::Text("Flags: %X", Flags);

        log << "Y:" << Y << "\tX: " << X 
            << "\tPattern: " << Pattern << "\tFlags: " << Flags << std::endl;
	}
    ImGui::EndTable();

    ImGui::PushID(2);
    if(ImGui::BeginPopupContextItem("OAM menu"))
    {
		if (ImGui::Selectable("Copy")) ImGui::SetClipboardText(log.str().c_str());
        ImGui::EndPopup();
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        ImGui::OpenPopup("OAM menu");

    ImGui::PopID();

	ImGui::End();
}


void debug::copy_log_to_clipboard()
{
    std::stringstream log;
    for(auto& i : debug::instruction_log)
    {
        log << "Instr num: " << i.instr_num << '\t' << i.instr << '\t' << i.cpu_stat << '\t' << i.gpu_stat << std::endl;
    }
    ImGui::SetClipboardText(log.str().c_str());
}

void debug::draw_memory_map(cartridge& cart)
{
    ImGui::Begin("MemoryMap");
    
    ImGui::BeginTable("MMTable", 17);

    ImGui::TableNextColumn();
    ImGui::Text("\t");
    for(int i = 0; i < 16; i++)
    {
        ImGui::TableNextColumn();
        ImGui::Text("%X\t", i);
    }
    
    for(uint32_t address = 0; address <= 0xFFFF; address += 16)
    {
        ImGui::TableNextColumn();
        ImGui::Text("%X\t", address);

        for (int i = 0; i < 16; i++)
        {
            ImGui::TableNextColumn();
            ImGui::Text("%X\t", read_byte(address + i, cart));
        }
    }
    ImGui::EndTable();

    ImGui::End();
}

float debug::calculate_delta_time()
{
    auto end_time = std::chrono::steady_clock::now();

    float delta_time = std::chrono::duration<float>(end_time - last_time).count();
    last_time = end_time;

    return delta_time;
}

void debug::log(cartridge& cart)
{
    cpu& cp = cpu::instance();
    gpu& gp = gpu::instance();
    instruction_set& is = instruction_set::instance();

    instruction_log.push_back({ instr_count + 1,
                    is.get_instruction(read_byte(cp.get_pc(), cart)).mnemonic, cp.debug(), gp.debug() });
}

void debug::draw_cpu_menu()
{
    cpu& cp = cpu::instance();

    uint8_t A = cp.get_a(), F = cp.get_f(), 
            B = cp.get_b(), C = cp.get_c(), 
            D = cp.get_d(), E = cp.get_e(), 
            H = cp.get_h(), L = cp.get_l();

    uint16_t BC = cp.get_bc(), DE = cp.get_de(), HL = cp.get_hl();

    uint8_t min_val = 0, max_val = 255;
    uint16_t min_val_u16 = 0, max_val_u16 = ~0;

    ImGui::Begin("CPU");

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 10));
    if (ImGui::BeginTable("Table", 2))
    {
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("A", ImGuiDataType_U8, &A, &min_val, &max_val, "%X"))
            cp.set_a(A);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("F", ImGuiDataType_U8, &F, &min_val, &max_val, "%X"))
            cp.set_f(F);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("B", ImGuiDataType_U8, &B, &min_val, &max_val, "%X"))
            cp.set_b(B);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("C", ImGuiDataType_U8, &C, &min_val, &max_val, "%X"))
            cp.set_c(C);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("D", ImGuiDataType_U8, &D, &min_val, &max_val, "%X"))
            cp.set_d(D);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("E", ImGuiDataType_U8, &E, &min_val, &max_val, "%X"))
            cp.set_e(E);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("H", ImGuiDataType_U8, &H, &min_val, &max_val, "%X"))
            cp.set_h(H);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("L", ImGuiDataType_U8, &L, &min_val, &max_val, "%X"))
            cp.set_l(L);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("BC", ImGuiDataType_U16, &BC, &min_val_u16, &max_val_u16, "%X"))
            cp.set_bc(BC);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("DE", ImGuiDataType_U16, &DE, &min_val_u16, &max_val_u16, "%X"))
            cp.set_de(DE);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("HL", ImGuiDataType_U16, &HL, &min_val_u16, &max_val_u16, "%X"))
            cp.set_hl(HL);
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImGui::End();

}

void debug::draw_gpu_menu()
{
    gpu& gp = gpu::instance();

    uint8_t LCDC = gp.get_register(0xFF40), STAT = gp.get_register(0xFF41), 
            LY = gp.get_register(0xFF44), LYC = gp.get_register(0xFF45), 
            SCX = gp.get_register(0xFF43), SCY = gp.get_register(0xFF42);

    uint8_t min_val = 0, max_val = 255;

    ImGui::Begin("GPU");

    if (ImGui::BeginTable("Table", 2)) 
    {
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("LCDC", ImGuiDataType_U8, &LCDC, &min_val, &max_val, "%X"))
            gp.set_register(0xFF40, LCDC);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("STAT", ImGuiDataType_U8, &STAT, &min_val, &max_val, "%X"))
            gp.set_register(0xFF41, STAT);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("LY", ImGuiDataType_U8, &LY, &min_val, &max_val, "%X"))
            gp.set_register(0xFF44, LY);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("LYC", ImGuiDataType_U8, &LYC, &min_val, &max_val, "%X"))
            gp.set_register(0xFF45, LYC);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("SCX", ImGuiDataType_U8, &SCX, &min_val, &max_val, "%X"))
            gp.set_register(0xFF43, SCX);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("SCY", ImGuiDataType_U8, &SCY, &min_val, &max_val, "%X"))
            gp.set_register(0xFF42, SCY);
        ImGui::EndTable();
    }
    ImGui::End();
}

unsigned long debug::menu(cartridge& cart)
{
    float delta_time = calculate_delta_time();

    unsigned long instr_to_execute = 0;
    unsigned long _instr_to_execute = 0;

    interrupt_registers& ir = interrupt_registers::instance();
    cpu& cp = cpu::instance();

    uint8_t IF = ir.get_if(), IE = ir.get_ie();
    uint16_t PC = cp.get_pc(), SP = cp.get_sp();
	uint16_t IME = cp.get_ime();

    uint8_t min_val = 0, max_val = 255;
    uint16_t min_val_u16 = 0, max_val_u16 = ~0;

    std::string next_instr_name = instruction_set::instance().get_instruction(read_byte(cp.get_pc(), cart)).mnemonic;
    std::string text = "Next instruction: " + next_instr_name;
    std::string instr_count_text = "Executed instr: " + std::to_string(debug::instr_count);

    ImGui::Begin("Debug");
    
    ImGui::Text("Frame time: %.3fs", delta_time);
    ImGui::Text(instr_count_text.c_str());
    ImGui::Text(text.c_str());

    if (ImGui::BeginTable("Table", 2))
    {
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("IF", ImGuiDataType_U8, &IF, &min_val, &max_val, "%X"))
            ir.set_if(IF);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("IE", ImGuiDataType_U8, &IE, &min_val, &max_val, "%X"))
            ir.set_ie(IE);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("PC", ImGuiDataType_U16, &PC, &min_val_u16, &max_val_u16, "%X"))
            cp.set_pc(PC);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("SP", ImGuiDataType_U16, &SP, &min_val_u16, &max_val_u16, "%X"))
            cp.set_sp(SP);
        ImGui::TableNextColumn();
        if (ImGui::InputScalar("IME", ImGuiDataType_U16, &IME, &min_val_u16, &max_val_u16, "%X"))
            cp.set_ime(IME);
        ImGui::EndTable();
    }

    ImGui::InputScalar("Execute instr", ImGuiDataType_U32, &_instr_to_execute);
        
    if (ImGui::IsItemDeactivatedAfterEdit())
        instr_to_execute = _instr_to_execute;

    if (ImGui::Button("execute next instruction"))
        instr_to_execute = 1;

	ImGui::Spacing();

	std::string stop_text = !execution ? "Execution was stopped" : "Execution...";
	ImGui::Text(stop_text.c_str());

    if (ImGui::Button("Continue")) execution = true;
	ImGui::SameLine();
    if (ImGui::Button("Stop")) execution = false;

    ImGui::Checkbox("Enable logging", &logging);
    if (logging) 
    {
        log(cart);

        ImGui::SameLine();
        if (ImGui::Button("Copy log to clipboard")) copy_log_to_clipboard();
    }

    ImGui::Checkbox("Show memory map", &show_memory_map);
    if(show_memory_map) draw_memory_map(cart);
    ImGui::End();

    draw_cpu_menu();
    draw_gpu_menu();
    draw_oam_menu();

    if (execution) instr_to_execute = 1;

    return instr_to_execute;
}
