#include "cminusf_builder.hpp"
#include "Constant.hpp"
#include "GlobalVariable.hpp"
#include "Type.hpp"
#include "Value.hpp"
#include <system_error>

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
    // TODO: This function is empty now.
    // Add some code here.
    Type *var_type;
    if (node.type == TYPE_INT) {
        var_type = INT32_T;
    } else if (node.type == TYPE_FLOAT) {
        var_type = FLOAT_T;
    }

    if(scope.in_global())//全局
    {
        if(node.num==nullptr)//不是数组
        {
            auto initial=ConstantZero::get(var_type, module.get());
            auto global_alloc=GlobalVariable::create(node.id,module.get(),var_type,false,initial);
            scope.push(node.id,global_alloc);
        }
        else//是数组
        {
            auto *arrayType=ArrayType::get(var_type,node.num->i_val);
            auto initial=ConstantZero::get(arrayType, module.get());
            auto global_alloc=GlobalVariable::create(node.id,module.get(),arrayType,false,initial);
            scope.push(node.id,global_alloc);
        }
    }
    else//非全局
    {
        if(node.num==nullptr)//不是数组
        {
            auto alloc=builder->create_alloca(var_type);
            scope.push(node.id,alloc);
        }
        else//数组
        {
            auto *arrayType=ArrayType::get(var_type,node.num->i_val);
            auto alloc=builder->create_alloca(arrayType);
            scope.push(node.id,alloc);
        }
    }

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
        // TODO: You need to deal with params and store them in the scope.
        Value *param_var;
        if(node.params[i]->isarray)
        {
            if(node.params[i]->type==TYPE_INT)
                param_var=builder->create_alloca(INT32PTR_T);
            else if(node.params[i]->type==TYPE_FLOAT)
                param_var=builder->create_alloca(FLOATPTR_T);
            builder->create_store(args[i],param_var);
            //param_value=builder->create_load(param_var);
            scope.push(node.params[i]->id, param_var);
        }
        else
        {
            if(node.params[i]->type==TYPE_INT)
                param_var=builder->create_alloca(INT32_T);
            else if(node.params[i]->type==TYPE_FLOAT)
                param_var=builder->create_alloca(FLOAT_T);
            builder->create_store(args[i],param_var);
            scope.push(node.params[i]->id, param_var);
        }

    }
    // std::cout << 1 << std::endl;
    node.compound_stmt->accept(*this);
    // if (builder->get_insert_block()->get_terminator() == nullptr)
    if(!builder->get_insert_block()->is_terminated()) 
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
    // TODO: This function is not complete.
    // You may need to add some code here
    // to deal with complex statements. 

    scope.enter();
    for (auto &decl : node.local_declarations) {
        decl->accept(*this);
    }

    for (auto &stmt : node.statement_list) {
        stmt->accept(*this);
        // if (builder->get_insert_block()->get_terminator() == nullptr)
        //     break;
        if (builder->get_insert_block()->is_terminated())
            break;
    }
    scope.exit();
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
        // if (not builder->get_insert_block()->is_terminated()) {
        //     builder->create_br(contBB);
        // }
        if (!builder->get_insert_block()->is_terminated()) {
            builder->create_br(contBB);
        }
    }

    builder->set_insert_point(contBB);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTIterationStmt &node) {
    // TODO: This function is empty now.
    // Add some code here.
    // auto *ret_val = node.expression->accept(*this);
    // auto fun = builder->get_insert_block()->get_parent();
    static int while_cnt = 0;
    int my_id = while_cnt++;

    auto judgeBB=BasicBlock::create(module.get(),"judgeBB"+ std::to_string(my_id),context.func);
    auto stateBB=BasicBlock::create(module.get(),"stateBB"+ std::to_string(my_id),context.func);
    auto outBB=BasicBlock::create(module.get(),"outBB"+ std::to_string(my_id),context.func);
//judge
    builder->create_br(judgeBB);
    builder->set_insert_point(judgeBB);
    auto *ret_val = node.expression->accept(*this);
    auto ex_tp=ret_val->get_type();
    Value* truefalse;
    if(ex_tp->is_integer_type())//整形
    {
        truefalse = builder->create_icmp_gt(ret_val, CONST_INT(0));
    }
    else
    {
        truefalse = builder->create_fcmp_gt(ret_val, CONST_FP(0));
    }
    builder->create_cond_br(truefalse,stateBB,outBB);
//state
    builder->set_insert_point(stateBB);
    node.statement->accept(*this);
    if(!builder->get_insert_block()->is_terminated())
        builder->create_br(judgeBB);//不会返回
//out
    builder->set_insert_point(outBB);
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
    // TODO: This function is empty now.
    // Add some code here.
    bool ret_val = context.require_lvalue;
    context.require_lvalue = false;
    if(node.expression==nullptr)
    {
        auto var=scope.find(node.id);
        if(ret_val) {
            context.require_lvalue = false;
            return var;
        }
        else
        {
            if(var->get_type()->get_pointer_element_type()->is_integer_type()||var->get_type()->get_pointer_element_type()->is_float_type()||var->get_type()->get_pointer_element_type()->is_pointer_type())
                return builder->create_load(var);
            else
                return builder->create_gep(var, {CONST_INT(0), CONST_INT(0)});
        }
    }
    else
    {
        auto var=scope.find(node.id);
        auto *ret_val_expr = node.expression->accept(*this);
        Value* index;
        // auto fun = builder->get_insert_block()->get_parent();
        auto ex_tp=ret_val_expr->get_type();
        // Value* ret;
        if(ret_val_expr->get_type()->is_float_type())
        {
            ret_val_expr=builder->create_fptosi(ret_val_expr,INT32_T);
        }
        index=ret_val_expr;
        Value* check;
        check=builder->create_icmp_lt(index, CONST_INT(0));
        auto curBB=BasicBlock::create(module.get(),"",context.func);
        auto wrongBB=BasicBlock::create(module.get(),"",context.func);
        builder->create_cond_br(check,wrongBB, curBB);
//wrong
        builder->set_insert_point(wrongBB);
        auto wrong=scope.find("neg_idx_except");
        builder->create_call(wrong,{});
        builder->create_br(curBB);
//correct
        builder->set_insert_point(curBB);
        Value* vargep;
        auto is_int = var->get_type()->get_pointer_element_type()->is_integer_type();
        auto is_float = var->get_type()->get_pointer_element_type()->is_float_type();
        auto is_ptr = var->get_type()->get_pointer_element_type()->is_pointer_type();
        if(is_int || is_float) 
            vargep = builder->create_gep(var, {index});
        else if(is_ptr) {
            auto arr_load = builder->create_load(var);
            vargep = builder->create_gep(arr_load, {index});
        }
        else vargep = builder->create_gep(var, {CONST_INT(0), index});
        if(ret_val) {
            // ret =vargep;
            context.require_lvalue = false;
            return vargep;
        }
        else
            return builder->create_load(vargep);
    }
    return nullptr;
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
    // TODO: This function is empty now.
    // Add some code here.
    Value* ret_l = node.additive_expression_l->accept(*this);
    if(node.additive_expression_r!=nullptr)//另一个不为空
    {
        Value* ret_r;
        ret_r = node.additive_expression_r->accept(*this);
        Value* ret;
        if((ret_l->get_type()->is_integer_type())&&(ret_r->get_type()->is_integer_type())){
            switch (node.op) {
                case OP_LE:
                    ret = builder->create_icmp_le(ret_l, ret_r);
                    break;
                case OP_LT:
                    ret = builder->create_icmp_lt(ret_l, ret_r);
                    break;
                case OP_GT:
                    ret = builder->create_icmp_gt(ret_l, ret_r);
                    break;
                case OP_GE:
                    ret = builder->create_icmp_ge(ret_l, ret_r);
                    break;
                case OP_EQ:
                    ret = builder->create_icmp_eq(ret_l, ret_r);
                    break;
                case OP_NEQ:
                    ret = builder->create_icmp_ne(ret_l, ret_r);
                    break;
                default: {
                    return nullptr;
                    break;
                }
            }
        }
        else{//至少有一个float
            if(ret_l->get_type()->is_integer_type())
                ret_l=builder->create_sitofp(ret_l,FLOAT_T);
            if(ret_r->get_type()->is_integer_type())
                ret_r=builder->create_sitofp(ret_r,FLOAT_T);
            switch (node.op) {
                case OP_LE:
                    ret = builder->create_fcmp_le(ret_l, ret_r);
                    break;
                case OP_LT:
                    ret = builder->create_fcmp_lt(ret_l, ret_r);
                    break;
                case OP_GT:
                    ret = builder->create_fcmp_gt(ret_l, ret_r);
                    break;
                case OP_GE:
                    ret = builder->create_fcmp_ge(ret_l, ret_r);
                    break;
                case OP_EQ:
                    ret = builder->create_fcmp_eq(ret_l, ret_r);
                    break;
                case OP_NEQ:
                    ret = builder->create_fcmp_ne(ret_l, ret_r);
                    break;
                default: {
                    return nullptr;
                    break;
                }
            }
        }
        return builder->create_zext(ret,INT32_T);
    }
    else
        return ret_l;
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
