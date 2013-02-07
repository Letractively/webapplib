/// \file waMysqlClient.cpp
/// webapp::MysqlClient,webapp::MysqlData��ʵ���ļ�

// �������:
// (CC) -I /usr/local/include/mysql/ -L /usr/local/lib/mysql -lmysqlclient -lm

#include <cstring>
#include "waMysqlClient.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \ingroup waMysqlClient
/// \fn string escape_sql( const string &str )
/// SQL����ַ�ת��
/// \param Ҫת����SQL�ַ���
/// \return ת������ַ���
string escape_sql( const string &str ) {
	char *p = new char[str.length()*2+1];
	mysql_escape_string( p, str.c_str(), str.length() );
	string s = p;
	delete[] p;
	return s;
}

////////////////////////////////////////////////////////////////////////////
// MysqlData

/// MysqlData��������
MysqlData::~MysqlData() {
	if ( _mysqlres != NULL )
		mysql_free_result( _mysqlres );
	_mysqlres = 0;
	_mysqlfields = 0;
}

/// ����ָ��λ�õ�MysqlData����
/// \param row ������λ��,Ĭ��Ϊ0
/// \param col ������λ��,Ĭ��Ϊ0
/// \return ��������,�������򷵻ؿ��ַ���
string MysqlData::get_data( const size_t row, const size_t col ) {
	if( _mysqlres!=NULL && row<_rows && col<_cols ) {
		if ( row != _fetched ) {
			if ( row != _curpos+1 ) {
				mysql_data_seek( _mysqlres, row );
			}
			_mysqlrow = mysql_fetch_row( _mysqlres );
			_fetched = row; 
		}
		
		if ( _mysqlrow!=NULL && _mysqlrow[col]!=NULL ) {
			_curpos = row; // log current cursor
			return  string( _mysqlrow[col] );
		}
	}
	return string( "" );
}

/// ����ָ���ֶε�MysqlData����
/// \param row ��λ��
/// \param field �ֶ���
/// \return �����ַ���,�����ڷ��ؿ��ַ���
string MysqlData::get_data( const size_t row, const string &field ) {
	int col = this->field_pos( field );
	if ( col != -1 )
		return this->get_data( row, col );
	else
		return string( "" );
}

/// ����ָ��λ�õ�MysqlData������
/// \param row ������λ��,Ĭ��Ϊ0����һ��
/// \return ����ֵ����ΪMysqlDataRow,��map<string,string>
MysqlDataRow MysqlData::get_row( const size_t row ) {
	MysqlDataRow datarow;
	string field;
		
	if( _mysqlres!=NULL && row<_rows ) {
		if ( row != _curpos ) {
			if ( row != _curpos+1 ) {
				mysql_data_seek( _mysqlres, row );
			}
			_mysqlrow = mysql_fetch_row( _mysqlres );
		}
		
		if ( _mysqlrow != NULL ) {
			_curpos = row; // log current cursor
			for ( size_t i=0; i<_cols; ++i ) {
				field = this->field_name( i );
				if ( field!="" && _mysqlrow[i]!=NULL ) {
					datarow.insert( MysqlDataRow::value_type(field,_mysqlrow[i]) );
				}
			}
		}
	}
	
	return datarow;
}

/// ���MysqlData����
/// \param mysql MYSQL*����
/// \retval true �ɹ�
/// \retval false ʧ��
bool MysqlData::fill_data( MYSQL *mysql ) {
	if ( mysql == NULL )
		return false;
	
	// clean		
	if ( _mysqlres != NULL )
		mysql_free_result( _mysqlres );
	_mysqlres = 0;
	_curpos = 0; // return to first position
	_field_pos.clear(); // clean field pos cache

	// fill data
	_mysqlres = mysql_store_result( mysql );
	if ( _mysqlres != NULL ) {
		_rows = mysql_num_rows( _mysqlres );
		_cols = mysql_num_fields( _mysqlres );
		_mysqlfields = mysql_fetch_fields( _mysqlres );
		
		// init first data
		mysql_data_seek( _mysqlres, 0 );
		_mysqlrow = mysql_fetch_row( _mysqlres );
		_fetched = 0;		
		
		return true;
	}
	return false;
}

/// �����ֶ�λ��
/// \param field �ֶ���
/// \return �����ݽ���д��ڸ��ֶ��򷵻��ֶ�λ��,���򷵻�-1
int MysqlData::field_pos( const string &field ) {
	if ( _mysqlfields==0 || field=="" )
		return -1;
	
	// check cache
	if ( _field_pos.find(field) != _field_pos.end() )
		return _field_pos[field];

	for( size_t i=0; i<_cols; ++i ) {
		if ( strcmp(field.c_str(),_mysqlfields[i].name) == 0 ) {
			_field_pos[field] = i;
			return i;
		}
	}
	_field_pos[field] = -1;
	return -1;
}

/// �����ֶ�����
/// \param col �ֶ�λ��
/// \return �����ݽ���д��ڸ��ֶ��򷵻��ֶ�����,���򷵻ؿ��ַ���
string MysqlData::field_name( size_t col ) const {
	if ( _mysqlfields!=0 && col<=_cols )
		return string( _mysqlfields[col].name );
	else
		return string( "" );
}

////////////////////////////////////////////////////////////////////////////
// MysqlClient

/// �������ݿ�
/// \param host MySQL����IP
/// \param user MySQL�û���
/// \param pwd �û�����
/// \param database Ҫ�򿪵����ݿ�
/// \param port ���ݿ�˿ڣ�Ĭ��Ϊ0
/// \param socket UNIX_SOCKET��Ĭ��ΪNULL
/// \retval true �ɹ�
/// \retval false ʧ��
bool MysqlClient::connect( const string &host, const string &user, const string &pwd, 
	const string &database, const int port, const char* socket ) 
{
	this->disconnect();
	
	if ( mysql_init(&_mysql) ) {
		if ( mysql_real_connect( &_mysql, host.c_str(), user.c_str(),
			pwd.c_str(), database.c_str(), port, socket, CLIENT_COMPRESS ) )
			_connected = true;
	}
	
	return _connected;
}

/// �Ͽ����ݿ�����
void MysqlClient::disconnect() {
	if( _connected ) {
		mysql_close( &_mysql );
		_connected = false;
	}
}

/// �ж��Ƿ��������ݿ�
/// \retval true ����
/// \retval false �Ͽ�
bool MysqlClient::is_connected() {
	if ( _connected ) {
		if ( mysql_ping(&_mysql) == 0 )
			_connected = true;
		else
			_connected = false;
	}
	
	return _connected;
}

/// ѡ�����ݿ�
/// \param database ���ݿ���
/// \retval true �ɹ�
/// \retval false ʧ��
bool MysqlClient::select_db( const string &database ) {
	if ( _connected && mysql_select_db(&_mysql,database.c_str())==0 )
		return true;
	else
		return false;
}

/// ִ��SQL���,ȡ�ò�ѯ���
/// \param sqlstr Ҫִ�е�SQL���
/// \param records �������ݽ����MysqlData����
/// \retval true �ɹ�
/// \retval false ʧ��
bool MysqlClient::query( const string &sqlstr, MysqlData &records ) {
	if ( _connected && mysql_real_query(&_mysql,sqlstr.c_str(),sqlstr.length())==0 ) {
		if( records.fill_data(&_mysql) )
			return true;
	}
	return false;
}

/// ִ��SQL���
/// \param sqlstr Ҫִ�е�SQL���
/// \retval true �ɹ�
/// \retval false ʧ��
bool MysqlClient::query( const string &sqlstr ) {
	if ( _connected && mysql_real_query(&_mysql,sqlstr.c_str(),sqlstr.length())==0 )
		return true;			
	else
		return false;
}

/// ���ز�ѯ�����ָ��λ�õ��ַ���ֵ
/// \param sqlstr SQL��ѯ�ַ���
/// \param row ������λ��,Ĭ��Ϊ0
/// \param col ������λ��,Ĭ��Ϊ0
/// \return ��ѯ�ɹ������ַ���,���򷵻ؿ��ַ���
string MysqlClient::query_val( const string &sqlstr, const size_t row, 
	const size_t col ) 
{
	MysqlData data;
	if ( this->query(sqlstr,data) ) {
		if ( data.rows()>row && data.cols()>col )
			return data(row,col);
	}
	return string( "" );
}

/// ���ز�ѯ�����ָ����
/// \param sqlstr SQL��ѯ�ַ���
/// \param row ������λ��,Ĭ��Ϊ0
/// \return ����ֵ����ΪMysqlDataRow,��map<string,string>
MysqlDataRow MysqlClient::query_row( const string &sqlstr, const size_t row ) {
    MysqlData data;
    MysqlDataRow datarow;
    if ( this->query(sqlstr,data) ) {
        if ( row < data.rows() )
            datarow = data.get_row( row );
    }

    return datarow;
}

/// �ϴβ�ѯ������Ӱ��ļ�¼����
/// \return ���ؼ�¼����,����size_t
size_t MysqlClient::affected() {
	if ( _connected )
		return mysql_affected_rows( &_mysql );
	else
		return 0;
}

/// ȡ���ϴβ�ѯ��һ��AUTO_INCREMENT�����ɵ�ID
/// һ��Mysql��ֻ����һ��AUTO_INCREMENT��,�ұ���Ϊ����
/// \return �������ɵ�ID
size_t MysqlClient::last_id() {
	if ( _connected )
		return mysql_insert_id( &_mysql );
	else
		return 0;
}

/// ȡ�ø�����Ϣ
/// \return ���ظ�����Ϣ
string MysqlClient::info() {
	if ( _connected )
		return string( mysql_info(&_mysql) );
	else
		return string( "" );
}

} // namespace

