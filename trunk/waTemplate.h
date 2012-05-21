/// \file waTemplate.h
/// HTMLģ�崦����ͷ�ļ�
/// ֧��������ѭ���ű���HTMLģ�崦����
/// ������ waString
/// <a href="wa_template.html">ʹ��˵���ĵ����򵥷���</a>

#ifndef _WEBAPPLIB_TEMPLATE_H_
#define _WEBAPPLIB_TEMPLATE_H_ 

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "waString.h"

using namespace std;

// WEB Application Library namaspace
namespace webapp {
	
/// ֧��������ѭ���ű���HTMLģ�崦����
/// <a href="wa_template.html">ʹ��˵���ĵ����򵥷���</a>
class Template {
	public:
	
	/// Ĭ�Ϲ��캯��
	Template(){};
	
	/// ���캯��
	/// \param tmpl_file ģ���ļ�
	Template( const string tmpl_file ) {
		this->load( tmpl_file );
	}
	
	/// ���캯��
	/// \param tmpl_dir ģ��Ŀ¼
	/// \param tmpl_file ģ���ļ�
	Template( const string tmpl_dir, const string tmpl_file ) {
		this->load( tmpl_dir, tmpl_file );
	}
	
	/// ��������
	virtual ~Template(){};
	
	/// \enum ����Htt::sort_table()����ʽ
	enum sort_mode {
		/// ����
		TEMPLATE_SORT_ASCE,	
		/// ����
		TEMPLATE_SORT_DESC	
	};

	/// \enum ����Htt::sort_table()����ȽϷ�ʽ
	enum cmp_mode {
		/// �ַ����Ƚ�
		TEMPLATE_CMP_STR,	
		/// �����Ƚ�
		TEMPLATE_CMP_INT	
	};

	/// \enum ���ʱ�Ƿ����������Ϣ
	enum output_mode {
		/// ��ʾ������Ϣ
		TEMPLATE_OUTPUT_DEBUG,	
		/// ����ʾ
		TEMPLATE_OUTPUT_RELEASE	
	};

	/// ��ȡHTMLģ���ļ�
	bool load( const string &tmpl_file );

	/// ��ȡģ��
	/// \param tmpl_dir ģ��Ŀ¼
	/// \param tmpl_file ģ���ļ�
	/// \retval true ��ȡ�ɹ�
	/// \retval false ʧ��
	inline bool load( const string &tmpl_dir, const string &tmpl_file ) {
		return this->load( tmpl_dir + "/" + tmpl_file );
	}
	
	/// ����HTMLģ������
	void tmpl( const string &tmpl );

	/// �����滻����
	void set( const string &name, const string &value );
	/// �����滻����
	/// \param name ģ��������
	/// \param value �滻ֵ
	inline void set( const string &name, const long value ) {
		this->set( name, itos(value) );
	}
	/// ȡ���滻����
	void unset( const string &name );
	
	/// �½����
	void table( const string &table, const char* field_0, ... );
	/// ȡ�����
	void unset_table( const string &table );
	/// ���һ�����ݵ����
	void set_row( const string &table, const char* value_0, ... );
	/// ���һ��ָ����ʽ�����ݵ����
	void format_row( const string &table, const char* format, ... );
	/// ȡ�����ָ����
	void unset_row( const string &table, const int row );
	/// ���ñ��ָ��λ��ֵ
	void set( const string &table, const int row, 
			  const string &field, const string &value );
	/// ���������
	void sort_table( const string &table, const string &field, 
					 const sort_mode mode = TEMPLATE_SORT_ASCE,
					 const cmp_mode cmp = TEMPLATE_CMP_STR );
	
	/// ��������滻����
	void clear_set();

	/// ����HTML�ַ���
	string html();
	/// ���HTML��stdout
	void print( const output_mode mode = TEMPLATE_OUTPUT_RELEASE );
	/// ���HTML���ļ�
	bool print( const string &file, const output_mode mode = TEMPLATE_OUTPUT_RELEASE,
				const mode_t permission = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
	
	////////////////////////////////////////////////////////////////////////////
	private:
	
	/// ��ȡָ��λ�õ�ģ��ű����ͼ����ʽ
	int parse_script( const string &tmpl, const unsigned int pos,
					  string &exp, int &type );

	/// �������ʽ��ֵ
	string exp_value( const string &expression );

	/// ��������ģ��
	void parse( const string &tmpl, ostream &output );
	
	/// ������������ʽ�Ƿ����
	bool compare( const string &exp );
	
	/// ��������Ƿ����
	bool check_if( const string &exp );

	/// ������������ģ��
	size_t parse_if( const string &tmpl, ostream &output, 
					 const bool parent_state, const string &parsed_exp,
					 const int parsed_length );

	/// �����ֶ�λ��
	int field_pos( const string &table, const string &field );

	/// ���ѭ������Ƿ���Ч
	bool check_for( const string &tablename );
	
	/// ���ر����ָ��λ���ֶε�ֵ
	string table_value( const string &field );

	/// ����ѭ������ģ��
	size_t parse_for( const string &tmpl, ostream &output, 
					  const bool parent_state, const string &parsed_exp,
					  const int parsed_length );
							
	/// ģ����������¼
	void error_log( const size_t lines, const string &error );
	/// ģ�������¼
	void parse_log( ostream &output );

	// ���ݶ���
	typedef vector<string> strings;		// �ַ����б�
	typedef struct {					// ���ģ�����ýṹ
		int cols;						// ����ֶ�����
		int rows;						// �����������
		int cursor;						// ��ǰ���λ��
		strings fields;					// ����ֶζ����б�
		map<string,int> fieldspos;		// ����ֶ�λ��,for speed
		vector<strings> datas;			// �������
	} template_table;

	// ģ������
	String _tmpl;						// HTMLģ������
	map<string,string> _sets;			// �滻�����б� <ģ��������,ģ����ֵ>
	map<string,template_table> _tables;	// ����滻�����б� <�������,���ģ�����ýṹ>
	
	// ������������
	string _table;						// ��ǰ�������
	int _cursor;						// ��ǰ�����λ��
	int _lines;							// �Ѵ���ģ������

	string _tmplfile;					// HTMLģ���ļ���
	char _date[15];						// ��ǰ����
	char _time[15];						// ��ǰʱ��
	output_mode _debug;					// ����ģʽ
	multimap<int,string> _errlog;		// ���������¼ <����λ������,����������Ϣ>
};

// ģ���﷨��ʽ����
const char TEMPLATE_BEGIN[]		= "{{";		const int TEMPLATE_BEGIN_LEN 	= strlen(TEMPLATE_BEGIN);
const char TEMPLATE_END[]		= "}}";		const int TEMPLATE_END_LEN 		= strlen(TEMPLATE_END);
const char TEMPLATE_SUBBEGIN[]	= "(";		const int TEMPLATE_SUBBEGIN_LEN = strlen(TEMPLATE_SUBBEGIN);
const char TEMPLATE_SUBEND[]	= ")";		const int TEMPLATE_SUBEND_LEN 	= strlen(TEMPLATE_SUBEND);
const char TEMPLATE_SPLIT[]		= ",";		const int TEMPLATE_SPLIT_LEN 	= strlen(TEMPLATE_SPLIT);
const char TEMPLATE_NEWLINE[]	= "\n";		const int TEMPLATE_NEWLINE_LEN 	= strlen(TEMPLATE_NEWLINE);

const char TEMPLATE_VALUE[]		= "$";		const int TEMPLATE_VALUE_LEN 	= strlen(TEMPLATE_VALUE);
const char TEMPLATE_DATE[]		= "%DATE";	const int TEMPLATE_DATE_LEN 	= strlen(TEMPLATE_DATE);
const char TEMPLATE_TIME[]		= "%TIME";	const int TEMPLATE_TIME_LEN 	= strlen(TEMPLATE_TIME);
const char TEMPLATE_SPACE[]		= "%SPACE";	const int TEMPLATE_SPACE_LEN	= strlen(TEMPLATE_SPACE);
const char TEMPLATE_BLANK[]		= "%BLANK";	const int TEMPLATE_BLANK_LEN 	= strlen(TEMPLATE_BLANK);
                                        
const char TEMPLATE_FOR[]		= "#FOR";	const int TEMPLATE_FOR_LEN 		= strlen(TEMPLATE_FOR);
const char TEMPLATE_ENDFOR[]	= "#ENDFOR";const int TEMPLATE_ENDFOR_LEN 	= strlen(TEMPLATE_ENDFOR);
const char TEMPLATE_TBLVALUE[]	= ".$";		const int TEMPLATE_TBLVALUE_LEN = strlen(TEMPLATE_TBLVALUE);
const char TEMPLATE_TBLSCOPE[]	= "@";		const int TEMPLATE_TBLSCOPE_LEN = strlen(TEMPLATE_TBLSCOPE);
const char TEMPLATE_CURSOR[]	= "%CURSOR";const int TEMPLATE_CURSOR_LEN	= strlen(TEMPLATE_CURSOR);
const char TEMPLATE_ROWS[]		= "%ROWS";	const int TEMPLATE_ROWS_LEN		= strlen(TEMPLATE_ROWS);

const char TEMPLATE_IF[]		= "#IF";	const int TEMPLATE_IF_LEN 		= strlen(TEMPLATE_IF);
const char TEMPLATE_ELSIF[]		= "#ELSIF";	const int TEMPLATE_ELSIF_LEN 	= strlen(TEMPLATE_ELSIF);
const char TEMPLATE_ELSE[]		= "#ELSE";	const int TEMPLATE_ELSE_LEN 	= strlen(TEMPLATE_ELSE);
const char TEMPLATE_ENDIF[]		= "#ENDIF";	const int TEMPLATE_ENDIF_LEN 	= strlen(TEMPLATE_ENDIF);

// �Ƚϲ���������
const char TEMPLATE_AND[]		= "AND";	const int TEMPLATE_AND_LEN 		= strlen(TEMPLATE_AND);
const char TEMPLATE_OR[]		= "OR";		const int TEMPLATE_OR_LEN 		= strlen(TEMPLATE_OR);
const char TEMPLATE_EQ[]		= "==";		const int TEMPLATE_EQ_LEN 		= strlen(TEMPLATE_EQ);
const char TEMPLATE_NE[]		= "!=";		const int TEMPLATE_NE_LEN 		= strlen(TEMPLATE_NE);
const char TEMPLATE_LE[]		= "<=";		const int TEMPLATE_LE_LEN 		= strlen(TEMPLATE_LE);
const char TEMPLATE_LT[]		= "<";		const int TEMPLATE_LT_LEN 		= strlen(TEMPLATE_LT);
const char TEMPLATE_GE[]		= ">=";		const int TEMPLATE_GE_LEN 		= strlen(TEMPLATE_GE);
const char TEMPLATE_GT[]		= ">";		const int TEMPLATE_GT_LEN 		= strlen(TEMPLATE_GT);

// Htt::format_row()��ʽ����
const char TEMPLATE_FMTSTR[]	= "%s";		const int TEMPLATE_FMTSTR_LEN 	= strlen(TEMPLATE_FMTSTR);
const char TEMPLATE_FMTINT[]	= "%d";		const int TEMPLATE_FMTINT_LEN 	= strlen(TEMPLATE_FMTINT);

// �ű��������
enum template_scripttype {
	TEMPLATE_S_VALUE,	
	TEMPLATE_S_TBLVALUE,	
	TEMPLATE_S_FOR,	
	TEMPLATE_S_ENDFOR,	
	TEMPLATE_S_IF,		
	TEMPLATE_S_ELSIF,	
	TEMPLATE_S_ELSE,		
	TEMPLATE_S_ENDIF,
	TEMPLATE_S_CURSOR,
	TEMPLATE_S_ROWS,
	TEMPLATE_S_DATE,
	TEMPLATE_S_TIME,
	TEMPLATE_S_SPACE,
	TEMPLATE_S_BLANK,
	TEMPLATE_S_UNKNOWN
};

// �߼���������
enum template_logictype {
	TEMPLATE_L_NONE,	
	TEMPLATE_L_AND,
	TEMPLATE_L_OR
};

// �Ƚ���������
enum template_cmptype {
	TEMPLATE_C_EQ,
	TEMPLATE_C_NE,
	TEMPLATE_C_LE,
	TEMPLATE_C_LT,
	TEMPLATE_C_GE,
	TEMPLATE_C_GT
};

} // namespace

#endif //_WEBAPPLIB_TEMPLATE_H_

