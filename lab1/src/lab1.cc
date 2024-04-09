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

// clang-format off
#include "gcc-plugin.h"  // the first gcc header to be included
//clang-format on

#include "context.h"
#include "plugin-version.h"
#include "tree-pass.h"

int plugin_is_GPL_compatible;  // asserts the plugin is licensed under the
                               // GPL-compatible license

namespace {

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

class PrintPass final : public gimple_opt_pass {
 public:
  PrintPass(gcc::context* ctxt) : gimple_opt_pass(kPrintPassData, ctxt) {}

  PrintPass* clone() override;
  unsigned int execute(function* fun) override;
};

PrintPass* PrintPass::clone() {
  return this;
}

unsigned int PrintPass::execute(function* fun) {
  // TODO
  return 0;
}

const auto kPrintPass = std::make_unique<PrintPass>(g);

}  // namespace

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version) {
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

  register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr,
                    &pass_info);

  return 0;
}
