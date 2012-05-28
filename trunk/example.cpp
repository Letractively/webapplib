/// \file example.cpp
/// ����ʾ���ļ�����ʾһ����CGI����

#include <iostream>
#include "webapplib.h"

using namespace webapp;

int main() {
	/***************************************************************************
	��ʾ������CGIӦ�ó�������,ģ��WEB��ѯ����	
	1����ȡCGI������Cookie���ݣ�String��Cgi��Cookie��
	2��ʹ�ö�ȡ���Ĳ�������Ȩ�޼��ӿڣ�ConfigFile��HttpClient��
	3��ʹ�ö�ȡ���Ĳ�����ѯ���ݿ⣨MysqlClient��
	4��ʹ����־�ļ���¼�û�����FileSystem��Utility��Encode��
	5�������û���Cookie��Cookie��DateTime��
	6����ʾHTMLҳ�棨Template��
	***************************************************************************/	

	////////////////////////////////////////////////////////////////////////////
	// 1����ȡCGI������Cookie���ݣ�Cgi��Cookie��
	
	Cgi cgi;
	Cookie cookie;
	String username = cgi["username"];
	String usercookie = cookie["usercookie"];
	
	/*��ʾ webapp::Cgi�ڶ�����CGI��������ʱ�������ڵ���ģʽ����ʾ����CGI����ֵ*/
	
	////////////////////////////////////////////////////////////////////////////
	// 2��ʹ�ö�ȡ���Ĳ�������Ȩ�޼��ӿڣ�ConfigFile��HttpClient��
	
	ConfigFile conf( "example.conf" );
	String check_interface = conf["check_interface"];
	
	cout << "---------------------------------------------------------" << endl;
	cout << "check user privilege from:" << check_interface << endl;
	
	HttpClient www;
	www.request( check_interface + "?username=" + username );
	if ( www.done() && www.content()=="CHECK_PASS" ) {
		cout << "check pass" << endl;
	} else {
		cout << "check fail" << endl;
	}
	
	////////////////////////////////////////////////////////////////////////////
	// 3��ʹ�ö�ȡ���Ĳ�����ѯ���ݿ⣨MysqlClient��
	String value;
	#ifndef _WEBAPPLIB_NOMYSQL

	String sql;
	sql.sprintf( "SELECT value FROM table WHERE user='%s'", escape_sql(username).c_str() );
	
	MysqlClient mysqlclient;
	MysqlData mysqldata;

	mysqlclient.connect( "example.mysql.com", "user", "pwd", "database" );
	if ( mysqlclient.is_connected() ) {
		if ( mysqlclient.query(sql,mysqldata) ) {
			value = mysqldata( 0, "value" );
		} else {
			cout << mysqlclient.error() << endl;
		}
	}

	#endif //_WEBAPPLIB_NOMYSQL

	////////////////////////////////////////////////////////////////////////////
	// 4��ʹ����־�ļ���¼�û�����FileSystem��Utility��Encode��
	
	String log_path = "/tmp/";
	String log_file = log_path + "/logfile.txt";
	if ( !file_exist(log_path) || !is_dir(log_path) ) {
		make_dir( log_path );
	}

	file_logger( log_file, "username:%s", username.c_str() );
	file_logger( log_file, "usermd5:%s", md5_encode(username).c_str() );
	
	cout << "---------------------------------------------------------" << endl;
	String file_content;
	file_content.load_file( log_file );
	cout << file_content << endl;

	////////////////////////////////////////////////////////////////////////////
	// 5�������û���Cookie��Cookie��DateTime��
	
	DateTime now;
	DateTime expires = now + ( TIME_ONE_DAY*3 ); // Cookie��Ч��Ϊ����
	cookie.set_cookie( "username", username, expires.gmt_datetime() );
	
	////////////////////////////////////////////////////////////////////////////
	// 6����ʾHTMLҳ�棨Template��
	
	Template page("example.tmpl");
	page.set( "username", username );
	page.set( "value", value );
	
	// ��ʾ��ѯ���
	cout << "---------------------------------------------------------" << endl;
	http_head();
	page.print();
}

