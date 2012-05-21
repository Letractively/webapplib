/// \file waFileSystem.h
/// �ļ���������ͷ�ļ�
/// �����ļ�����

#ifndef _WEBAPPLIB_FILE_H_
#define _WEBAPPLIB_FILE_H_ 

#include <cstdio>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

// WEB Application Library namaspace
namespace webapp {

/// �ļ�����Ŀ¼�Ƿ����
bool file_exist( const string &file );

/// �ļ��Ƿ�Ϊ����
bool is_link( const string &file );
/// �Ƿ�ΪĿ¼
bool is_dir( const string &file );

/// ��������
bool make_link( const string &srcfile, const string &destfile );

/// ȡ���ļ���С
long file_size( const string &file );
/// ȡ���ļ�����ʱ��
long file_time( const string &file );
/// ȡ���ļ�·��
string file_path( const string &file );
/// ȡ���ļ�����
string file_name( const string &file );

/// �ļ�����Ŀ¼����
bool rename_file( const string &oldname, const string &newname );
/// �����ļ�	
bool copy_file( const string &srcfile, const string &destfile );
/// ɾ���ļ�
bool delete_file( const string &file );
/// �ƶ��ļ�
bool move_file( const string &srcfile, const string &destfile );

/// ����Ŀ¼�ļ��б�
vector<string> dir_files( const string &dir );
/// ����Ŀ¼
bool make_dir( const string &dir, 
			   const mode_t mode = S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
/// ����Ŀ¼
bool copy_dir( const string &srcdir, const string &destdir );
/// ɾ��Ŀ¼
bool delete_dir( const string &dir );
/// �ƶ�Ŀ¼
bool move_dir( const string &srcdir, const string &destdir );
			  
/// �ļ����������
void lock_file( int fd, const int type );
/// �ļ����������
inline void lock_file( FILE *fp, const int type ) {
	lock_file( fileno(fp), type );
}

/// �ж��ļ������
bool is_locked( int fd );
/// �ж��ļ������
inline bool is_locked( FILE *fp ) {
	return is_locked( fileno(fp) );
}

/// �����������ļ�
FILE* lock_open( const string &file, const char* mode, const int type );

} // namespace

#endif //_WEBAPPLIB_FILE_H_


