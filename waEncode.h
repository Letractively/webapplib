/// \file waEncode.h
/// ����,�ӽ��ܺ���ͷ�ļ�
/// �ַ���BASE64��URI��MD5���뺯��
   
#ifndef _WEBAPPLIB_ENCODE_H_
#define _WEBAPPLIB_ENCODE_H_ 

#include <string>

using namespace std;

// WEB Application Develop Library namaspace
namespace webapp {

/// URI����
string uri_encode( const string &source );
/// URI����
string uri_decode( const string &source );

/// �ַ���MIME BASE64����
string base64_encode( const string &source );
/// �ַ���MIME BASE64����
string base64_decode( const string &source );

/// MD5����
string md5_encode( const string &source );

} // namespace

#endif //_WEBAPPLIB_ENCODE_H_
