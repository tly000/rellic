/*
 * Copyright (c) 2021-present, Trail of Bits, Inc.
 * All rights reserved.
 *
 * This source code is licensed in accordance with the terms specified in
 * the LICENSE file found in the root directory of this source tree.
 */

#include "rellic/AST/ASTBuilder.h"

#include <clang/Tooling/Tooling.h>
#include <doctest/doctest.h>

namespace {
static std::unique_ptr<clang::ASTUnit> GetEmptyASTUnit() {
  auto unit{clang::tooling::buildASTFromCode("", "out.c")};
  REQUIRE(unit != nullptr);
  return unit;
}

static void IsNullPtrExprCheck(clang::ASTContext &ctx, clang::Expr *expr) {
  CHECK(
      expr->isNullPointerConstant(ctx, clang::Expr::NPC_NeverValueDependent) ==
      clang::Expr::NPCK_ZeroLiteral);
  CHECK(
      expr->isNullPointerConstant(ctx, clang::Expr::NPC_ValueDependentIsNull) ==
      clang::Expr::NPCK_ZeroLiteral);
  CHECK(expr->isNullPointerConstant(ctx,
                                    clang::Expr::NPC_ValueDependentIsNotNull) ==
        clang::Expr::NPCK_ZeroLiteral);
}
}  // namespace

// TODO(surovic): Add test cases for signed `llvm::APInt` and group
// via `SCENARIO` rather than `TEST_SUITE`. This needs better
// support for value-parametrized tests from doctest. Should be in
// version 2.5.0
TEST_SUITE("ASTBuilder::CreateIntLit") {
  SCENARIO("Create clang::IntegerLiteral for 1 bit integer value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("1 bit unsigned llvm::APInt") {
        llvm::APInt api(1U, 0U, /*isSigned=*/false);
        auto lit{ast.CreateIntLit(api)};
        THEN("return a `unsigned int` typed integer literal") {
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::IntegerLiteral>(lit));
          CHECK(lit->getType() == ctx.UnsignedIntTy);
        }
      }
    }
  }

  SCENARIO("Create clang::IntegerLiteral for 8 bit integer value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("8 bit unsigned llvm::APInt") {
        llvm::APInt api(8U, 42U, /*isSigned=*/false);
        THEN("return a `unsigned int` typed integer literal") {
          auto lit{ast.CreateIntLit(api)};
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::IntegerLiteral>(lit));
          CHECK(lit->getType() == ctx.UnsignedIntTy);
        }
        THEN(
            "return a `unsigned int` typed integer literal casted to `unsigned "
            "char`") {
          auto cast{ast.CreateAdjustedIntLit(api)};
          REQUIRE(cast != nullptr);
          CHECK(clang::isa<clang::CStyleCastExpr>(cast));
          CHECK(cast->getType() == ctx.UnsignedCharTy);
          auto lit{cast->IgnoreCasts()};
          CHECK(clang::isa<clang::IntegerLiteral>(lit));
          CHECK(lit->getType() == ctx.UnsignedIntTy);
        }
      }
    }
  }

  SCENARIO("Create clang::IntegerLiteral for 16 bit integer value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("16 bits wide llvm::APInt") {
        llvm::APInt api(16U, 42U, /*isSigned=*/false);
        THEN("return a `unsigned int` typed integer literal") {
          auto lit{ast.CreateIntLit(api)};
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::IntegerLiteral>(lit));
          CHECK(lit->getType() == ctx.UnsignedIntTy);
        }
        THEN(
            "return a `unsigned int` typed integer literal casted to `unsigned "
            "short`") {
          auto cast{ast.CreateAdjustedIntLit(api)};
          REQUIRE(cast != nullptr);
          CHECK(clang::isa<clang::CStyleCastExpr>(cast));
          CHECK(cast->getType() == ctx.UnsignedShortTy);
          auto lit{cast->IgnoreCasts()};
          CHECK(clang::isa<clang::IntegerLiteral>(lit));
          CHECK(lit->getType() == ctx.UnsignedIntTy);
        }
      }
    }
  }

  SCENARIO("Create clang::IntegerLiteral for 32 bit integer value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("32 bits wide llvm::APInt") {
        llvm::APInt api(32U, 42U, /*isSigned=*/false);
        auto lit{ast.CreateIntLit(api)};
        THEN("return a `unsigned int` typed integer literal") {
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::IntegerLiteral>(lit));
          CHECK(lit->getType() == ctx.UnsignedIntTy);
        }
      }
    }
  }

  SCENARIO("Create clang::IntegerLiteral for 64 bit integer value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("64 bits wide llvm::APInt") {
        llvm::APInt api(64U, 42U, /*isSigned=*/false);
        auto lit{ast.CreateIntLit(api)};
        THEN("return a `unsigned long` typed integer literal") {
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::IntegerLiteral>(lit));
          CHECK(lit->getType() == ctx.UnsignedLongTy);
        }
      }
    }
  }
}

TEST_SUITE("ASTBuilder::CreateCharLit") {
  SCENARIO("Create clang::CharacterLiteral for 8 bit integer value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("8 bit wide unsigned llvm::APInt") {
        llvm::APInt api(8U, 'x', /*isSigned=*/false);
        auto lit{ast.CreateCharLit(api)};
        THEN("return a `int` typed character literal") {
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::CharacterLiteral>(lit));
          CHECK(lit->getType() == ctx.IntTy);
        }
      }
    }
  }
}

TEST_SUITE("ASTBuilder::CreateStrLit") {
  SCENARIO("Create clang::StringLiteral from std::string") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("std::string object by value") {
        std::string str("a string");
        auto lit{ast.CreateStrLit(str)};
        THEN("return a `char[]` typed character literal") {
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::StringLiteral>(lit));
          CHECK(lit->getType() ==
                ctx.getStringLiteralArrayType(ctx.CharTy, str.size()));
        }
      }
    }
  }
}

TEST_SUITE("ASTBuilder::CreateFPLit") {
  SCENARIO("Create clang::FloatingLiteral for 32 bit IEEE 754 value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("`float` initialized llvm::APFloat") {
        auto lit{ast.CreateFPLit(llvm::APFloat(float(3.14)))};
        THEN("return a `float` typed floating point literal") {
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::FloatingLiteral>(lit));
          CHECK(lit->getType() == ctx.FloatTy);
        }
      }
    }
  }

  SCENARIO("Create clang::FloatingLiteral for 64 bit IEEE 754 value") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("`double` initialized llvm::APFloat") {
        auto lit{ast.CreateFPLit(llvm::APFloat(double(3.14)))};
        THEN("return a `double` typed floating point literal") {
          REQUIRE(lit != nullptr);
          CHECK(clang::isa<clang::FloatingLiteral>(lit));
          CHECK(lit->getType() == ctx.DoubleTy);
        }
      }
    }
  }
}

TEST_SUITE("ASTBuilder::CreateNull") {
  SCENARIO("Create clang::Expr representing a null pointer") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      THEN("return a `0U` integer literal casted to a `void` pointer") {
        auto expr{ast.CreateNull()};
        REQUIRE(expr != nullptr);
        IsNullPtrExprCheck(ctx, expr);
      }
    }
  }
}

TEST_SUITE("ASTBuilder::CreateUndef") {
  SCENARIO("Create clang::Expr whose value is undefined") {
    GIVEN("Empty clang::ASTContext") {
      auto unit{GetEmptyASTUnit()};
      auto &ctx{unit->getASTContext()};
      rellic::ASTBuilder ast(ctx);
      GIVEN("an arbitrary type `t`") {
        auto type{ctx.DoubleTy};
        THEN("return a null pointer dereference of type `t`") {
          auto expr{ast.CreateUndef(type)};
          REQUIRE(expr != nullptr);
          CHECK(expr->getType() == ctx.DoubleTy);
          auto deref{clang::dyn_cast<clang::UnaryOperator>(expr)};
          REQUIRE(deref != nullptr);
          CHECK(deref->getOpcode() == clang::UO_Deref);
          IsNullPtrExprCheck(ctx, deref->getSubExpr()->IgnoreCasts());
        }
      }
    }
  }
}