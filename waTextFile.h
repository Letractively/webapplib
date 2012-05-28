/// \file waTextFile.h
/// �̶��ָ����ı��ļ���ȡ������ͷ�ļ�
/// ��ȡ�����̶��ָ����ı��ļ�
/// ������ webapp::String

#ifndef _WEBAPPLIB_TEXTFILE_H_
#define _WEBAPPLIB_TEXTFILE_H_ 

#include <cstdio>
#include "waString.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// �̶��ָ����ı��ļ���ȡ������
class TextFile {
	public:
		
	/// Ĭ�Ϲ��캯��
	TextFile():_fp(0), _line(0), _len(0)
	{};
	
	/// ����Ϊ�ı��ļ����Ĺ��캯��
	TextFile( const string &file )
	:_fp(0), _line(0), _len(0) {
		this->open( file );
	}
	
	/// ��������
	~TextFile() {
		this->close();
	}
	
	/// ���ı��ļ�
	bool open( const string &file );
	
	/// �ر��ı��ļ�
	void close();
	
	/// ��ȡ��һ��
	bool next_line( string &line );
	
	/// ��ȡ��һ�в����ָ�������ֶ�
	bool next_fields( vector<String> &fields, const string &split="\t", const int limit=0 );
	
	////////////////////////////////////////////////////////////////////////////
	private:
		
	FILE *_fp;
	char *_line;
	size_t _len;
};
	
} // namespace

#endif //_WEBAPPLIB_TEXTFILE_H_

