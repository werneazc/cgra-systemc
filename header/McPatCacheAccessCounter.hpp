/*
 * McPatCacheAccessCounter.hpp
 *
 *  Created on: Mar 14, 2021
 *      Author: andrewerner
 */

#ifndef MCPAT_CACHE_ACCESS_COUNTER_H_
#define MCPAT_CACHE_ACCESS_COUNTER_H_

#include <iostream>

namespace cgra
{

struct McPatCacheAccessCounter
{
protected:
    std::size_t m_readAccessCounter{0};
    std::size_t m_writeAccessCounter{0};

public:
    /**
     * \brief Dump runtime statistics for McPAT simulation
     *
     * \param os Define used outstream [default: std::cout]
     */
    virtual void dumpMcpatStatistics(std::ostream &os = ::std::cout) const = 0;
};

} // namespace cgra

#endif // MCPAT_CACHE_ACCESS_COUNTER_H_
