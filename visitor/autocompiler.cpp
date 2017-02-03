#include "autocompiler.h"

#include "../ast/all.h"

#include <stdint.h>

using namespace asmjit;
using namespace vaiven::visitor;

void AutoCompiler::compile(Expression<Location>& root, int numVars) {
  cc.addFunc(FuncSignature8<int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t>());

  for (int i = 0; i < numVars; ++i) {
    X86Gp arg = cc.newInt64();
    cc.setArg(i, arg);
    argRegs.push_back(arg);
  }

  root.accept(*this);
  cc.ret(vRegs.top());
  cc.endFunc();
  cc.finalize();
}

void AutoCompiler::visitAdditionExpression(AdditionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.add(result, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.add(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.add(result, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.add(result, rhsReg);
  }
  vRegs.push(result);
}

void AutoCompiler::visitSubtractionExpression(SubtractionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.sub(result, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.neg(result);
    cc.add(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.sub(result, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.sub(result, rhsReg);
  }
  vRegs.push(result);
}
void AutoCompiler::visitMultiplicationExpression(MultiplicationExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.imul(result, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.imul(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.imul(result, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.imul(result, rhsReg);
  }
  vRegs.push(result);
}
void AutoCompiler::visitDivisionExpression(DivisionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newInt64();
  X86Gp dummy = cc.newInt64();
  cc.xor_(dummy, dummy);
  X86Gp divisor = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.mov(divisor, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    // will this work if its on the stack?
    divisor = vRegs.top(); vRegs.pop();
    cc.mov(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.mov(divisor, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    divisor = vRegs.top(); vRegs.pop();
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
  }
  cc.idiv(dummy, result, divisor);
  vRegs.push(result);
}

void AutoCompiler::visitIntegerExpression(IntegerExpression<Location>& expr) {
  // should only happen when in a stmt by itself
  X86Gp var = cc.newInt64();
  cc.mov(var, expr.value);
  vRegs.push(var);
}

void AutoCompiler::visitVariableExpression(VariableExpression<Location>& expr) {
  vRegs.push(argRegs[expr.resolvedData.data.argIndex]);
}
