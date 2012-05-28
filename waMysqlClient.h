/// \file waMysqlClient.h
/// webapp::ysqlClient,webapp::MysqlData��ͷ�ļ�
/// MySQL���ݿ�C++�ӿ�

// �������:
// (CC) -I /usr/local/include/mysql/ -L /usr/local/lib/mysql -lmysqlclient -lm

#ifndef _WEBAPPLIB_MYSQLCLIENT_H_
#define _WEBAPPLIB_MYSQLCLIENT_H_ 

#include <string>
#include <vector>
#include <map>
#include <mysql.h>

using namespace std;

/// Web Application Library namaspace
namespace webapp {

/// \defgroup waMysqlClient waMysqlClient�������������ȫ�ֺ���

/// \ingroup waMysqlClient
/// \typedef MysqlDataRow 
/// MysqlData ���������� (map<string,string>)
typedef map<string,string> MysqlDataRow;

/// SQL����ַ�ת��
string escape_sql( const string &str );

/// MySQL���ݼ���
class MysqlData {
	friend class MysqlClient;
	
	protected:
	
	/// ���MysqlData����
	bool fill_data( MYSQL *mysql );
	
	unsigned long _rows;
	unsigned int _cols;
	unsigned long _curpos;
	long _fetched;

	MYSQL_RES *_mysqlres;
	MYSQL_ROW _mysqlrow;
	MYSQL_FIELD *_mysqlfields;
	map<string,int> _field_pos;

	////////////////////////////////////////////////////////////////////////////
	public:

	/// MysqlData���캯��
	MysqlData():
	_rows(0), _cols(0), _curpos(0), _fetched(-1),
	_mysqlres(0), _mysqlfields(0)
	{};
	
	/// MysqlData��������
	virtual ~MysqlData();
	
	/// ����ָ��λ�õ�MysqlData����
	/// \param row ��λ��
	/// \param col ��λ��
	/// \return �����ַ���
	inline string operator() ( const unsigned long row, const unsigned int col ) {
		return this->get_data( row, col );
	}
	/// ����ָ��λ�õ�MysqlData����
	string get_data( const unsigned long row, const unsigned int col );

	/// ����ָ���ֶε�MysqlData����
	/// \param row ��λ��
	/// \param field �ֶ���
	/// \return �����ַ���
	inline string operator() ( const unsigned long row, const string &field ) {
		return this->get_data( row, field );
	}
	/// ����ָ���ֶε�MysqlData����
	string get_data( const unsigned long row, const string &field );

	/// ����ָ��λ�õ�MysqlData������
	MysqlDataRow get_row( const long row = -1 );

	/// ����MysqlData��������
	inline unsigned long rows() const {
		return _rows;
	}
	/// ����MysqlData��������
	inline unsigned int cols() const {
		return _cols;
	}
	
	/// �����ֶ�λ��
	int field_pos( const string &field );
	/// �����ֶ�����
	string field_name( const unsigned int col ) const;

	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// ��ֹ���ÿ������캯��
	MysqlData( MysqlData &copy );
	/// ��ֹ���ÿ�����ֵ����
	MysqlData& operator = ( const MysqlData& copy );
};

/// MySQL���ݿ�������
class MysqlClient {
	public:
	
	/// MysqlĬ�Ϲ��캯��
	MysqlClient():
	_connected(false)
	{};
	
	/// Mysql���캯��
	/// \param host MySQL����IP
	/// \param user MySQL�û���
	/// \param pwd �û�����
	/// \param database Ҫ�򿪵����ݿ�
	/// \param port ���ݿ�˿ڣ�Ĭ��Ϊ0
	/// \param socket UNIX_SOCKET��Ĭ��ΪNULL
	MysqlClient( const string &host, const string &user, const string &pwd, 
		const string &database, const int port = 0, const char* socket = NULL ) 
	{
		this->connect( host, user, pwd, database, port, socket );
	}
	
	/// Mysql��������
	virtual ~MysqlClient() {
		this->disconnect();
	}
	
	/// �������ݿ�
	bool connect( const string &host, const string &user, const string &pwd, 
		const string &database, const int port = 0, const char* socket = NULL );
	/// �Ͽ����ݿ�����
	void disconnect();
	/// �ж��Ƿ��������ݿ�
	bool is_connected();
	
	/// ѡ�����ݿ�
	bool select_db( const string &database );

	/// ִ��SQL���,ȡ�ò�ѯ���
	bool query( const string &sqlstr, MysqlData &records );
	/// ִ��SQL���
	bool query( const string &sqlstr );
	/// ���ز�ѯ�����ָ��λ�õ��ַ���ֵ
	string query_val( const string &sqlstr, 
		const unsigned long row = 0, const unsigned int col = 0 );
    /// ���ز�ѯ�����ָ����
    MysqlDataRow query_row( const string &sqlstr, const unsigned long row = 0 );

	/// �ϴβ�ѯ������Ӱ��ļ�¼����
	unsigned long affected();
	/// ȡ���ϴβ�ѯ��һ��AUTO_INCREMENT�����ɵ�ID
	unsigned long last_id();
	
	/// ȡ��Mysql������Ϣ
	/// \return ���ش�����Ϣ�ַ���
	inline string error() {
		return string( mysql_error(&_mysql) );
	}
	/// ȡ��Mysql������
	/// \return ���ش�����Ϣ���
	inline unsigned int errnum() {
		return mysql_errno( &_mysql );
	}

	/// ȡ�ø�����Ϣ
	string info();

	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// ��ֹ���ÿ������캯��
	MysqlClient( MysqlClient &copy );
	/// ��ֹ���ÿ�����ֵ����
	MysqlClient& operator = ( const MysqlClient& copy );

	MYSQL _mysql;
	bool _connected;
};

} // namespace

#endif //_WEBAPPLIB_MYSQLCLIENT_H_

