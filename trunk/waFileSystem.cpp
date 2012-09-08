/// \file waFileSystem.cpp
/// �ļ���������ʵ���ļ�

#include <cstdio>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include "waFileSystem.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \defgroup waFileSystem waFileSystem�ļ�����������
	
/// \ingroup waFileSystem
/// \fn bool file_exist( const string &file )
/// �ļ�����Ŀ¼�Ƿ����
/// \param file �ļ�·����
/// \retval true �ļ�����
/// \retval false ������
bool file_exist( const string &file ) {
	if ( access(file.c_str(),F_OK) == 0 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn bool is_link( const string &file )
/// �ļ��Ƿ�Ϊ����
/// \param file �ļ�·����
/// \retval true �ļ�������Ϊ��������
/// \retval false �����ڻ��߲��Ƿ�������
bool is_link( const string &file ) {
	struct stat statbuf;
	
	if( lstat(file.c_str(),&statbuf) == 0 ) {
		if ( S_ISLNK(statbuf.st_mode) != 0 )
			return true;
	}
	return false;
}

/// \ingroup waFileSystem
/// \fn bool is_dir( const string &file )
/// �Ƿ�ΪĿ¼
/// \param dile Ŀ¼·����
/// \retval true Ŀ¼����
/// \retval false �����ڻ��߲���Ŀ¼
bool is_dir( const string &file ) {
	struct stat statbuf;
	
	if( stat(file.c_str(),&statbuf) == 0 ) {
		if ( S_ISDIR(statbuf.st_mode) != 0 )
			return true;
	}
	return false;
}

/// \ingroup waFileSystem
/// \fn bool make_link( const string &srcfile, const string &destfile )
/// ��������,���������ļ����벻����
/// \param srcfile ԭ�ļ���
/// \param destfile ��������
/// \retval true �����ɹ�
/// \retval false ���ɹ�
bool make_link( const string &srcfile, const string &destfile ) {
	if ( symlink(srcfile.c_str(),destfile.c_str()) == 0 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn long file_size( const string &file )
/// ȡ���ļ���С
/// \param file �ļ�·����
/// \return ���ļ������򷵻ش�С,���򷵻�-1
long file_size( const string &file ) {
	struct stat statbuf;
	
	if( stat(file.c_str(),&statbuf)==0 )
		return statbuf.st_size;
	else		
		return -1;
}

/// \ingroup waFileSystem
/// \fn long file_time( const string &file )
/// ȡ���ļ�����ʱ��
/// \param file �ļ�·����
/// \return ���ļ������򷵻���������ʱ��,���򷵻�-1
long file_time( const string &file ) {
	struct stat statbuf;
	
	if( stat(file.c_str(),&statbuf)==0 )
		return statbuf.st_mtime;
	else		
		return -1;
}

/// \ingroup waFileSystem
/// \fn string file_path( const string &file )
/// ȡ���ļ�·��
/// \param file �ļ�·����
/// \return ����ȡ���ļ�·���򷵻�,���򷵻ؿ��ַ���
string file_path( const string &file ) {
	size_t p;
	if ( (p=file.rfind("/")) != file.npos )
		return file.substr( 0, p );
	else if ( (p=file.rfind("\\")) != file.npos )
		return file.substr( 0, p );
	return string( "" );
}

/// \ingroup waFileSystem
/// \fn string file_name( const string &file )
/// ȡ���ļ�����
/// \param file �ļ�·����
/// \return ����ȡ���ļ������򷵻�,���򷵻�ԭ�ļ�·������
string file_name( const string &file ) {
	size_t p;
	if ( (p=file.rfind("/")) != file.npos )
		return file.substr( p+1 );
	else if ( (p=file.rfind("\\")) != file.npos )
		return file.substr( p+1 );
	return file;
}

/// \ingroup waFileSystem
/// \fn bool rename_file( const string &oldname, const string &newname )
/// �ļ�����Ŀ¼����,���ļ���������ԭ�ļ���λ��ͬһ�ļ�ϵͳ
/// \param oldname ԭ�ļ���
/// \param newname ���ļ���
/// \retval true �����ɹ�
/// \retval false ʧ��
bool rename_file( const string &oldname, const string &newname ) {
	if ( rename(oldname.c_str(),newname.c_str()) != -1 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn bool copy_file( const string &srcfile, const string &destfile )
/// �����ļ�	
/// \param srcfile ԭ�ļ���
/// \param destfile Ŀ���ļ���,�ļ�����Ϊ0666
/// \retval true �����ɹ�
/// \retval false ʧ��
bool copy_file( const string &srcfile, const string &destfile ) {
	FILE *src=NULL, *dest=NULL;
	if ( (src=fopen(srcfile.c_str(),"rb")) == NULL ) {
		return false;
	}
	if ( (dest=fopen(destfile.c_str(),"wb+")) == NULL ) {
		fclose( src );
		return false;
	}
	
	const int bufsize = 8192;
	char buf[bufsize];
	size_t n;
	while ( (n=fread(buf,1,bufsize,src)) >= 1 ) {
		if ( fwrite(buf,1,n,dest) != n ) {
			fclose( src );
			fclose( dest );
			return false;
		}
	}
	
	fclose( src );
	fclose( dest );

	//chmod to 0666
	mode_t mask = umask( 0 );
	chmod( destfile.c_str(), S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
	umask( mask );
	
	return true;
}

/// \ingroup waFileSystem
/// \fn bool delete_file( const string &file )
/// ɾ���ļ�
/// \param file �ļ�·����
/// \retval true ɾ���ɹ�
/// \retval false �ļ������ڻ���ɾ��ʧ��
bool delete_file( const string &file ) {
	if ( remove(file.c_str()) == 0 )
		return true;
	else
		return false;
}

/// \ingroup waFileSystem
/// \fn bool move_file( const string &srcfile, const string &destfile )
/// �ƶ��ļ�
/// \param srcfile ԭ�ļ���
/// \param destfile ���ļ���
/// \retval true �����ɹ�
/// \retval false ʧ��
bool move_file( const string &srcfile, const string &destfile ) {
	if ( rename_file(srcfile,destfile) )
		return true;

	// rename fail, copy and delete file		
	if ( copy_file(srcfile,destfile) ) {
		if ( delete_file(srcfile) )
			return true;
	}
	
	return false;
}

/// \ingroup waFileSystem
/// \fn vector<string> dir_files( const string &dir )
/// ����Ŀ¼�ļ��б�
/// \param dir ����ΪĿ¼·����
/// \return ���ؽ��Ϊ�ļ�����Ŀ¼�б�,��Ŀ¼�ĵ�һ���ַ�Ϊ '/',
/// ���ؽ���в���������ǰ����һ��Ŀ¼�� "/.", "/.."
vector<string> dir_files( const string &dir ) {
	vector<string> files;
	string file;
	DIR *pdir = NULL;
	dirent *pdirent = NULL;
	
	if ( (pdir=opendir(dir.c_str())) != NULL ) {
		while ( (pdirent=readdir(pdir)) != NULL ) {
			file = pdirent->d_name;
			if ( file!="." && file!=".." ) {
				if ( is_dir(dir+"/"+file) ) 
					file = "/"+file;
				files.push_back( file );
			}
		}
		closedir( pdir );
	}
	
	return files;
}

/// \ingroup waFileSystem
/// \fn bool make_dir( const string &dir, const size_t mode )
/// ����Ŀ¼
/// \param dir Ҫ������Ŀ¼,���ϲ�Ŀ¼���������Զ�����
/// \param mode ����Ŀ¼Ȩ��,Ĭ��ΪS_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH(0755)
/// �ɹ�����true, ���򷵻�false.
/// \retval true �����ɹ�
/// \retval false ʧ��
bool make_dir( const string &dir, const mode_t mode ) {
	// check
	size_t len = dir.length();
	if ( len <= 0 )	return false;

	string tomake;
	char curchr;
	for( size_t i=0; i<len; ++i ) {
		// append
		curchr = dir[i];
		tomake += curchr;
		if ( curchr=='/' || i==(len-1) ) {
			// need to mkdir
			if ( !file_exist(tomake) && !is_dir(tomake) ) {
				// able to mkdir
				mode_t mask = umask( 0 );				
				if ( mkdir(tomake.c_str(),mode) == -1 ) {
					umask( mask );
					return false;
				}
				umask( mask );
			}
		}
	}
	
	return true;
}	

/// \ingroup waFileSystem
/// \fn bool copy_dir( const string &srcdir, const string &destdir )
/// ����Ŀ¼,������Ŀ¼ʱΪ�ݹ����
/// \param srcdir ԭĿ¼
/// \param destdir Ŀ��Ŀ¼
/// \retval true �����ɹ�
/// \retval false ʧ��
bool copy_dir( const string &srcdir, const string &destdir ) {	
	vector<string> files = dir_files( srcdir );
	string from;
	string to;
	
	// ����Ŀ��Ŀ¼
	if ( !file_exist(destdir) )
		make_dir( destdir );
	
	for ( size_t i=0; i<files.size(); ++i ) {
		from = srcdir + "/" + files[i];
		to = destdir + "/" + files[i];
		
		// ��Ŀ¼,�ݹ����
		if ( files[i][0] == '/' ) {
			 if ( !copy_dir(from,to) )
				return false;
		}
		
		// �ļ�
		else if ( !copy_file(from,to) )
			return false;
	}
	
	return true;
}

/// \ingroup waFileSystem
/// \fn bool delete_dir( const string &dir )
/// ɾ��Ŀ¼,ɾ����Ŀ¼ʱΪ�ݹ����
/// \param dir Ҫɾ����Ŀ¼
/// \retval true �����ɹ�
/// \retval false ʧ��
bool delete_dir( const string &dir ) {	
	vector<string> files = dir_files( dir );
	string todel;
	
	// ɾ���ļ�
	for ( size_t i=0; i<files.size(); ++i ) {
		todel = dir + "/" + files[i];
		
		// ��Ŀ¼,�ݹ����
		if ( files[i][0] == '/' ) {
			 if ( !delete_dir(todel) )
				return false;
		}
		
		// �ļ�
		else if ( !delete_file(todel) )
			return false;
	}
	
	// ɾ��Ŀ¼
	if ( rmdir(dir.c_str()) == 0 )
		return true;

	return false;
}
			
/// \ingroup waFileSystem
/// \fn bool move_dir( const string &srcdir, const string &destdir )
/// �ƶ�Ŀ¼
/// \param srcdir ԭĿ¼
/// \param destdir Ŀ��Ŀ¼
/// \retval true �����ɹ�
/// \retval false ʧ��
bool move_dir( const string &srcdir, const string &destdir ) {
	if ( rename_file(srcdir,destdir) )
		return true;

	// rename fail, copy and delete dir	
	if ( copy_dir(srcdir,destdir) ) {
		if ( delete_dir(srcdir) )
			return true;
	}

	return false;
}

/// \ingroup waFileSystem
/// \fn void lock_file( int fd, const int type )
/// �ļ���������������ļ��ѱ������������ȴ�
/// \param fd �ļ����
/// \param type ��ģʽ����ѡF_WRLCK��F_RDLCK��F_UNLCK
void lock_file( int fd, const int type ) {
	struct flock lck;
	lck.l_start = 0;
	lck.l_len = 0;
	lck.l_whence = SEEK_SET;
	
	lck.l_type = type;
	if ( type==F_WRLCK || type==F_RDLCK )
		lck.l_type = type;
	else
		lck.l_type = F_UNLCK;
	
	while ( fcntl(fd,F_SETLKW,&lck)==-1 && errno==EINTR );
	return;
}

/// \ingroup waFileSystem
/// \fn bool is_locked( int fd )
/// �ж��ļ������
/// \param fd �ļ����
/// \retval true �ļ��ѱ���
/// \retval false �ļ�δ����
bool is_locked( int fd ) {
	struct flock lck;
	lck.l_start = 0;
	lck.l_len = 0;
	lck.l_whence = SEEK_SET;
	lck.l_type = F_WRLCK;

	while ( (fcntl(fd,F_GETLK,&lck))==-1 && errno==EINTR );

	if ( lck.l_type == F_UNLCK )
		return false;
	else
		return true;
}

/// \ingroup waFileSystem
/// \fn FILE* lock_open( const string &file, const char* mode, const int type )
/// �����������ļ������ļ��ѱ������������ȴ�
/// \param file �ļ�·��
/// \param mode �ļ���ģʽ����fopen()ͬ����������ͬ
/// \param type ��ģʽ����ѡF_WRLCK��F_RDLCK��F_UNLCK
/// \return �ļ������ʧ�ܷ���NULL
FILE* lock_open( const string &file, const char* mode, const int type ) {
	FILE *fp;
	mode_t mask = umask( 0 );
	int fd = open( file.c_str(), O_CREAT|O_EXCL|O_RDWR, 
				   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
	if ( fd >= 0 ) {
		// file not exist, create success
		lock_file( fd, type );
		fp = fdopen( fd, mode );
	} else {
		// file exist
		fp = fopen( file.c_str(), mode );
		if ( fp == NULL )
			return NULL;
		lock_file( fileno(fp), type );
	}
	umask( mask );

	return fp;
}

} // namespace

