  // StringFile.h: interface for the CStringFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGFILE_H__A6247296_4E48_11D2_BF32_0040333952B6__INCLUDED_)
#define AFX_STRINGFILE_H__A6247296_4E48_11D2_BF32_0040333952B6__INCLUDED_

#define SFBUF_SIZE	10240  //1条记录限制在10K,Read缓冲在64K内
#define SFBUF_OPTI	2048000  //删除、排序用的最大缓冲2M

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringFile  
{
public:
	BOOL DeleteLine(CString szLine);
	CString FindString(CString szFind,CString szPart);
	BOOL DecryptByte(char* byDecryptLine,int nByteSize);
	BOOL EncryptByte(char* byEncryptLine,int nByteSize);
	DWORD WriteNextLine(CString &szLine,BOOL IsEncypt);
	CStringFile(int nBufSize = SFBUF_SIZE);
	virtual ~CStringFile();
	BOOL Open(LPCSTR szFile, CFileException *feError=NULL);
	void Close();
	void Reset(void);
	void GetErrorText(CString &szMsg);
	DWORD GetNextLine(LPSTR szLine,int iLineSize);
	DWORD GetNextLine(CString &szLine,BOOL IsEncypt);

protected:
	int m_nBufferSize;
	CString m_szError;
	DWORD	m_nMaxSize;
	DWORD	m_dwRead;
	DWORD	m_dwLine;
	DWORD	m_dwMasterIndex;
	DWORD	m_dwIndex;
	int		m_nSectionCount;
	BYTE	*m_pBuffer;
	CFile	m_fFile;
};

#endif // !defined(AFX_STRINGFILE_H__A6247296_4E48_11D2_BF32_0040333952B6__INCLUDED_)
