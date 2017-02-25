#include "unused_code.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void UnusedCodeEliminator::remove(Instruction* instr) {
  if (lastInstr != NULL) {
    lastInstr->next = instr->next;
  } else {
    curBlock->head = instr->next;
  }
  instr->next = NULL;
  delete instr;
  performedWork = true;
}

void UnusedCodeEliminator::visitPureInstr(Instruction& instr) {
  // save next before instr is maybe freed!!
  Instruction* next = instr.next;
  if (instr.usages.size() == 0) {
     remove(&instr);
  }
}

void UnusedCodeEliminator::visitPhiInstr(PhiInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitArgInstr(ArgInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitConstantInstr(ConstantInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCallInstr(CallInstr& instr) {
}

void UnusedCodeEliminator::visitTypecheckInstr(TypecheckInstr& instr) {
}

void UnusedCodeEliminator::visitBoxInstr(BoxInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitAddInstr(AddInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitSubInstr(SubInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitMulInstr(MulInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitDivInstr(DivInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitNotInstr(NotInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpEqInstr(CmpEqInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpIneqInstr(CmpIneqInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpGtInstr(CmpGtInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpGteInstr(CmpGteInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpLtInstr(CmpLtInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpLteInstr(CmpLteInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitErrInstr(ErrInstr& instr) {
  Instruction* deleteFrom = instr.next;
  instr.next = NULL;

  // TODO free multiple instructions in a row right
  //while (deleteFrom != NULL) {
  //  Instruction* next = deleteFrom->next;
  //  delete deleteFrom;
  //  deleteFrom = next;
  //}
}

void UnusedCodeEliminator::visitRetInstr(RetInstr& instr) {
  Instruction* deleteFrom = instr.next;
  instr.next = NULL;

  // TODO free multiple instructions in a row right
  //while (deleteFrom != NULL) {
  //  Instruction* next = deleteFrom->next;
  //  delete deleteFrom;
  //  deleteFrom = next;
  //}
}
