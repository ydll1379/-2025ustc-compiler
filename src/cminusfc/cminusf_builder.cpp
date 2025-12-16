#include "cminusf_builder.hpp"
#include "GlobalVariable.hpp"

#define CONST_FP(num) ConstantFP::get((float)num, module.get())
#define CONST_INT(num) ConstantInt::get(num, module.get())

// types
Type *VOID_T;
Type *INT1_T;
Type *INT32_T;
Type *INT32PTR_T;
Type *FLOAT_T;
Type *FLOATPTR_T;

bool promote(IRBuilder *builder, Value **l_val_p, Value **r_val_p) {
    bool is_int = false;
    auto &l_val = *l_val_p;
    auto &r_val = *r_val_p;
    if (l_val->get_type() == r_val->get_type()) {
        is_int = l_val->get_type()->is_integer_type();
    } else {
        if (l_val->get_type()->is_integer_type()) {
            l_val = builder->create_sitofp(l_val, FLOAT_T);
        } else {
            r_val = builder->create_sitofp(r_val, FLOAT_T);
        }
    }
    return is_int;
}

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

Value* CminusfBuilder::visit(ASTProgram &node) {
    VOID_T = module->get_void_type();
    INT1_T = module->get_int1_type();
    INT32_T = module->get_int32_type();
    INT32PTR_T = module->get_int32_ptr_type();
    FLOAT_T = module->get_float_type();
    FLOATPTR_T = module->get_float_ptr_type();

    Value *ret_val = nullptr;
    for (auto &decl : node.declarations) {
        ret_val = decl->accept(*this);
    }
    return ret_val;
}

Value* CminusfBuilder::visit(ASTNum &node) {
    if (node.type == TYPE_INT) {
        return CONST_INT(node.i_val);
    }
    return CONST_FP(node.f_val);
}

Value* CminusfBuilder::visit(ASTVarDeclaration &node) {
    Type *var_type;
    if (node.type == TYPE_INT) {
        var_type = INT32_T;
    } else {
        var_type = FLOAT_T;
    }

    Value *var_val = nullptr;
    if (node.num) {
        // Array declaration
        auto *array_size_val = node.num->accept(*this);
        auto *array_size_const = dynamic_cast<ConstantInt*>(array_size_val);
        assert(array_size_const && "Array size must be constant integer");
        int array_size = array_size_const->get_value();
        auto *array_type = ArrayType::get(var_type, array_size);

        if (scope.in_global()) {
            // Global array
            auto *initializer = ConstantZero::get(array_type, module.get());
            var_val = GlobalVariable::create(node.id, module.get(), array_type, false, initializer);
        } else {
            // Local array: alloca array
            var_val = builder->create_alloca(array_type);
        }
    } else {
        // Scalar variable
        if (scope.in_global()) {
            // Global variable
            auto *initializer = ConstantZero::get(var_type, module.get());
            var_val = GlobalVariable::create(node.id, module.get(), var_type, false, initializer);
        } else {
            // Local variable: alloca
            var_val = builder->create_alloca(var_type);
        }
    }

    scope.push(node.id, var_val);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTFunDeclaration &node) {
    FunctionType *fun_type;
    Type *ret_type;
    std::vector<Type *> param_types;
    if (node.type == TYPE_INT)
        ret_type = INT32_T;
    else if (node.type == TYPE_FLOAT)
        ret_type = FLOAT_T;
    else
        ret_type = VOID_T;

    for (auto &param : node.params) {
        if (param->type == TYPE_INT) {
            if (param->isarray) {
                param_types.push_back(INT32PTR_T);
            } else {
                param_types.push_back(INT32_T);
            }
        } else {
            if (param->isarray) {
                param_types.push_back(FLOATPTR_T);
            } else {
                param_types.push_back(FLOAT_T);
            }
        }
    }

    fun_type = FunctionType::get(ret_type, param_types);
    auto func = Function::create(fun_type, node.id, module.get());
    scope.push(node.id, func);
    context.func = func;
    auto funBB = BasicBlock::create(module.get(), "entry", func);
    builder->set_insert_point(funBB);
    scope.enter();
    context.pre_enter_scope = true;
    std::vector<Value *> args;
    for (auto &arg : func->get_args()) {
        args.push_back(&arg);
    }
    for (unsigned int i = 0; i < node.params.size(); ++i) {
        auto &param = node.params[i];
        Type *param_type;
        if (param->type == TYPE_INT) {
            if (param->isarray) {
                param_type = INT32PTR_T;
            } else {
                param_type = INT32_T;
            }
        } else {
            if (param->isarray) {
                param_type = FLOATPTR_T;
            } else {
                param_type = FLOAT_T;
            }
        }
        // Allocate space for parameter
        auto *alloca = builder->create_alloca(param_type);
        // Store the argument value into the allocated space
        builder->create_store(args[i], alloca);
        // Push the address to scope
        scope.push(param->id, alloca);
    }
    node.compound_stmt->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr) 
    {
        if (context.func->get_return_type()->is_void_type())
            builder->create_void_ret();
        else if (context.func->get_return_type()->is_float_type())
            builder->create_ret(CONST_FP(0.));
        else
            builder->create_ret(CONST_INT(0));
    }
    scope.exit();
    return nullptr;
}

Value* CminusfBuilder::visit(ASTParam &node) {
    return nullptr;
}

Value* CminusfBuilder::visit(ASTCompoundStmt &node) {
    // Enter new scope unless it's the function body's first compound stmt
    bool should_enter_scope = !context.pre_enter_scope;
    if (should_enter_scope) {
        scope.enter();
    }
    context.pre_enter_scope = false;

    for (auto &decl : node.local_declarations) {
        decl->accept(*this);
    }

    for (auto &stmt : node.statement_list) {
        stmt->accept(*this);
        // If current block is terminated (e.g., return, break), stop executing further statements
        if (builder->get_insert_block()->get_terminator() != nullptr)
            break;
    }

    if (should_enter_scope) {
        scope.exit();
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTExpressionStmt &node) {
    if (node.expression != nullptr) {
        return node.expression->accept(*this);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTSelectionStmt &node) {
    auto *ret_val = node.expression->accept(*this);
    auto *trueBB = BasicBlock::create(module.get(), "", context.func);
    BasicBlock *falseBB{};
    auto *contBB = BasicBlock::create(module.get(), "", context.func);
    Value *cond_val = nullptr;
    if (ret_val->get_type()->is_integer_type()) {
        cond_val = builder->create_icmp_ne(ret_val, CONST_INT(0));
    } else {
        cond_val = builder->create_fcmp_ne(ret_val, CONST_FP(0.));
    }

    if (node.else_statement == nullptr) {
        builder->create_cond_br(cond_val, trueBB, contBB);
    } else {
        falseBB = BasicBlock::create(module.get(), "", context.func);
        builder->create_cond_br(cond_val, trueBB, falseBB);
    }
    builder->set_insert_point(trueBB);
    node.if_statement->accept(*this);

    if (not builder->get_insert_block()->is_terminated()) {
        builder->create_br(contBB);
    }

    if (node.else_statement == nullptr) {
        // falseBB->erase_from_parent(); // did not clean up memory
    } else {
        builder->set_insert_point(falseBB);
        node.else_statement->accept(*this);
        if (not builder->get_insert_block()->is_terminated()) {
            builder->create_br(contBB);
        }
    }

    builder->set_insert_point(contBB);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTIterationStmt &node) {
    auto *func = context.func;
    auto *condBB = BasicBlock::create(module.get(), "", func);
    auto *bodyBB = BasicBlock::create(module.get(), "", func);
    auto *endBB = BasicBlock::create(module.get(), "", func);

    // Jump to condition block if current block is not terminated
    if (not builder->get_insert_block()->is_terminated()) {
        builder->create_br(condBB);
    }

    // Condition block
    builder->set_insert_point(condBB);
    auto *cond_val = node.expression->accept(*this);
    Value *bool_cond = nullptr;
    if (cond_val->get_type()->is_integer_type()) {
        bool_cond = builder->create_icmp_ne(cond_val, CONST_INT(0));
    } else {
        bool_cond = builder->create_fcmp_ne(cond_val, CONST_FP(0.));
    }
    builder->create_cond_br(bool_cond, bodyBB, endBB);

    // Loop body
    builder->set_insert_point(bodyBB);
    node.statement->accept(*this);
    // If loop body doesn't terminate (no return), jump back to condition
    if (not builder->get_insert_block()->is_terminated()) {
        builder->create_br(condBB);
    }

    // Continue after loop
    builder->set_insert_point(endBB);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTReturnStmt &node) {
    if (node.expression == nullptr) {
        builder->create_void_ret();
    } else {
        auto *fun_ret_type =
            context.func->get_function_type()->get_return_type();
        auto *ret_val = node.expression->accept(*this);
        if (fun_ret_type != ret_val->get_type()) {
            if (fun_ret_type->is_integer_type()) {
                ret_val = builder->create_fptosi(ret_val, INT32_T);
            } else {
                ret_val = builder->create_sitofp(ret_val, FLOAT_T);
            }
        }

        builder->create_ret(ret_val);
    }

    return nullptr;
}

Value* CminusfBuilder::visit(ASTVar &node) {
    Value *var_addr = scope.find(node.id);
    assert(var_addr && "Variable not found in scope");

    // Handle array indexing if present
    if (node.expression) {
        // Array element access
        auto *index_val = node.expression->accept(*this);
        // index_val should be integer
        if (index_val->get_type()->is_float_type()) {
            index_val = builder->create_fptosi(index_val, INT32_T);
        }
        // Check for negative index
        auto *neg_cmp = builder->create_icmp_lt(index_val, CONST_INT(0));
        auto *cur_bb = builder->get_insert_block();
        auto *func = cur_bb->get_parent();
        auto *error_bb = BasicBlock::create(module.get(), "", func);
        auto *cont_bb = BasicBlock::create(module.get(), "", func);
        builder->create_cond_br(neg_cmp, error_bb, cont_bb);

        builder->set_insert_point(error_bb);
        builder->create_call(static_cast<Function*>(scope.find("neg_idx_except")), {});
        // Return appropriate value based on function return type
        auto *ret_type = context.func->get_return_type();
        if (ret_type->is_void_type()) {
            builder->create_void_ret();
        } else if (ret_type->is_float_type()) {
            builder->create_ret(CONST_FP(0.));
        } else {
            builder->create_ret(CONST_INT(0));
        }

        builder->set_insert_point(cont_bb);
        // Get element pointer
        std::vector<Value*> indices;
        auto *elem_type = var_addr->get_type()->get_pointer_element_type();
        if (elem_type->is_array_type()) {
            // Pointer to array: need two indices [0][index]
            indices.push_back(CONST_INT(0));
            indices.push_back(index_val);
        } else {
            // Pointer to scalar: need one index [index]
            indices.push_back(index_val);
        }
        var_addr = builder->create_gep(var_addr, indices);
    }

    if (context.require_lvalue) {
        context.require_lvalue = false;
        return var_addr;  // Address for storing
    } else {
        // Load value
        return builder->create_load(var_addr);
    }
}

Value* CminusfBuilder::visit(ASTAssignExpression &node) {
    auto *expr_result = node.expression->accept(*this);
    context.require_lvalue = true;
    auto *var_addr = node.var->accept(*this);
    if (var_addr->get_type()->get_pointer_element_type() !=
        expr_result->get_type()) {
        if (expr_result->get_type() == INT32_T) {
            expr_result = builder->create_sitofp(expr_result, FLOAT_T);
        } else {
            expr_result = builder->create_fptosi(expr_result, INT32_T);
        }
    }
    builder->create_store(expr_result, var_addr);
    return expr_result;
}

Value* CminusfBuilder::visit(ASTSimpleExpression &node) {
    auto *l_val = node.additive_expression_l->accept(*this);
    auto *r_val = node.additive_expression_r->accept(*this);
    bool is_int = promote(&*builder, &l_val, &r_val);

    Value *cmp_result = nullptr;
    switch (node.op) {
        case OP_LE:
            if (is_int) {
                cmp_result = builder->create_icmp_le(l_val, r_val);
            } else {
                cmp_result = builder->create_fcmp_le(l_val, r_val);
            }
            break;
        case OP_LT:
            if (is_int) {
                cmp_result = builder->create_icmp_lt(l_val, r_val);
            } else {
                cmp_result = builder->create_fcmp_lt(l_val, r_val);
            }
            break;
        case OP_GT:
            if (is_int) {
                cmp_result = builder->create_icmp_gt(l_val, r_val);
            } else {
                cmp_result = builder->create_fcmp_gt(l_val, r_val);
            }
            break;
        case OP_GE:
            if (is_int) {
                cmp_result = builder->create_icmp_ge(l_val, r_val);
            } else {
                cmp_result = builder->create_fcmp_ge(l_val, r_val);
            }
            break;
        case OP_EQ:
            if (is_int) {
                cmp_result = builder->create_icmp_eq(l_val, r_val);
            } else {
                cmp_result = builder->create_fcmp_eq(l_val, r_val);
            }
            break;
        case OP_NEQ:
            if (is_int) {
                cmp_result = builder->create_icmp_ne(l_val, r_val);
            } else {
                cmp_result = builder->create_fcmp_ne(l_val, r_val);
            }
            break;
    }
    // Convert i1 to i32 if needed? In C, boolean is int.
    // But the IR expects i1 for conditions. We'll keep as i1.
    return cmp_result;
}

Value* CminusfBuilder::visit(ASTAdditiveExpression &node) {
    if (node.additive_expression == nullptr) {
        return node.term->accept(*this);
    }

    auto *l_val = node.additive_expression->accept(*this);
    auto *r_val = node.term->accept(*this);
    bool is_int = promote(&*builder, &l_val, &r_val);
    Value *ret_val = nullptr;
    switch (node.op) {
    case OP_PLUS:
        if (is_int) {
            ret_val = builder->create_iadd(l_val, r_val);
        } else {
            ret_val = builder->create_fadd(l_val, r_val);
        }
        break;
    case OP_MINUS:
        if (is_int) {
            ret_val = builder->create_isub(l_val, r_val);
        } else {
            ret_val = builder->create_fsub(l_val, r_val);
        }
        break;
    }
    return ret_val;
}

Value* CminusfBuilder::visit(ASTTerm &node) {
    if (node.term == nullptr) {
        return node.factor->accept(*this);
    }

    auto *l_val = node.term->accept(*this);
    auto *r_val = node.factor->accept(*this);
    bool is_int = promote(&*builder, &l_val, &r_val);

    Value *ret_val = nullptr;
    switch (node.op) {
    case OP_MUL:
        if (is_int) {
            ret_val = builder->create_imul(l_val, r_val);
        } else {
            ret_val = builder->create_fmul(l_val, r_val);
        }
        break;
    case OP_DIV:
        if (is_int) {
            ret_val = builder->create_isdiv(l_val, r_val);
        } else {
            ret_val = builder->create_fdiv(l_val, r_val);
        }
        break;
    }
    return ret_val;
}

Value* CminusfBuilder::visit(ASTCall &node) {
    auto *func = dynamic_cast<Function *>(scope.find(node.id));
    std::vector<Value *> args;
    auto param_type = func->get_function_type()->param_begin();
    for (auto &arg : node.args) {
        auto *arg_val = arg->accept(*this);
        if (!arg_val->get_type()->is_pointer_type() &&
            *param_type != arg_val->get_type()) {
            if (arg_val->get_type()->is_integer_type()) {
                arg_val = builder->create_sitofp(arg_val, FLOAT_T);
            } else {
                arg_val = builder->create_fptosi(arg_val, INT32_T);
            }
        }
        args.push_back(arg_val);
        param_type++;
    }

    return builder->create_call(static_cast<Function *>(func), args);
}
