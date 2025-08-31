#include "src/schema/schema.hpp"
#include "src/logger.hpp"

int main()
{
    g_pSchemaDumper = std::make_unique<CSchemaDumper>();
    g_pLogger = std::make_unique<CLogger>();

    g_pLogger->ClearConsole();

    HKEY hKey;
    char szBuffer[512];
    DWORD dwSize = sizeof(szBuffer), type;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Valve\\cs2", 0, KEY_READ | KEY_WOW64_32KEY, &hKey) != ERROR_SUCCESS)
        g_pLogger->Log(LogLevel::Error, "error opening key: %i", GetLastError());

    if (RegQueryValueExA(hKey, "installpath", 0, &type, (LPBYTE)szBuffer, &dwSize) != ERROR_SUCCESS || type != REG_SZ) {
        RegCloseKey(hKey);
        g_pLogger->Log(LogLevel::Error, "error reading key: %i", GetLastError());
    }

    RegCloseKey(hKey);

    g_pSchemaDumper->Init(szBuffer);  
    
    std::vector<std::pair<std::string, std::string>> vecModules = {
        { "\\game\\bin\\win64\\", "engine2.dll" },
        { "\\game\\csgo\\bin\\win64\\", "client.dll" },
        { "\\game\\csgo\\bin\\win64\\", "server.dll" }
    };

    for (const auto& [szModulePath, szModuleName] : vecModules) {
        std::string fullPath = std::string(szBuffer) + szModulePath;
        g_pSchemaDumper->InstallSchemaBindings(fullPath.c_str(), szModuleName.c_str());
    }

    char szPath[MAX_PATH];
    GetModuleFileNameA(NULL, szPath, MAX_PATH);
    std::string szDir(szPath);
    szDir = szDir.substr(0, szDir.find_last_of("\\/"));

    CreateDirectoryA((szDir + "\\output").c_str(), NULL);

    for (int i = 0; i < g_pSchemaDumper->m_pSchemaSystem->m_nScopeSize; i++) {
		auto currentScope = g_pSchemaDumper->m_pSchemaSystem->m_pScopeArray[i];

        std::string szScopeName = currentScope->m_szName;
        size_t nPos = szScopeName.find(".dll");
        if (nPos != std::string::npos) {
            szScopeName.replace(nPos, 4, "_dll");
        }

        std::string szFileName = "output/" + szScopeName + ".h";

        std::ofstream outfile(szFileName);
        outfile << "#include <cstddef>\n\n";

        for (int i = 0; i < 999; i++) {
            CSchemaDeclaredClassEntry& classDef = currentScope->m_pDeclaredClasses[i];

            if (!classDef.m_pDeclaredClass)
                break;

            if (!classDef.m_pDeclaredClass->m_szName)
                break;
            
            auto classData = classDef.m_pDeclaredClass->m_Class;
            if (!classData)
                continue;

            if (!classData->m_nNumFields)
                continue;

            outfile << "namespace " << classData->m_szName << " {\n";
            for (int i = 0; i < classData->m_nNumFields; i++) 
            {
                auto field = classData->m_pFields[i];
                outfile << "    constexpr std::ptrdiff_t " << field.m_szName << " = 0x" << std::hex << field.m_nOffset << "; \n";
            }
            outfile << "}\n\n";
        }

        g_pLogger->Log(LogLevel::Info, "saving classes from %s to %s", currentScope->m_szName, szFileName.c_str());
	}
}