/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
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
 *
 */

#include "FAIOFileInterface.h"
#include <faio/faio.h>

FAIOFileInterface::~FAIOFileInterface()
{
}

// Opens a file.
Rocket::Core::FileHandle FAIOFileInterface::Open(const Rocket::Core::String& path)
{
	FAIO::FAFile* fp = FAIO::FAfopen(path.CString());
    return (Rocket::Core::FileHandle) fp;
}

// Closes a previously opened file.
void FAIOFileInterface::Close(Rocket::Core::FileHandle file)
{
	FAIO::FAfclose((FAIO::FAFile*) file);
}

// Reads data from a previously opened file.
size_t FAIOFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
{
	return FAIO::FAfread(buffer, 1, size, (FAIO::FAFile*) file);
}

// Seeks to a point in a previously opened file.
bool FAIOFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
{
	return FAIO::FAfseek((FAIO::FAFile*) file, offset, origin) == 0;
}

// Returns the current position of the file pointer.
size_t FAIOFileInterface::Tell(Rocket::Core::FileHandle file)
{
	return FAIO::FAftell((FAIO::FAFile*) file);
}
