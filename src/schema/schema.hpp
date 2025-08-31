#include "../include.hpp"

#ifndef S2_PAD
	#define S2_PAD_INSERT(x, y) x ## y
	#define S2_PAD_DEFINE(x, y) S2_PAD_INSERT(x, y)
	#define S2_PAD(size) char S2_PAD_DEFINE(padding_, __LINE__)[size]
#endif

class CSchemaField
{
public:
	const char* m_szName;
	void* m_pType;
	uint32_t m_nOffset;
	uint32_t m_nMetadataSize;
	void* m_nMetadata;
};

class CSchemaClass
{
public:
	void* vfptr;
	const char* m_szName;
	const char* m_szModuleName;
	uint32_t m_nSize;
	uint16_t m_nNumFields;

	S2_PAD(0x2);

	uint16_t m_nStaticSize;
	uint16_t m_nMetadataSize;

	S2_PAD(0x4);

	CSchemaField* m_pFields;
};

class CSchemaDeclaredClass
{
public:
	void* vfptr;
	const char* m_szName;
	const char* m_szModuleName;
	const char* m_szUnknownStr;
	CSchemaClass* m_Class;
};

class CSchemaDeclaredClassEntry
{
public:
	uint64_t m_nHash[2];
	CSchemaDeclaredClass* m_pDeclaredClass;
};

class CSchemaSystemTypeScope
{
public:
	void* vfptr; 							// 0x000
	char m_szName[256]; 					// 0x008
	CSchemaSystemTypeScope* pGlobalScope; 	// 0x108
	S2_PAD(0x338);
	CSchemaDeclaredClassEntry* m_pDeclaredClasses; // 0x448
};

class CSchemaSystem
{
public:
	S2_PAD(0x188);

	uint64_t m_nScopeSize;
	CSchemaSystemTypeScope** m_pScopeArray;
	
	inline CSchemaSystemTypeScope* GetScopeEntry(int idx) {
		return m_pScopeArray[idx];
	}
};

class CSchemaDumper
{
public:
	CSchemaSystem* m_pSchemaSystem;

	void Init(const char* szInstallPath);
	void InstallSchemaBindings(std::string szModulePath, std::string szModuleName);

};
inline std::unique_ptr<CSchemaDumper> g_pSchemaDumper;