#include "fafileobject.h"
#include <iostream>


FAIO::FAFileObject::FAFileObject(const std::string pathFile)
{
    faFile = FAIO::FAfopen(pathFile);
}

FAIO::FAFileObject::~FAFileObject()
{
    if (faFile)
    {
        FAfclose(faFile);
        faFile = NULL;
    }
}

bool FAIO::FAFileObject::isValid() {
    return faFile != NULL;
}


bool FAIO::FAFileObject::exists(const std::string & filename)
{
    return FAIO::exists(filename);
}


size_t FAIO::FAFileObject::FAfread(void * ptr, size_t size, size_t count)
{
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::FAfread: faFile is NULL." << std::endl;
        return 0;
    }
    return FAIO::FAfread(ptr, size, count, faFile);
}

int FAIO::FAFileObject::FAfseek(size_t offset, int origin)
{
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::FAfseek: faFile is NULL." << std::endl;
        return 0;
    }
    return FAIO::FAfseek(faFile, offset, origin);
}

size_t FAIO::FAFileObject::FAftell()
{
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::FAftell: faFile is NULL." << std::endl;
        return 0;
    }
    return FAIO::FAftell(faFile);
}

size_t FAIO::FAFileObject::FAsize()
{
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::FAsize: faFile is NULL." << std::endl;
        return 0;
    }
    return FAIO::FAsize(faFile);
}


uint32_t FAIO::FAFileObject::read32() {
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::read32: faFile is NULL." << std::endl;
        return 0;
    }
    return FAIO::read32(faFile);
}


uint16_t FAIO::FAFileObject::read16() {
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::read16: faFile is NULL." << std::endl;
        return 0;
    }
    return FAIO::read16(faFile);
}

uint8_t FAIO::FAFileObject::read8() {
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::read8: faFile is NULL." << std::endl;
        return 0;
    }
    return FAIO::read8(faFile);
}


std::string FAIO::FAFileObject::readCString(size_t ptr) {
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::read8: faFile is NULL." << std::endl;
        return std::string("");
    }
    return FAIO::readCString(faFile, ptr);
}


std::string FAIO::FAFileObject::readCStringFromWin32Binary(size_t ptr, size_t offset) {
    if (!faFile)
    {
        std::cerr << "FAFileWrapper::readCStringFromWin32Binary: faFile is NULL." << std::endl;
        return std::string("");
    }
    return FAIO::readCStringFromWin32Binary(faFile, ptr, offset);
}


std::string FAIO::FAFileObject::getMPQFileName() {
    return FAIO::getMPQFileName();
}


void FAIO::FAFileObject::quit() {
    FAIO::quit();
}
