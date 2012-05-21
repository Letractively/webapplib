/// \file example.cpp
/// ʾ���ļ�
/// ��ʾwaString,MysqlClient,MysqlData,Encode,Cgi,DateTime,Template,HttpClient��ʹ��

#include <iostream>
#include "webapplib.h"

using namespace webapp;

int main() {
	////////////////////////////////////////////////////////////////////////////
	// ��ʾ������CGIӦ�ó�������,ģ��WEB��ѯ����
	
	// ����cookie�ж��û����;
	// ����Զ��CGI��֤�û��Ƿ���Ч;
	// ȡ��CGI����data_name;
	// ƴ��ΪSQL���;
	// �������ݿⲢִ�в�ѯ;
	// ��¼���β�ѯ����;
	// ���û������д��cookie;
	// ��ʾ��ѯ���.

	////////////////////////////////////////////////////////////////////////////
	// ��ȡcookie "username"
	
	Cookie cookie; // Cookie��ȡ����
	String username = cookie["username"];;
	if ( username == "" ) {
		// ������cookieֵ,�ٶ�Ϊ�ն˵���ģʽ,�ֶ��������ִ��
		cout << "*** Terminal Debug Mode ***" << endl;
		cout << "Input value of cookie 'username': " << flush;
		cin >> username;
	}
	
	////////////////////////////////////////////////////////////////////////////
	// ͨ������Զ��CGI�ӿ���֤�û����Ƿ���Ч
	
	String checkuser = "http://members.sina.com.cn/cgi-bin/check.cgi?username=" + username;
	HttpClient www; // HTTP�������
	www.request( checkuser );
	if ( !www.done() ) {
		// ִ��HTTP����ʧ��,�ٶ�Ϊ�ն˵���ģʽ,����ִ��
		cout << "HTTP request failure, continue..." << endl;
	}
	
	if ( www.content() != "CHECK OK!" ) {
		// ���ؽ������,�ٶ�Ϊ�ն˵���ģʽ,����ִ��
		cout << "Check username error, continue..." << endl;
	}
	
	////////////////////////////////////////////////////////////////////////////
	// ȡ��CGI����
	
	Cgi cgi;  // CGI��ȡ����
	String data_name = cgi["data_name"];
	
	// �����������ƴ��SQL���
	String sql;
	sql.sprintf( "SELECT data_value FROM data_table WHERE data_name='%s'", 
				 escape_sql(data_name).c_str() );
	
	////////////////////////////////////////////////////////////////////////////
	// �������ݿ�
	
	MysqlClient mysql;  // Mysql���ݿ����Ӷ���
	MysqlData mysqlres; // MysqlRes���ݿ���������
	String datavalue;

	// �������ݿ�
	mysql.connect( "127.0.0.1", "user", "pwd", "database" );
	if ( !mysql.is_connected() ) {
		// �޷��������ݿ�,�ٶ�Ϊ�ն˵���ģʽ,����ִ��
		cout << "Connect DB failure, continue..." << endl;
	}

	// ִ�в�ѯ
	if ( mysql.query(sql,mysqlres) ) {
		// ȡ�ò�ѯ����ĵ�һ�м�¼�е�"data_value"�ֶ�ֵ
		datavalue = mysqlres( 0, "data_value" );
	} else {
		// ��ѯ���ݿ����,�ٶ�Ϊ�ն˵���ģʽ,����ִ��
		cout << "Query DB error, input value of 'data_value':" << flush;
		cin >> datavalue;
	}

	////////////////////////////////////////////////////////////////////////////
	// ��¼���β�ѯ����
	
	DateTime now; // DateTime����,Ĭ�ϳ�ʼ��Ϊ��ǰʱ��

	String log;
	// ��ǰ����ʱ��,�û���,SQL���,���ݿ������Ϣ
	log = now.datetime() + "\t" + username + "\t" + sql + "\t" + mysql.error() + "\n";
	// ׷��д�뵽��־�ļ�
	log.save_file( "example.log", ios::app, 0666 );

	////////////////////////////////////////////////////////////////////////////
	// ���û������д���û���cookie
	
	// ��Ч��Ϊ����
	DateTime expires = now + ( TIME_ONE_DAY*3 );
	// ���ڰ�ȫ�Կ��ǣ��û����������cookie����Ӧ�����м���
	cookie.set_cookie( "username", username, expires.gmt_datetime() );
	
	////////////////////////////////////////////////////////////////////////////
	// ������ҳ��
	
	// ��ȡHTML���ҳ��ģ�岢�滻�������
	Template page;
	if ( !page.load("template.htt") ) {
		// �޷���ȡģ���ļ�,�ٶ�Ϊ�ն˵���ģʽ,����ִ��
		String input_words;
		cout << "Load template file failure, input content of 'welcome word':" << endl;
		cin >> input_words;
		String template_text = input_words + " ([$username])\n" +
							   "Value of '" + data_name + "' is: ([$datavalue])\n";
		page.tmpl( template_text );
	}
	
	page.set( "username", username );
	page.set( "datavalue", datavalue );
	
	// ��ʾ��ѯ���
	http_head();
	page.print();
}

