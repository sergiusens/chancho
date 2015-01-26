/*
 * Copyright (c) 2014 Manuel de la Peña <mandel@themacaque.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <vector>

#define DECLARE_STATIC_INIT(ClassName) \
    static void static_init_func(); \
    static static_init_helper ClassName##_static_init_helper

#define STATIC_INIT(ClassName) \
    static_init_helper ClassName::ClassName##_static_init_helper(&ClassName::static_init_func); \
    void ClassName::static_init_func()

typedef void (*init_func_type)();

namespace com {

namespace chancho {

class static_init {
 public:
    static static_init& instance() {
        static static_init inst;
        return inst;
    }

    void add_init_func(init_func_type f) {
        funcs_.push_back(f);
    }

    static void execute() {
        auto& inst = instance();
        for (auto c : inst.funcs_) c();
    }

 private:
    static_init() {
    }

    std::vector<init_func_type> funcs_;
};

class static_init_helper {
 public:
    static_init_helper(init_func_type f) {
        static_init::instance().add_init_func(f);
    }
};

}

}
