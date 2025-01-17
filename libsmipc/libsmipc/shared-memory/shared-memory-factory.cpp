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

#include <libsmipc/shared-memory/shared-memory-factory.hpp>

#ifdef _INTIME
#	include <libsmipc/shared-memory/platform/intime-shared-memory.hpp>
#elif _WIN32
#	include <libsmipc/shared-memory/platform/windows-shared-memory.hpp>
#elif __linux__
#	include <libsmipc/shared-memory/platform/posix-shared-memory.hpp>
#endif

std::unique_ptr<ISharedMemory> MakeUniqueSharedMemory()
{
	#ifdef _INTIME
		return std::make_unique<IntimeSharedMemory>();
	#elif _WIN32
		return std::make_unique<WindowsSharedMemory>();
	#elif __linux__
		return std::make_unique<PosixSharedMemory>();
	#endif
}
