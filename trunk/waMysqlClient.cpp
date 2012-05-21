/// \file waMysqlClient.cpp
/// MysqlClient,MysqlData��ʵ���ļ�

// �������:
// (CC) -I /usr/local/include/mysql/ -L /usr/local/lib/mysql -lmysqlclient -lm

#include "waMysqlClient.h"

using namespace std;

// WEB Application Library namaspace
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
string MysqlData::get_data( const unsigned long row, const unsigned int col ) {
	if( _mysqlres!=NULL && row<_rows && col<_cols ) {
		if ( static_cast<long>(row) != _fetched ) {
			if ( row != _curpos+1 )
				mysql_data_seek( _mysqlres, row );
			_mysqlrow = mysql_fetch_row( _mysqlres );
			_fetched = static_cast<long>( row ); 
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
string MysqlData::get_data( const unsigned long row, const string &field ) {
	int col = this->field_pos( field );
	if ( col != -1 )
		return this->get_data( row, col );
	else
		return string( "" );
}

/// ����ָ��λ�õ�MysqlData������
/// \param row ������λ��,Ĭ��Ϊ��ǰ��¼λ��,
/// ��ǰ��¼λ����first(),prior(),next(),last(),find()��������,Ĭ��Ϊ0
/// \return ����ֵ����ΪMysqlDataRow,��map<string,string>
MysqlDataRow MysqlData::get_row( const long row ) {
	MysqlDataRow datarow;
	string field;
	unsigned long rowpos;
	
	if ( row < 0 ) 
		rowpos = _curpos;
	else
		rowpos = row;
		
	if( _mysqlres!=NULL && rowpos<_rows ) {
		if ( rowpos != _curpos ) {
			if ( rowpos != _curpos+1 )
				mysql_data_seek( _mysqlres, rowpos );
			_mysqlrow = mysql_fetch_row( _mysqlres );
		}
		
		if ( _mysqlrow != NULL ) {
			_curpos = rowpos; // log current cursor
			for ( size_t i=0; i<_cols; ++i ) {
				field = this->field_name( i );
				if ( field!="" && _mysqlrow[i]!=NULL )
					datarow.insert( MysqlDataRow::value_type(field,_mysqlrow[i]) );
			}
		}
	}
	
	return datarow;
}

/// ���õ�ǰλ��Ϊǰһ������
/// \retval true �ɹ� 
/// \retval false �Ѿ�Ϊ��һ������
bool MysqlData::prior() {
	if ( _curpos > 0 ) {
		--_curpos;
		return true;
	}
	return false;
}

/// ���õ�ǰλ��Ϊ��һ������
/// \retval true �ɹ� 
/// \retval false �Ѿ�Ϊ���һ������
bool MysqlData::next() {
	if ( _curpos < _rows-1 ) {
		++_curpos;
		return true;
	}
	return false;
}

/// ��ѯ����λ�� 
/// \param field �����ֶ���
/// \param value Ҫ���ҵ�����ֵ
/// \param mode ���ҷ�ʽ
/// - MysqlData::FIND_FIRST ��ͷ��ʼ����,
/// - MysqlData::FIND_NEXT �ӵ�ǰλ�ÿ�ʼ��������
/// - Ĭ��ΪMysqlData::FIND_FIRST
/// \return �����ҵ���Ӧ��¼���ؼ�¼����λ�ò���Ϊ��ǰ����λ�ã����򷵻�-1
long MysqlData::find( const string &field, const string &value, 
					 const find_mode mode ) {
	// confirm
	if ( _rows==0 || _cols==0 )
		return -1;
	
	int col = this->field_pos( field );
	if ( col==-1 || static_cast<size_t>(col)>_cols ) 
		return -1;
		
	// init
	unsigned long pos;
	if ( mode == FIND_FIRST )
		pos = 0;
	else if ( _curpos < _rows )
		pos = _curpos + 1;
	else
		return -1;	
		
	// find
	for ( ; pos<_rows; ++pos ) {
		if ( this->get_data(pos,col) == value ) {
			_curpos = pos;
			return _curpos;
		}
	}
	return -1;
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
	this->first(); // return to first position
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
string MysqlData::field_name( unsigned int col ) const {
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
					 const string &database, const int port, const char* socket ) {
	this->disconnect();
	
	if ( mysql_init(&_mysql) ) {
		if ( mysql_real_connect( &_mysql, host.c_str(), user.c_str(),
								 pwd.c_str(), database.c_str(),
								 port, socket, CLIENT_COMPRESS ) )
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
string MysqlClient::query_val( const string &sqlstr, const unsigned long row, 
						 const unsigned int col ) {
	MysqlData res;
	if ( this->query(sqlstr,res) ) {
		if ( res.rows()>row && res.cols()>col )
			return res(row,col);
	}
	return string( "" );
}

/// ���ز�ѯ�����ָ����
/// \param sqlstr SQL��ѯ�ַ���
/// \param row ������λ��,Ĭ��Ϊ0
/// \return ����ֵ����ΪMysqlDataRow,��map<string,string>
MysqlDataRow MysqlClient::query_row( const string &sqlstr, const unsigned long row ) {
	MysqlData res;
	MysqlDataRow resrow;
	if ( this->query(sqlstr,res) ) {
		if ( row < res.rows() )
			resrow = res.get_row( row );
	}
	
	return resrow;
}

/// �ϴβ�ѯ������Ӱ��ļ�¼����
/// \return ���ؼ�¼����,����unsigned long
unsigned long MysqlClient::affected() {
	if ( _connected )
		return mysql_affected_rows( &_mysql );
	else
		return 0;
}

/// ȡ���ϴβ�ѯ��һ��AUTO_INCREMENT�����ɵ�ID
/// һ��Mysql��ֻ����һ��AUTO_INCREMENT��,�ұ���Ϊ����
/// \return �������ɵ�ID
unsigned long MysqlClient::last_id() {
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

