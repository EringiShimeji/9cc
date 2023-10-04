#ifdef __APPLE__
#define _9CC_MAIN "_main"
#define _9CC_MOVZB "movzx"
#else
#define _9CC_MAIN "main"
#define _9CC_MOVZB "movzb"
#endif

#define _9CC_ASM_UNARY_OP_FORMAT(op, format, value) \
	printf(                                         \
		"  "                                        \
		"" op " " format "\n",                      \
		value)
#define _9CC_ASM_NULLARY_OP(op) \
	printf(                     \
		"  "                    \
		"" op "\n")
#define _9CC_ASM_UNARY_OP(op, operand) \
	printf(                            \
		"  "                           \
		"" op " " operand "\n")
#define _9CC_ASM_BINARY_OP(op, lhs, rhs) \
	printf(                              \
		"  "                             \
		"" op " " lhs ", " rhs "\n")
#define _9CC_ASM_BINARY_OP_FORMAT(op, lhs, rhs, ...) \
	printf(                                          \
		"  "                                         \
		"" op " " lhs ", " rhs "\n",                 \
		__VA_ARGS__)
#define _9CC_VA_ASM(_1, _2, _3, _4, _5, NAME, ...) NAME
#define ASM(...)                                               \
	_9CC_VA_ASM(__VA_ARGS__, _9CC_ASM_BINARY_OP_FORMAT,        \
				_9CC_ASM_BINARY_OP_FORMAT, _9CC_ASM_BINARY_OP, \
				_9CC_ASM_UNARY_OP, _9CC_ASM_NULLARY_OP, )      \
	(__VA_ARGS__)
#define MOVZB(dst, src) ASM(_9CC_MOVZB, dst, src)
#define CMP(lhs, rhs) ASM("cmp", lhs, rhs)
#define _9CC_PUSH_FORMAT(format, value) \
	_9CC_ASM_UNARY_OP_FORMAT("push", format, value)
#define _9CC_PUSH_REG(reg) ASM("push", reg)
#define _9CC_VA_PUSH(_1, _2, NAME, ...) NAME
#define PUSH(...) \
	_9CC_VA_PUSH(__VA_ARGS__, _9CC_PUSH_FORMAT, _9CC_PUSH_REG)(__VA_ARGS__)
#define POP(reg) ASM("pop", reg)
#define RET() ASM("ret")
#define MOV(dst, src) ASM("mov", dst, src)
