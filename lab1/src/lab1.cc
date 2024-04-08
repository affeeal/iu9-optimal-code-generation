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

#include "gcc-plugin.h"  // must be the first gcc header to be included
#include "plugin-version.h"

int plugin_is_GPL_compatible;  // asserts the plugin is licensed under the
                               // GPL-compatible license

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version) {
  if (!plugin_default_version_check(version, &gcc_version)) {
    return 1;
  }

  std::cout << "Hello, GCC!\n";
  return 0;
}
