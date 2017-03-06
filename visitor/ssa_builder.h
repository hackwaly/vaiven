#ifndef VAIVEN_VISITOR_HEADER_SSA_BUILDER
#define VAIVEN_VISITOR_HEADER_SSA_BUILDER

#include <stack>
#include <map>
#include <string>
#include <unordered_set>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../type_info.h"
#include "../scope.h"
#include "../ssa/ssa.h"
#include "../ssa/cfg.h"
#include "../function_usage.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;
using std::stack;
using std::map;
using std::string;
using std::unordered_set;

class SsaBuilder : public Visitor<TypedLocationInfo> {

  public:
  SsaBuilder(FunctionUsage& usageInfo)
      : usageInfo(usageInfo), curBlock(&head), cur(NULL), writePoint(NULL) {};

  virtual void visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr);
  virtual void visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr);
  virtual void visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr);
  virtual void visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr);
  virtual void visitVariableExpression(VariableExpression<TypedLocationInfo>& expr);
  virtual void visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr);
  virtual void visitNotExpression(NotExpression<TypedLocationInfo>& expr);
  virtual void visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr);
  virtual void visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr);
  virtual void visitGtExpression(GtExpression<TypedLocationInfo>& expr);
  virtual void visitGteExpression(GteExpression<TypedLocationInfo>& expr);
  virtual void visitLtExpression(LtExpression<TypedLocationInfo>& expr);
  virtual void visitLteExpression(LteExpression<TypedLocationInfo>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt);
  virtual void visitIfStatement(IfStatement<TypedLocationInfo>& stmt);
  virtual void visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt);
  virtual void visitBlock(Block<TypedLocationInfo>& expr);
  virtual void visitFuncDecl(FuncDecl<TypedLocationInfo>& funcDecl);
  virtual void visitVarDecl(VarDecl<TypedLocationInfo>& varDecl);

  ssa::Block head;
  ssa::Block* curBlock;
  ssa::Instruction* cur;
  ssa::Instruction* writePoint;

  private:
  void emit(ssa::Instruction* next);

  Scope<ssa::Instruction*> scope;
  FunctionUsage& usageInfo;
  unordered_set<string> varsToPhi;

};

}}

#endif
