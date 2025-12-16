#include "lightir/Pass.hpp"
#include <queue>
#include <algorithm>

using std::unordered_set;

bool DeadCodeElimination::run_on_function(Function* func) {
    bool changed = false;

    // Perform multiple iterations until no more changes
    bool local_changed;
    do {
        local_changed = false;

        // Remove unreachable basic blocks
        local_changed |= remove_unreachable_blocks(func);

        // Eliminate dead instructions
        local_changed |= eliminate_dead_code(func);

        changed |= local_changed;
    } while (local_changed);

    return changed;
}

bool DeadCodeElimination::is_side_effect_free(Instruction* instr) {
    // Instructions with side effects that cannot be removed:
    // - store (modifies memory)
    // - call (may have side effects)
    // - ret (terminator)
    // - br (terminator)
    // - alloca (allocates memory)

    if (instr->is_store() || instr->is_call() || instr->is_ret() ||
        instr->is_br() || instr->is_alloca()) {
        return false;
    }

    // Other instructions are generally side-effect free
    return true;
}

bool DeadCodeElimination::is_critical_instruction(Instruction* instr) {
    // Critical instructions that should always be kept:
    // - Terminators (ret, br)
    // - Instructions with side effects (store, call with side effects)
    // - Alloca (stack allocation)

    if (instr->is_ret() || instr->is_br() || instr->is_store() ||
        instr->is_alloca()) {
        return true;
    }

    // Call instructions might be critical if they have side effects
    // For simplicity, we treat all calls as critical
    if (instr->is_call()) {
        return true;
    }

    return false;
}

bool DeadCodeElimination::eliminate_dead_code(Function* func) {
    std::unordered_set<Value*> live_values;

    // Mark live values
    mark_live_values(func, live_values);

    // Remove dead instructions
    return remove_dead_instructions(func, live_values);
}

void DeadCodeElimination::mark_live_values(Function* func, std::unordered_set<Value*>& live_values) {
    std::queue<Value*> worklist;

    // Initialize worklist with values that are inherently live:
    // 1. Function arguments
    for (auto& arg : func->get_args()) {
        worklist.push(&arg);
        live_values.insert(&arg);
    }

    // 2. Instructions with side effects (store, call, etc.)
    // 3. Terminator instructions
    for (auto& bb : func->get_basic_blocks()) {
        for (auto& instr : bb.get_instructions()) {
            if (!is_side_effect_free(&instr)) {
                worklist.push(&instr);
                live_values.insert(&instr);
            }

            // Also mark the operands of critical instructions
            if (is_critical_instruction(&instr)) {
                for (auto* op : instr.get_operands()) {
                    worklist.push(op);
                    live_values.insert(op);
                }
            }
        }
    }

    // Propagate liveness backwards through use-def chains
    while (!worklist.empty()) {
        Value* val = worklist.front();
        worklist.pop();

        // For each use of this value, mark the user as live
        for (auto& use : val->get_use_list()) {
            User* user = use.val_;
            if (live_values.find(user) == live_values.end()) {
                live_values.insert(user);
                worklist.push(user);
            }
        }
    }
}

bool DeadCodeElimination::remove_dead_instructions(Function* func, const std::unordered_set<Value*>& live_values) {
    bool changed = false;
    // Iterate through all basic blocks and instructions
    for (auto& bb : func->get_basic_blocks()) {
        auto& instr_list = bb.get_instructions();
        auto it = instr_list.begin();

        while (it != instr_list.end()) {
            Instruction* instr = &(*it);

            // Skip critical instructions and live values
            if (is_critical_instruction(instr) || live_values.find(instr) != live_values.end()) {
                ++it;
                continue;
            }

            // Check if this instruction is side-effect free and dead
            if (is_side_effect_free(instr) && live_values.find(instr) == live_values.end()) {
                // Remove the instruction
                it = instr_list.erase(it);
                changed = true;

                // Remove this instruction from the use lists of its operands
                // The instruction's destructor will call remove_all_operands()
            } else {
                ++it;
            }
        }
    }
    return changed;
}

bool DeadCodeElimination::remove_unreachable_blocks(Function* func) {
    if (func->is_declaration()) {
        return false;
    }

    std::unordered_set<BasicBlock*> reachable;
    std::queue<BasicBlock*> worklist;

    // Start from entry block
    BasicBlock* entry = func->get_entry_block();
    worklist.push(entry);
    reachable.insert(entry);

    // Breadth-first search to find all reachable blocks
    while (!worklist.empty()) {
        BasicBlock* bb = worklist.front();
        worklist.pop();

        // Add successors to worklist
        for (auto* succ : bb->get_succ_basic_blocks()) {
            if (reachable.find(succ) == reachable.end()) {
                reachable.insert(succ);
                worklist.push(succ);
            }
        }
    }

    // Remove unreachable blocks
    auto& bb_list = func->get_basic_blocks();
    auto it = bb_list.begin();
    bool changed = false;

    while (it != bb_list.end()) {
        BasicBlock* bb = &(*it);
        if (reachable.find(bb) == reachable.end()) {
            // Remove the block from its predecessors' successor lists
            for (auto* pred : bb->get_pre_basic_blocks()) {
                pred->remove_succ_basic_block(bb);
            }

            // Remove the block from its successors' predecessor lists
            for (auto* succ : bb->get_succ_basic_blocks()) {
                succ->remove_pre_basic_block(bb);
            }

            // Erase the block
            it = bb_list.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }
    return changed;
}