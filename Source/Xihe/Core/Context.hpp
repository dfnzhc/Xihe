/**
 * @File Context.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */
 
#pragma once

#include <memory>
#include "Core/Base/Defines.hpp"

namespace xihe {

class Logger;

class XIHE_API Context
{
public:
    Context();
    ~Context();

    const Logger* getLogger() const;

    // -----------------------------
    static bool Create();
    static void Destroy();
    
    static Context& Get();
    
private:
    inline static Context* sInstance = nullptr;
    
    std::unique_ptr<Logger> _logger;
};

} // namespace xihe