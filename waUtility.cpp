/// \file waUtility.cpp
/// ϵͳ���ù��ߺ���ʵ���ļ�

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <set>

// for host_addr()
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "waString.h"
#include "waDateTime.h"
#include "waUtility.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// \ingroup waUtility 
/// \fn unsigned int string_hash( const string &str )
/// �����ַ���HASHֵ������DJB HASH�㷨
/// Perl����ʵ�ְ汾 string_hash.pl
/// JavaScript����ʵ�ְ汾 string_hash.js
/// \param str Դ�ַ���
/// \return �ַ���HASH������޷�������
unsigned int string_hash( const string &str ) {
	unsigned char ch;
	unsigned int hash = 5381;
	int len = str.length();
	char *p = const_cast<char*>( str.c_str() );
	
	while ( len > 0 ) {
		ch = *p++ - 'A';
		if ( ch <= ('Z'-'A') )
			ch += ('a'-'A');
		hash = ( (hash<<5) + hash ) ^ ch;
		--len;
	}
	return hash;
}	

/// \ingroup waUtility 
/// \fn string replace_text( const string &text, const map<string,string> &replace )
/// ȫ�Ĵʱ��滻������GBK����
/// \param text �ַ���ԭ��
/// \param replace �滻��Ӧ�ʱ�
/// \return �滻����
string replace_text( const string &text, const map<string,string> &replace ) {
	// replace size list 
	map<string,string>::const_iterator it = replace.begin();
	set<size_t> size_set;
	for ( ; it!=replace.end(); ++it ) {
		if ( (it->first) != "" ) {
			size_set.insert( (it->first).length() );
		}
	}
	vector<size_t> size_list( size_set.begin(), size_set.end() );
	std::sort( size_list.begin(), size_list.end() );
	
	// repalce loop
	size_t p = 0;
	string token, result;
	result.reserve( text.length()*2 );

	while ( p < text.length() ) {
		for ( size_t i=0; i<size_list.size(); ++i ) {
			size_t s = size_list[i];

			// next token
			size_t border = p + s;
			if ( text.length()>border && isgbk(text[border-1],text[border]) )
				token = String(text).w_substr( p, s );
			else
				token = text.substr( p, s );

			it = replace.find( token );

			// found
			if ( it != replace.end() ) {
				result.append( it->second );
				p += token.length();
				break;
			}

			// not found
			if ( i == size_list.size()-1 ) {
				if ( text.length()>(p+1) && isgbk(text[p],text[p+1]) ) {
					// step next GBK word
					result += text[p];
					result += text[p+1];
					p += 2;
				} else {
					// step next char
					result += text[p];
					p += 1;
				}
			}
		}
	};
	
	return result;
}

// �ж��Ƿ�������ַ�
bool is_punctuation( unsigned char c ) {
	const char filter_char[] = "`[]~@^_{}|\\";
	if ( strchr(filter_char,c) != NULL )
		return true;
	else
		return false;
}

// ȫ����ĸ�����֡���㡢�հ��ַ�ת��Ϊ����ַ�
string sbc_to_dbc( const string &sbc_string ) {
	string dbc;
	if ( sbc_string == "" ) return dbc;
	
	char sbc_chr[3];
	unsigned int len = sbc_string.length();

	dbc.reserve( len );
	for ( unsigned int i=0; i<len; ++i ) {
		if ( i<(len-1) && isgbk(sbc_string[i],sbc_string[i+1]) ) {
			// double byte char
			sbc_chr[0] = sbc_string[i];
			sbc_chr[1] = sbc_string[++i];
			sbc_chr[2] = '\0';
			
			bool chinese_char = true;
			for ( int j=0; j<SDBC_TABLE_SIZE; ++j ) {
				if ( strncmp(sbc_chr,SBC_TABLE[j],3) == 0 ) {
					// alpha, digit, punct, space
					dbc += DBC_TABLE[j];
					chinese_char = false;
					break;
				}
			}
			if ( chinese_char )
				dbc += sbc_chr;
		} else {
			// single byte char
			dbc += sbc_string[i];
		}
	}
	
	return dbc;
}

/// \ingroup waUtility 
/// \fn string extract_html( const string &html )
/// ��ȡHTML��������
/// \param html HTML�����ַ���
/// \return ����HTML�������ȡ���
string extract_html( const string &html ) {
	bool in_html = false;
	string text, curr_tag;
	text.reserve( html.length() );
	
	for ( unsigned int i=0; i<html.length(); ++i ) {
		if ( !in_html && html[i]=='<' ) {
			// <...
			in_html = true;
			curr_tag = "<";
		} else if ( in_html && html[i]=='>' ) {
			// >...
			in_html = false;
		} else if ( !in_html ) {
			// ...
			text += html[i];
		} else if ( in_html ) {
			// <...>
			curr_tag += html[i];
		}
	}

	if ( in_html ) // unclosed html code
		text += curr_tag;
	return text;
}

/// \ingroup waUtility 
/// \fn string extract_text( const string &text, const int option, const size_t len )
/// ȫ�ǰ���ַ�ת������ȡ����
/// \param text Դ�ַ���
/// \param option ���˷�Χѡ���ѡֵ�����
/// - EXTRACT_ALPHA ������ĸ
/// - EXTRACT_DIGIT ��������
/// - EXTRACT_PUNCT ���˱��
/// - EXTRACT_SPACE ���˿հ�
/// - EXTRACT_HTML ����HTML����
/// - Ĭ��ֵΪEXTRACT_ALL������ȫ��
/// \param len ���˳��ȣ�����0ʱֻ��ȡǰlen����Ч�ַ���Ĭ��Ϊ0
/// \return ת����ȡ����ַ�������Դ�ַ������ݱ�ȫ�������򷵻ؿ�
string extract_text( const string &text, const int option, const size_t len ) {
	if ( text=="" || option<=0 )
		return text;
	
	string converted = sbc_to_dbc( text );
	
	// is HTML
	if ( option&EXTRACT_HTML )
		converted = extract_html( converted );
	if ( option == EXTRACT_HTML )
		return converted;

	string extracted;
	extracted.reserve( text.length() );
	
	for ( unsigned int i=0; i<converted.length(); ++i ) {
		unsigned char c = converted[i];
		if ( isalpha(c) )
			c = tolower( c );
		
		// is GBK char
		if ( !is_punctuation(c) && !isalpha(c) && 
			 ((c>=0x81&&c<=0xFE) || (c>=0x40&&c<=0x7E) || (c>=0xA1&&c<=0xFE)) )
			extracted += c;
		// is alpha
		else if ( option&EXTRACT_ALPHA && isalpha(c) )
			continue;
		// is digit
		else if ( option&EXTRACT_DIGIT && isdigit(c) )
			continue;
		// is punct
		else if ( option&EXTRACT_PUNCT && (ispunct(c)||is_punctuation(c)) )
			continue;
		// is space
		else if ( option&EXTRACT_SPACE && (isspace(c)||isblank(c)) )
			continue;
		// other 
		else
			extracted += c;
		
		// enough
		if ( len>0 && extracted.length()>=len )
			break;
	}
	
	return extracted;
}

// �ײ���־����ʵ��
void _file_logger( FILE *fp, va_list ap, const char *format ) {
	if ( fp == NULL ) return;
	
	// prefix datetime
	DateTime now;
	String logformat;
	logformat.sprintf( "%s\t%s", now.datetime().c_str(), format );

	// log content
	vfprintf( fp, logformat.c_str(), ap );
	fputc( '\n', fp );
}

/// \ingroup waUtility 
/// \fn void file_logger( const string &file, const char *format, ... )
/// ׷����־��¼
/// \param file ��־�ļ�·��
/// \param format ��־�и�ʽ
/// \param ... ��־���ݲ����б�
void file_logger( const string &file, const char *format, ... ) {
	if ( file == "" ) return;
	
	FILE *fp = fopen( file.c_str(), "a" );
	if ( fp != NULL ) {
		va_list ap;
		va_start( ap, format );
		_file_logger( fp, ap, format );
		va_end( ap );
		fclose( fp );
	}
}

/// \ingroup waUtility 
/// \fn void file_logger( FILE *fp, const char *format, ... )
/// ׷����־��¼
/// \param fp ��־�ļ����������stdout/stderr
/// \param format ��־�и�ʽ
/// \param ... ��־���ݲ����б�
void file_logger( FILE *fp, const char *format, ... ) {
	if ( fp == NULL ) return;
	
	va_list ap;
	va_start( ap, format );
	_file_logger( fp, ap, format );
	va_end( ap );
}

/// \ingroup waUtility 
/// \fn string system_command( const string &cmd )
/// ִ�������������������
/// \param cmd �����ַ��������������в���
/// \return ����ִ��������
string system_command( const string &cmd ) {
	string res;
	if ( cmd == "" ) return res;
	
	char buf[256] = {0};
	FILE *pp = popen( cmd.c_str(), "r" );
	if( pp==NULL || pp==(void*)-1 )
		return res;
	while ( fgets(buf,sizeof(buf),pp) )
		res += buf;
		
	pclose( pp );
	return res;
}

/// \ingroup waUtility 
/// \fn string host_addr( const string &interface )
/// ����ָ�������豸�󶨵�IP��ַ
/// \param interface �����豸����Ĭ��Ϊ"eth0"
/// \return ָ�������豸�󶨵�IP��ַ
string host_addr( const string &interface ) {
	int fd;
	if ( (fd=socket(AF_INET,SOCK_DGRAM,0)) < 0 )
		return string("");
	
	ifreq ifr;
	memset( &ifr, 0, sizeof(ifr) );
	strncpy( ifr.ifr_name, interface.c_str(), sizeof(ifr.ifr_name)-1 );

	sockaddr_in *sin;
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	sin->sin_family = AF_INET;
	
	if ( ioctl(fd,SIOCGIFADDR,&ifr) < 0 ) {
		close( fd );
		return string("");
	}
	
	static char buf[256] = {0};
	if( inet_ntop(AF_INET,(void *)&sin->sin_addr,buf,sizeof(buf)-1) < 0 ) {
		close( fd );
		return string("");
	}
	
	close( fd );
	return string(buf);
}

} // namespace

