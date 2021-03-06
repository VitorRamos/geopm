/*
 * Copyright (c) 2015, 2016, 2017, 2018, 2019, 2020, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <functional>
#include <mutex>

#include "IOGroup.hpp"

#include "geopm_plugin.hpp"
#include "MSRIOGroup.hpp"
#include "CpuinfoIOGroup.hpp"
#include "TimeIOGroup.hpp"
#include "ProfileIOGroup.hpp"
#include "Helper.hpp"
#include "config.h"
#ifdef GEOPM_CNL_IOGROUP
#include "CNLIOGroup.hpp"
#endif
#ifdef GEOPM_DEBUG
#include <iostream>
#endif


namespace geopm
{
    const std::string IOGroup::M_PLUGIN_PREFIX = "libgeopmiogroup_";


    IOGroupFactory::IOGroupFactory()
    {
        register_plugin(MSRIOGroup::plugin_name(),
                        MSRIOGroup::make_plugin);
        register_plugin(TimeIOGroup::plugin_name(),
                        TimeIOGroup::make_plugin);
        register_plugin(CpuinfoIOGroup::plugin_name(),
                        CpuinfoIOGroup::make_plugin);
        register_plugin(ProfileIOGroup::plugin_name(),
                        ProfileIOGroup::make_plugin);
#ifdef GEOPM_CNL_IOGROUP
        register_plugin(CNLIOGroup::plugin_name(),
                        CNLIOGroup::make_plugin);
#endif
    }


    IOGroupFactory &iogroup_factory(void)
    {
        static IOGroupFactory instance;
        static bool is_once = true;
        static std::once_flag flag;
        if (is_once) {
            is_once = false;
            std::call_once(flag, plugin_load, IOGroup::M_PLUGIN_PREFIX);
        }
        return instance;
    }


    std::vector<std::string> IOGroup::iogroup_names(void)
    {
        return iogroup_factory().plugin_names();
    }


    std::unique_ptr<IOGroup> IOGroup::make_unique(const std::string &iogroup_name)
    {
        return iogroup_factory().make_plugin(iogroup_name);
    }


    std::function<std::string(double)> IOGroup::format_function(const std::string &signal_name) const
    {
#ifdef GEOPM_DEBUG
        static bool is_once = true;
        if (is_once) {
            is_once = false;
            std::cerr << "Warning: <geopm> Use of geopm::IOGroup::format_function() is deprecated, each IOGroup will be required implement this method in the future.\n";
        }
#endif
        std::function<std::string(double)> result = string_format_double;
        if (string_ends_with(signal_name, "#")) {
            result = string_format_raw64;
        }
        return result;
    }
}
