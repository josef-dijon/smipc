/* MIT License
 *
 * Copyright (c) 2024 Josef de Joanelli (josef@pixelrift.io)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ABSTRACT_SHARED_MEMORY_H_
#define ABSTRACT_SHARED_MEMORY_H_

#include <libsmipc/shared-memory/shared-memory-view.hpp>

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

class ISharedMemory
{
public:
	virtual ~ISharedMemory() = default;

	virtual void create(const std::string& name, std::size_t size) = 0;
	virtual void open(const std::string& name) = 0;
	virtual void close() = 0;
	virtual void closeAll() = 0;

	virtual auto getName() const -> std::string_view = 0;
	virtual auto getSize() const -> std::size_t = 0;
	virtual auto getView() -> SharedMemoryView = 0;
	virtual auto getView() const -> const SharedMemoryView = 0;
};

#endif  // ABSTRACT_SHARED_MEMORY_H_
