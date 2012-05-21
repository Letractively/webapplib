/// \file waString.h
/// String��ͷ�ļ�
/// �̳���std::string���ַ�����
/// <a href=std_string.html>����std::stringʹ��˵���ĵ�</a>

#ifndef _WEBAPPLIB_STRING_H_
#define _WEBAPPLIB_STRING_H_ 

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

// WEB Application Library namaspace
namespace webapp {
	
////////////////////////////////////////////////////////////////////////////////	
// �հ��ַ��б�
const char BLANK_CHARS[] = " \t\n\r\v\f";
// CGI�����ַ��б�
const char CGI_SENSITIVE[] = "&;`'\\\"|*?~<>^()[]{}$\n\r\t\0#./%";

////////////////////////////////////////////////////////////////////////////////
/// long intת��Ϊstring
string itos( const long i, const ios::fmtflags base = ios::dec );
/// stringת��Ϊint
long stoi( const string &s, const ios::fmtflags base = ios::dec );
/// doubleת��Ϊstring
string ftos( const double f, const int ndigit = 2 );
/// stringת��Ϊdouble
double stof( const string &s );
/// �ж�һ��˫�ֽ��ַ��Ƿ���GBK���뺺��
bool isgbk( const unsigned char c1, const unsigned char c2 );
/// �ɱ�����ַ�����ʽ������va_start()��va_end()�����ʹ��
string va_sprintf( va_list ap, const string &format );
/// ��ʽ���ַ���������
string va_str( const char *format, ... );

////////////////////////////////////////////////////////////////////////////////
/// �̳���std::string���ַ�����
/// <a href="std_string.html">����std::stringʹ��˵���ĵ�</a>
class String : public string {
	public:
	
	////////////////////////////////////////////////////////////////////////////
	/// Ĭ�Ϲ��캯��
	String(){}
	
	/// ����Ϊchar*�Ĺ��캯��
	String( const char *s ) {
		if( s ) this->assign( s );
		else this->erase();
	}
	
	/// ����Ϊstring�Ĺ��캯��
	String( const string &s ) {
		this->assign( s );
	}
	
	/// ��������
	virtual ~String(){}
	
	////////////////////////////////////////////////////////////////////////////
	/// \enum ����String::split()�ָ������ط�ʽ
	enum split_resmode {
		/// ���Էָ����еĿ��Ӵ�
		SPLIT_ERASE_BLANK,
		/// �����ָ����еĿ��Ӵ�
		SPLIT_KEEP_BLANK
	};	
	/// \enum ����String::split()�зֱ�Ƿ�ʽ
	enum split_splmode {
		/// ���зֱ���ַ�����Ϊ�����з�����
		SPLIT_SINGLE,
		/// ���зֱ���ַ�����Ϊ�з���������
		SPLIT_MULTI
	};
	/// \enum ����String::escape_html()ת����ʽ
	enum eschtml_mode {
		/// HTML����ת��Ϊת���
		ESCAPE_HTML,		
		/// ת���ת��ΪHTML����
		UNESCAPE_HTML			
	};	

	////////////////////////////////////////////////////////////////////////////
	/// ���� char* �ͽ���������߱������ delete[] �ͷ��������ڴ�
	char* c_char() const;
	
	/// �����ַ�������֧��ȫ���ַ�
	string::size_type w_length() const;
	/// ��ȡ���ַ�����֧��ȫ���ַ�
	String w_substr( const string::size_type pos = 0, 
					 const string::size_type n = npos ) const;

	/// ������հ��ַ�
	void trim_left( const string &blank = BLANK_CHARS );
	/// ����Ҳ�հ��ַ�
	void trim_right( const string &blank = BLANK_CHARS );
	/// �������հ��ַ�
	void trim( const string &blank = BLANK_CHARS );

	/// ����߽�ȡָ�������Ӵ�
	String left( const string::size_type n ) const;
	/// ���м��ȡָ�������Ӵ�
	String mid( const string::size_type pos, 
				const string::size_type n = npos ) const;
	/// ���ұ߽�ȡָ�������Ӵ�
	String right( const string::size_type n ) const;
	
	/// �����ַ�������
	void resize( const string::size_type n );

	/// ͳ��ָ���Ӵ����ֵĴ���
	int count( const string &str ) const;
	
	/// ���ݷָ���ָ��ַ���
	vector<String> split( const string &tag, 
						  const int limit = 0, 
						  const split_resmode ret = SPLIT_ERASE_BLANK,
						  const split_splmode spl = SPLIT_SINGLE ) const;
	
	/// ת���ַ���ΪHASH�ṹ(map<string,string>)
	map<string,string> tohash( const string &itemtag = "&", 
							   const string &exptag = "=" ) const;

	/// ����ַ���
	void join( const vector<string> &strings, const string &tag );
	void join( const vector<String> &strings, const string &tag );

	/// ��ʽ����ֵ
	bool sprintf( const char *format, ... );
	
	/// �滻
	int replace( const string &oldstr, const string &newstr );
	/// �滻
	inline int replace( const string &oldstr, const long &i ) {
		return this->replace( oldstr, itos(i) );
	}
	
	/// ȫ���滻
	int replace_all( const string &oldstr, const string &newstr );
	/// ȫ���滻
	inline int replace_all( const string &oldstr, const long &i ) {
		return this->replace_all( oldstr, itos(i) );
	}
	
	/// ת��Ϊ��д��ĸ
	void upper();
	/// ת��ΪСд��ĸ
	void lower();
	
	/// �ַ����Ƿ���ȫ���������
	bool isnum() const;
	
	/// ��ȡ�ļ����ַ���
	bool load_file( const string &filename );
	/// �����ַ������ļ�
	bool save_file( const string &filename, 
					const ios::openmode mode = ios::trunc|ios::out,
					const mode_t permission = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH ) const;
	
	/// �����ַ�
	void filter( const string &filter = CGI_SENSITIVE, 
				 const string &newstr = "" );
	
	/// HTML����ת��
	string escape_html( const eschtml_mode mode );
	
	/// ���ַ������Ϊ�ַ���֧��ȫ���ַ�
	vector<string> split_char() const;
};

} // namespace

#endif //_WEBAPPLIB_STRING_H_

