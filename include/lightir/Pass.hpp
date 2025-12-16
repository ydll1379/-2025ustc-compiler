#pragma once

#include "Module.hpp"
#include "Function.hpp"
#include "BasicBlock.hpp"
#include "Instruction.hpp"

#include <vector>
#include <unordered_set>
#include <memory>

class Pass {
public:
    virtual ~Pass() = default;
    virtual bool run(Module* module) = 0;
    virtual std::string get_name() const = 0;
};

class ModulePass : public Pass {
public:
    virtual bool run(Module* module) override {
        bool changed = false;
        for (auto& func : module->get_functions()) {
            if (!func.is_declaration()) {
                changed |= run_on_function(&func);
            }
        }
        return changed;
    }

    virtual bool run_on_function(Function* func) = 0;
};

class FunctionPass : public Pass {
public:
    virtual bool run(Module* module) override {
        bool changed = false;
        for (auto& func : module->get_functions()) {
            if (!func.is_declaration()) {
                changed |= run_on_function(&func);
            }
        }
        return changed;
    }

    virtual bool run_on_function(Function* func) = 0;
};

// Dead Code Elimination Pass
class DeadCodeElimination : public FunctionPass {
public:
    virtual std::string get_name() const override {
        return "DeadCodeElimination";
    }

    virtual bool run_on_function(Function* func) override;

private:
    // Helper methods
    bool is_side_effect_free(Instruction* instr);
    bool is_critical_instruction(Instruction* instr);
    bool eliminate_dead_code(Function* func);
    bool remove_unreachable_blocks(Function* func);
    void mark_live_values(Function* func, std::unordered_set<Value*>& live_values);
    bool remove_dead_instructions(Function* func, const std::unordered_set<Value*>& live_values);
};