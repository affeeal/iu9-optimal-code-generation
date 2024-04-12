/*
 * Copyright © 2024 Ilya Afanasyev
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <memory>
#include <string_view>

// clang-format off
#include <boost/json.hpp>

#include "gcc-plugin.h"  // the first gcc header to be included
#include "tree-core.h"
#include "tree.h"
#include "tree-pass.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "gimple-pretty-print.h"
#include "context.h"
#include "plugin-version.h"
#include "ssa.h"
#include "ssa-iterators.h"
// clang-format on

int plugin_is_GPL_compatible;  // asserts the plugin is licensed under the
                               // GPL-compatible license

namespace {

class PrintPass;

boost::json::object ir;

const auto kPrintPass = std::make_unique<PrintPass>(g);

constexpr std::string_view kBasicBlocks = "basic_blocks";
constexpr std::string_view kFunctions = "functions";
constexpr std::string_view kGimpleType = "gimple_type";
constexpr std::string_view kIndex = "index";
constexpr std::string_view kLhs = "lhs";
constexpr std::string_view kName = "name";
constexpr std::string_view kPredecessors = "predecessors";
constexpr std::string_view kRhsClass = "rhs_class";
constexpr std::string_view kRhsCode = "rhs_code";
constexpr std::string_view kStatements = "statements";
constexpr std::string_view kSuccessors = "successors";
constexpr std::string_view kTreeType = "tree_type";
constexpr std::string_view kValue = "value";

constexpr std::string_view kGimpleSingleRhs = "GIMPLE_SINGLE_RHS";
constexpr std::string_view kGimpleUnaryRhs = "GIMPLE_UNARY_RHS";
constexpr std::string_view kGimpleBinaryRhs = "GIMPLE_BINARY_RHS";
constexpr std::string_view kGimpleTernaryRhs = "GIMPLE_TERNARY_RHS";
constexpr std::string_view kGimpleInvalidRhs = "GIMPLE_INVALID_RHS";

constexpr auto kPrintPassData = pass_data{
    .type = GIMPLE_PASS,
    .name = "print",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required = PROP_gimple_any,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0,
};

std::string_view ToString(const gimple_rhs_class rhs_class) {
  switch (rhs_class) {
    case GIMPLE_SINGLE_RHS: {
      return kGimpleSingleRhs;
    }

    case GIMPLE_UNARY_RHS: {
      return kGimpleUnaryRhs;
    }

    case GIMPLE_BINARY_RHS: {
      return kGimpleBinaryRhs;
    }

    case GIMPLE_TERNARY_RHS: {
      return kGimpleTernaryRhs;
    }

    case GIMPLE_INVALID_RHS: {
      return kGimpleInvalidRhs;
    }
  }
}

class PrintPass final : public gimple_opt_pass {
 public:
  PrintPass(gcc::context* ctxt) : gimple_opt_pass(kPrintPassData, ctxt) {}

  PrintPass* clone() override;
  unsigned int execute(function* fn) override;
};

PrintPass* PrintPass::clone() { return this; }

boost::json::object ToObject(const tree tr) {
  auto obj = boost::json::object{};
  obj[kTreeType] = get_tree_code_name(TREE_CODE(tr));

  switch (TREE_CODE(tr)) {
    case INTEGER_CST: {
      obj[kValue] = TREE_INT_CST_LOW(tr);
      break;
    }

    case SSA_NAME: {
      if (const auto id = SSA_NAME_IDENTIFIER(tr)) {
        obj["name"] = IDENTIFIER_POINTER(id);
      } else {
        // TODO: отобразить "индекс" переменной?
        obj["name"] = nullptr;
      }

      obj["version"] = SSA_NAME_VERSION(tr);

      if (const auto stmt = SSA_NAME_DEF_STMT(tr);
          gimple_code(stmt) == GIMPLE_PHI) {
        const auto args_num = gimple_phi_num_args(stmt);

        auto& args = (obj["phi_args"] = boost::json::array{}).as_array();
        args.reserve(args_num);

        for (std::size_t i = 0; i < args_num; ++i) {
          args.push_back(ToObject(gimple_phi_arg_def(stmt, i)));
        }
      }

      break;
    }

    default: {
      // TODO
      break;
    }
  }

  return obj;
}

boost::json::object TraverseGimpleAssignStatement(gimple* const stmt) {
  auto stmt_obj = boost::json::object{};
  stmt_obj[kGimpleType] = "GIMPLE_ASSIGN";
  stmt_obj[kRhsClass] = ToString(gimple_assign_rhs_class(stmt));
  stmt_obj[kRhsCode] = get_tree_code_name(gimple_assign_rhs_code(stmt));
  stmt_obj[kLhs] = ToObject(gimple_assign_lhs(stmt));
  // TODO: rhs1, rhs2, rhs3

  return stmt_obj;
}

boost::json::object TraverseBasicBlock(const basic_block bb) {
  auto bb_obj = boost::json::object{};
  bb_obj[kIndex] = bb->index;

  auto& preds = (bb_obj[kPredecessors] = boost::json::array{}).as_array();
  preds.reserve(bb->preds->length());

  auto& succs = (bb_obj[kSuccessors] = boost::json::array{}).as_array();
  succs.reserve(bb->succs->length());

  auto e = edge{};
  auto ei = edge_iterator{};

  FOR_EACH_EDGE(e, ei, bb->preds) { preds.push_back(e->src->index); }
  FOR_EACH_EDGE(e, ei, bb->succs) { succs.push_back(e->dest->index); }

  auto& stmts = (bb_obj[kStatements] = boost::json::array{}).as_array();
  // TODO: reserve

  for (auto gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
    auto* stmt = gsi_stmt(gsi);

    switch (gimple_code(stmt)) {
      case GIMPLE_ASSIGN: {
        stmts.push_back(TraverseGimpleAssignStatement(stmt));
        break;
      }

      default: {
        // TODO
      }
    }
  }

  return bb_obj;
}

unsigned int PrintPass::execute(function* fn) {
  auto fn_obj = boost::json::object{};
  fn_obj[kName] = function_name(fn);

  auto& bbs = (fn_obj[kBasicBlocks] = boost::json::array{}).as_array();
  bbs.reserve(n_basic_blocks_for_fn(fn));

  auto bb = basic_block{};
  FOR_EACH_BB_FN(bb, fn) { bbs.push_back(TraverseBasicBlock(bb)); }

  ir[kFunctions].as_array().push_back(std::move(fn_obj));

  return 0;
}

void PluginStartUnit([[maybe_unused]] void* gcc_data,
                     [[maybe_unused]] void* user_data) {
  ir[kFunctions] = boost::json::array{};
}

void PluginFinish([[maybe_unused]] void* gcc_data,
                  [[maybe_unused]] void* user_data) {
  std::cout << boost::json::serialize(ir) << "\n";
}

}  // namespace

int plugin_init(struct plugin_name_args* plugin_info,
                struct plugin_gcc_version* version) {
  if (!plugin_default_version_check(version, &gcc_version)) {
    std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR
              << "." << GCCPLUGIN_VERSION_MINOR << "\n";
    return 1;
  }

  auto plugin_additonal_info = ::plugin_info{
      .version = "1.0",
      .help = "GCC GIMPLE/IR print plugin",
  };

  register_callback(plugin_info->base_name, PLUGIN_INFO, nullptr,
                    &plugin_additonal_info);

  auto pass_info = register_pass_info{
      .pass = kPrintPass.get(),
      .reference_pass_name = "ssa",
      .ref_pass_instance_number = 1,
      .pos_op = PASS_POS_INSERT_AFTER,
  };

  register_callback(plugin_info->base_name, PLUGIN_START_UNIT, PluginStartUnit,
                    nullptr);
  register_callback(plugin_info->base_name, PLUGIN_FINISH, PluginFinish,
                    nullptr);
  register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr,
                    &pass_info);

  return 0;
}
