/*
 * evaluator.cpp
 *
 *  Created on: 18 сент. 2019 г.
 *      Author: sadko
 */

#include <math.h>
#include <core/calc/parser.h>
#include <core/calc/evaluator.h>

namespace lsp
{
    namespace calc
    {
        #define NUMERIC_OP(eval_name, oper) \
            status_t eval_name(value_t *value, const expr_t *expr, eval_env_t *env) \
            { \
                value_t right; \
                status_t res = expr->calc.left->eval(value, expr->calc.left, env); \
                if (res != STATUS_OK) \
                    return res; \
                \
                res = cast_numeric(value); \
                if (res == STATUS_OK) \
                    res = expr->calc.right->eval(&right, expr->calc.right, env); \
                \
                if (res != STATUS_OK) \
                { \
                    destroy_value(value); \
                    return res; \
                } \
                \
                res = cast_numeric(&right); \
                if (res == STATUS_OK) \
                { \
                    if (value->type == VT_INT) \
                    { \
                        if (right.type == VT_INT) \
                        { \
                            value->type     = VT_INT; \
                            value->v_int    = value->v_int oper right.v_int; \
                        } \
                        else \
                        { \
                            value->type     = VT_FLOAT; \
                            value->v_int    = value->v_int oper right.v_float; \
                        } \
                    } \
                    else \
                    { \
                        if (value->type == VT_INT) \
                        { \
                            value->type     = VT_FLOAT; \
                            value->v_int    = value->v_float oper right.v_int; \
                        } \
                        else \
                        { \
                            value->type     = VT_FLOAT; \
                            value->v_int    = value->v_float oper right.v_float; \
                        } \
                    } \
                } \
                else \
                    destroy_value(value); \
                \
                destroy_value(&right); \
                \
                return res; \
            }

        #define INT_OP(eval_name, oper) \
            status_t eval_name(value_t *value, const expr_t *expr, eval_env_t *env) \
            { \
                value_t right; \
                status_t res = expr->calc.left->eval(value, expr->calc.left, env); \
                if (res != STATUS_OK) \
                    return res; \
                \
                res = cast_int(value); \
                if (res == STATUS_OK) \
                    res = expr->calc.right->eval(&right, expr->calc.right, env); \
                if (res != STATUS_OK) \
                { \
                    destroy_value(value); \
                    return res; \
                } \
                \
                res = cast_int(&right); \
                if (res == STATUS_OK) \
                { \
                    value->type     = VT_INT; \
                    value->v_int    = value->v_int oper right.v_int; \
                } \
                else \
                    destroy_value(value); \
                \
                destroy_value(&right); \
                return res; \
            }

        NUMERIC_OP(eval_add, + );
        NUMERIC_OP(eval_sub, - );
        NUMERIC_OP(eval_mul, * );
        NUMERIC_OP(eval_div, / );

        INT_OP(eval_iadd, + );
        INT_OP(eval_isub, - );
        INT_OP(eval_imul, * );
        INT_OP(eval_idiv, / );
        INT_OP(eval_imod, % );
        INT_OP(eval_bit_or, | );
        INT_OP(eval_bit_and, & );
        INT_OP(eval_bit_xor, ^ );

        status_t eval_xor(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            value_t right;

            // Test left argument
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res == STATUS_OK)
                res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }

            // Test right argument
            res = cast_bool(&right);
            if (res == STATUS_OK)
                value->v_bool = !(value->v_bool == right.v_bool);
            else
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_or(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            // Test left argument
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }
            else if (value->v_bool)
                return STATUS_OK;

            // Test right argument
            destroy_value(value);
            res = expr->calc.right->eval(value, expr->calc.right, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_and(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            // Test left argument
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }
            else if (!value->v_bool)
                return STATUS_OK;

            // Test right argument
            destroy_value(value);
            res = expr->calc.right->eval(value, expr->calc.right, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_cmp(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            value_t right;

            // Test left argument
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }

            // Perform comparison
            if ((value->type == VT_STRING) || (right.type == VT_STRING))
            {
                res = cast_string(value);
                if (res == STATUS_OK)
                    res = cast_string(&right);
                if (res == STATUS_OK)
                {
                    ssize_t cmp     = value->v_str->compare_to(right.v_str);
                    destroy_value(value);
                    destroy_value(&right);

                    value->type     = VT_INT;
                    value->v_int    = cmp;
                }
            }
            else if ((value->type == VT_FLOAT) || (right.type == VT_FLOAT))
            {
                res = cast_float(value);
                if (res == STATUS_OK)
                    res = cast_float(&right);
                if (res == STATUS_OK)
                {
                    value->type     = VT_INT;
                    value->v_int    =
                            (value->v_float < right.v_float) ? -1 :
                            (value->v_float > right.v_float) ? 1 : 0;

                    destroy_value(value);
                    destroy_value(&right);


                }
            }


            switch (value->type)
            {
                case VT_INT:
                {
                    switch (right.type)
                    {
                        case VT_INT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_int < right.v_int) ? -1 :
                                    (value->v_int > right.v_int) ? 1 : 0;
                            break;
                        case VT_FLOAT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_int < right.v_float) ? -1 :
                                    (value->v_int > right.v_float) ? 1 : 0;
                            break;
                        case VT_BOOL:
                        {

                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_int < right.v_float) ? -1 :
                                    (value->v_int > right.v_float) ? 1 : 0;
                            break;
                        }
                    }
                }
                case VT_FLOAT:
                case VT_BOOL:
                case VT_STRING:
                default:
                    destroy_value(value);
                    res = STATUS_BAD_TYPE;
                    break;
            }
            destroy_value(&right);

            return res;
        }

        status_t eval_cmp_eq(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_ne(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_lt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_gt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_le(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_ge(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }



        status_t eval_icmp(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_eq(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_ne(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_lt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_gt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_le(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_ge(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_power(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            value_t right;
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_float(value);
            switch (value->type)
            {
                case VT_FLOAT:
                    res = expr->calc.right->eval(&right, expr->calc.right, env);
                    break;
                case VT_NULL:
                    value->type = VT_UNDEF;
                    return STATUS_OK;
                case VT_UNDEF:
                    return STATUS_OK;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res == STATUS_OK)
            {
                cast_float(&right);

                switch (right.type)
                {
                    case VT_FLOAT:
                        value->v_float  = ::pow(value->v_float, right.v_int);
                        break;
                    case VT_NULL:
                    case VT_UNDEF:
                        value->type = VT_UNDEF;
                        break;
                    default:
                        res = STATUS_BAD_TYPE;
                        break;
                }

                destroy_value(&right);
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_neg(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            if (value->type == VT_STRING)
                cast_numeric(value);

            switch (value->type)
            {
                case VT_INT:
                    value->type     = VT_INT;
                    value->v_int    = ~value->v_int;
                    break;
                case VT_FLOAT:
                    value->type     = VT_FLOAT;
                    value->v_float  = ~ssize_t(value->v_float);
                    break;
                case VT_BOOL:
                    value->type     = VT_BOOL;
                    value->v_bool   = !value->v_bool;
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res             = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_not(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_bool(value);
            switch (value->type)
            {
                case VT_BOOL:
                    value->type     = VT_BOOL;
                    value->v_bool   = !value->v_bool;
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res             = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_psign(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return expr->eval(value, expr->calc.left, env);
        }

        status_t eval_nsign(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_numeric(value);
            switch (value->type)
            {
                case VT_INT:
                    value->type     = VT_INT;
                    value->v_int    = -value->v_int;
                    break;
                case VT_FLOAT:
                    value->type     = VT_FLOAT;
                    value->v_float  = -value->v_float;
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_exists(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res    = expr->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            bool exists     = value->type != VT_UNDEF;
            destroy_value(value);

            value->type     = VT_BOOL;
            value->v_bool   = exists;

            return STATUS_OK;
        }

        status_t eval_db(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_float(value);
            switch (value->type)
            {
                case VT_FLOAT:
                    value->v_float  = exp(value->v_float * M_LN10 * 0.05);
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_resolve(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res;

            // No indexes? Do simple stuff
            if (expr->resolve.count <= 0)
            {
                res = env->resolve(value, expr->resolve.name, 0, NULL);
                if (res != STATUS_NOT_FOUND)
                    return res;

                value->type     = VT_UNDEF;
                value->v_str    = NULL;
                return STATUS_OK;
            }

            // Compute index values
            ssize_t *indexes = reinterpret_cast<ssize_t *>(::malloc(expr->resolve.count));
            if (indexes == NULL)
                return STATUS_NO_MEM;

            value_t tmp;
            for (size_t i=0; i<expr->resolve.count; ++i)
            {
                expr_t *e = expr->resolve.items[i];

                // Evaluate and store index
                res = e->eval(&tmp, e, env);
                if (res == STATUS_OK)
                {
                    res = cast_int(&tmp);
                    if (res == STATUS_OK)
                        indexes[i] = tmp.v_int;
                    destroy_value(&tmp);
                }

                // All is OK?
                if (res != STATUS_OK)
                {
                    ::free(indexes);
                    return res;
                }
            }

            // Now we can resolve values
            res = env->resolve(value, expr->resolve.name, expr->resolve.count, indexes);
            ::free(indexes);

            return res;
        }

        status_t eval_value(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return copy_value(value, &expr->value);
        }

        status_t eval_ternary(value_t *value, const expr_t *expr, eval_env_t *env)
        {

            return STATUS_NOT_IMPLEMENTED;
        }
    }
}


