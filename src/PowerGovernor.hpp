/*
 * Copyright (c) 2015, 2016, 2017, 2018, 2019, Intel Corporation
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

#ifndef POWERGOVERNOR_HPP_INCLUDE
#define POWERGOVERNOR_HPP_INCLUDE

#include <memory>
#include <vector>

namespace geopm
{
    class PlatformIO;
    class PlatformTopo;

    class PowerGovernor
    {
        public:
            PowerGovernor() = default;
            virtual ~PowerGovernor() = default;
            /// @brief Registsters signals and controls with PlatformIO.
            virtual void init_platform_io(void) = 0;
            /// @brief To be called inside of the Agent's
            ///        sample_platform() method to read any values
            ///        required when calling adjust_platform().  This
            ///        method is currently a noop since no signals are
            ///        required for this algorithm.
            virtual void sample_platform(void) = 0;
            /// @brief Calculates metric of DRAM power history, subtracting that value
            ///        from the provided target node power.
            /// @param [in] node_power_request Total expected node power consumption.
            /// @param [out] node_power_actual Achievable node power limit.  Should equal
            //              node_power_request unless clamped by bounds.
            /// @return True if platform adjustments have been made, false otherwise.
            virtual bool adjust_platform(double node_power_request, double &node_power_actual) = 0;
            /// @brief Sets min and max package bounds.
            /// @param [in] min_pkg_power Minimum package power.
            /// @param [in] max_pkg_power Maximum package power.
            virtual void set_power_bounds(double min_pkg_power, double max_pkg_power) = 0;
            /// @brief Get the time window for controling package power.
            /// @return Time window in units of seconds.
            virtual double power_package_time_window(void) const = 0;
    };

    class PowerGovernorImp : public PowerGovernor
    {
        public:
            PowerGovernorImp(PlatformIO &platform_io, PlatformTopo &platform_topo);
            virtual ~PowerGovernorImp();
            void init_platform_io(void) override;
            virtual void sample_platform(void) override;
            bool adjust_platform(double node_power_request, double &node_power_actual) override;
            void set_power_bounds(double min_pkg_power, double max_pkg_power) override;
            double power_package_time_window(void) const override;
        private:
            PlatformIO &m_platform_io;
            PlatformTopo &m_platform_topo;
            const double M_POWER_PACKAGE_TIME_WINDOW;
            int m_pkg_pwr_domain_type;
            int m_num_pkg;
            const double M_MIN_PKG_POWER_SETTING;
            const double M_MAX_PKG_POWER_SETTING;
            double m_min_pkg_power_policy;
            double m_max_pkg_power_policy;
            std::vector<int> m_control_idx;
            double m_last_pkg_power_setting;
    };
}

#endif
