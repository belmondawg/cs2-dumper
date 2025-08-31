#include "schema.hpp"
#include "../logger.hpp"

typedef uintptr_t(*_CreateInterface)(const char* szInterfaceName, int nUnknown);
typedef uintptr_t(*_InstallSchemaBindings)(const char* szInterfaceName, CSchemaSystem* pSchemaSystem);

void CSchemaDumper::Init(const char* szInstallPath)
{
    std::string szDependencyPath = std::string(szInstallPath) + "\\game\\bin\\win64";
    SetDllDirectoryA(szDependencyPath.c_str());

    std::string szSchemaPath = szDependencyPath + "\\schemasystem.dll";

    HMODULE hSchemaModule = LoadLibraryA(szSchemaPath.c_str());
    if (!hSchemaModule) 
        g_pLogger->Log(LogLevel::Error, "error loading schemasystem.dll: %i", GetLastError());

    _CreateInterface CreateInterface = (_CreateInterface)GetProcAddress(hSchemaModule, "CreateInterface");
    m_pSchemaSystem = (CSchemaSystem*)CreateInterface("SchemaSystem_001", NULL);

    FreeLibrary(hSchemaModule);
    g_pLogger->Log(LogLevel::Info, "created interface: 0x%llx", m_pSchemaSystem);

    szSchemaPath = szInstallPath + std::string("\\game\\bin\\win64\\");
    InstallSchemaBindings(szSchemaPath.c_str(), "schemasystem.dll");
}

void CSchemaDumper::InstallSchemaBindings(std::string szModulePath, std::string szModuleName)
{
    std::string szFullModulePath = szModulePath + szModuleName;

    HMODULE hModule = LoadLibraryA(szFullModulePath.c_str());
    if (!hModule) 
        g_pLogger->Log(LogLevel::Error, "error loading %s: %i", szModuleName.c_str(), GetLastError());

    _InstallSchemaBindings InstallSchemaBindings = (_InstallSchemaBindings)GetProcAddress(hModule, "InstallSchemaBindings");
    
    auto res = InstallSchemaBindings("SchemaSystem_001", m_pSchemaSystem);
    if (res != 0x00000000C0000001) 
	    g_pLogger->Log(LogLevel::Error, "error installing bindings for %s: %i", szModuleName.c_str(), res);
    
    g_pLogger->Log(LogLevel::Info, "installed bindings for: %s", szModuleName.c_str());
}