#include "inliner.h"
#include "function_merge.h"
#include "../visitor/ssa_builder.h"

using namespace vaiven::ssa;
using namespace vaiven::ast;

const int STOP_INLINING_AT_SIZE = 5096; // ?
const int MAX_INLINE_SIZE = 512; // ?
const int RECURSION_IDEAL_SIZE = 128; // ?
const int BOX_COST = 2;
const int TYPECHECK_COST = 3;
const int SPILL_COST = 1;
const int ARG_COST = 1; // every arg is a chance to optimize

void Inliner::visitPhiInstr(PhiInstr& instr) {
}

void Inliner::visitArgInstr(ArgInstr& instr) {
}

void Inliner::visitConstantInstr(ConstantInstr& instr) {
}

void Inliner::visitCallInstr(CallInstr& instr) {
  if (funcs.funcs.find(instr.funcName) == funcs.funcs.end()) {
    return;
  }

  // arbitrary max sizes to stop inlining
  if (currentWorstSize > STOP_INLINING_AT_SIZE
      || funcs.funcs[instr.funcName]->worstSize > MAX_INLINE_SIZE
      || funcs.funcs[instr.funcName]->worstSize > MAX_INLINE_SIZE) {
    return;
  }

  int argc = instr.inputs.size();
  int callOverheadGuess = argc * SPILL_COST + argc * ARG_COST;
  for (vector<Instruction*>::iterator it = instr.inputs.begin(); it != instr.inputs.end(); ++it) {
    if ((*it)->tag == INSTR_BOX) {
      callOverheadGuess += BOX_COST;
    } else if ((*it)->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
      // assume passed in args are type checked at least once
      callOverheadGuess += TYPECHECK_COST;
    }
  }

  // rax will spill, must be a boxed value, may be typechecked after
  callOverheadGuess += SPILL_COST + BOX_COST;
  for (set<Instruction*>::iterator it = instr.usages.begin(); it != instr.usages.end(); ++it) {
    if ((*it)->tag == INSTR_TYPECHECK) {
      callOverheadGuess += TYPECHECK_COST;
    }
  }

  // assume 40% reduction in code size from hot optimization (?) vs worst size
  int afterOptimizeGuessSize = funcs.funcs[instr.funcName]->worstSize * 6 / 10;

  // TODO count % of times executed rather than "is hot" (which doesn't even really matter)
  bool isHot = funcs.funcs[instr.funcName]->usage->count == HOT_COUNT;

  // if the function isn't smaller than its overhead, and its not hot, don't inline
  if (afterOptimizeGuessSize > callOverheadGuess && !isHot) {
    return;
  }

  currentWorstSize += funcs.funcs[instr.funcName]->worstSize;

  FuncDecl<TypedLocationInfo>& funcDecl = *funcs.funcs[instr.funcName]->ast;

  Block* returnPoint = new Block();
  PhiInstr* resultPhi = new PhiInstr();

  // note: usage is unused here...
  visitor::SsaBuilder buildInlineCode(*funcs.funcs[instr.funcName]->usage);

  // TODO consolidate this better with SsaBuilder
  for (int i = 0; i < funcDecl.args.size(); ++i) {
    if (instr.inputs.size() < i) {
      ConstantInstr* void_ = new ConstantInstr(Value());
      buildInlineCode.emit(void_);
      buildInlineCode.scope.put(funcDecl.args[i], void_);
    } else {
      buildInlineCode.scope.put(funcDecl.args[i], instr.inputs[i]);
    }
  }

  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = funcDecl.statements.begin();
      it != funcDecl.statements.end();
      ++it) {
    (*it)->accept(buildInlineCode);
  }

  // TODO proper logic here
  if (buildInlineCode.cur != NULL && buildInlineCode.cur->tag != INSTR_RET) {
    buildInlineCode.emit(new RetInstr(buildInlineCode.cur));
  }

  FunctionMerger merger(returnPoint, resultPhi);
  buildInlineCode.head.accept(merger);

  Block* finalBlock = merger.curBlock;

  resultPhi->next = instr.next;
  instr.next = NULL;
  instr.replaceUsagesWith(resultPhi);
  if (lastInstr != NULL) {
    lastInstr->next = buildInlineCode.head.head.release();
    delete &instr;
  } else {
    curBlock->head.reset(buildInlineCode.head.head.release());
  }
  std::swap(returnPoint->exits, curBlock->exits);
  returnPoint->next.reset(curBlock->next.release());
  finalBlock->next.reset(returnPoint);
  curBlock->next.reset(buildInlineCode.head.next.release());
  std::swap(curBlock->exits, buildInlineCode.head.exits);
  finalBlock->exits.push_back(unique_ptr<BlockExit>(new UnconditionalBlockExit(returnPoint)));

  returnPoint->head.reset(resultPhi);
}

void Inliner::visitTypecheckInstr(TypecheckInstr& instr) {
}

void Inliner::visitBoxInstr(BoxInstr& instr) {
}

void Inliner::visitAddInstr(AddInstr& instr) {
}

void Inliner::visitSubInstr(SubInstr& instr) {
}

void Inliner::visitMulInstr(MulInstr& instr) {
}

void Inliner::visitDivInstr(DivInstr& instr) {
}

void Inliner::visitNotInstr(NotInstr& instr) {
}

void Inliner::visitCmpEqInstr(CmpEqInstr& instr) {
}

void Inliner::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void Inliner::visitCmpGtInstr(CmpGtInstr& instr) {
}

void Inliner::visitCmpGteInstr(CmpGteInstr& instr) {
}

void Inliner::visitCmpLtInstr(CmpLtInstr& instr) {
}

void Inliner::visitCmpLteInstr(CmpLteInstr& instr) {
}

void Inliner::visitErrInstr(ErrInstr& instr) {
}

void Inliner::visitRetInstr(RetInstr& instr) {
}

void Inliner::visitJmpCcInstr(JmpCcInstr& instr) {
}

void Inliner::visitBlock(Block& block) {
  curBlock = &block;
  lastInstr = NULL;
  Instruction* next = block.head.get();
  while (next != NULL) {
    next->accept(*this);
    // special cases: next was replaced
    if (lastInstr != NULL && lastInstr->next != next) {
      next = lastInstr->next;
    } else if (lastInstr == NULL && block.head.get() != next) {
      next = block.head.get();
    } else {
      lastInstr = next;
      next = next->next;
    }
  }

  if (block.next != NULL) {
    block.next->accept(*this);
  }
}