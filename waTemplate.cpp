/// \file waTemplate.cpp
/// HTMLģ�崦����ʵ���ļ�

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include "waTemplate.h"

using namespace std;

// WEB Application Library namaspace
namespace webapp {
	
////////////////////////////////////////////////////////////////////////////
// set functions

/// ��ȡHTMLģ���ļ�
/// \param tmpl_file ģ��·���ļ���
/// \retval true ��ȡ�ɹ�
/// \retval false ʧ��
bool Template::load( const string &tmpl_file ) {
	if ( _tmpl.load_file(tmpl_file) ) {
		_tmplfile = tmpl_file;
		return true;
	} else {
		_tmplfile = "Error: Can't open file " + tmpl_file;
		this->error_log( 0, _tmplfile );
		return false;
	}
}

/// ����HTMLģ������
/// \param tmpl ģ�������ַ���
void Template::tmpl( const string &tmpl ) {
	_tmplfile = "Read from string";
	_tmpl = tmpl;
}

/// �����滻����
/// \param name ģ��������
/// \param value �滻ֵ
void Template::set( const string &name, const string &value ) {
	if ( name != "" )
		_sets[name] = value;
}

/// ȡ���滻����
/// \param name ģ��������
void Template::unset( const string &name ) {
	if ( name != "" )
		_sets.erase( name );
}

/// �½����
/// \param table �������
/// \param field_0 field_0��field_0֮��Ϊ�ֶ������б�,���һ������������NULL
/// \param ... �ֶ������б�,���һ������������NULL
void Template::table( const string &table, const char* field_0, ... ) {
	va_list ap;
	const char *p;
	string field;
	strings fields;
	size_t cols = 0;
	
	// check same name table
	if ( _tables.find(table) != _tables.end() )
		this->error_log( 0, "Warning: table name \""+table+"\" redefined" );
	
	// get fields
	va_start( ap, field_0 );
	for ( p=field_0; p; p=va_arg(ap,const char*) ) {
		if ( (field=p) != "" ) {
			fields.push_back( field );
			_tables[table].fieldspos[field] = cols; // for speed
			++cols;
		}
	}
	va_end( ap );

	// for waTemplate old version templet script ( < v0.7 )
	if ( _sets.find(table) == _sets.end() )
		set( table, table );

	// init table
	_tables[table].fields = fields;
	for ( size_t i=0; i<_tables[table].datas.size(); ++i )
		_tables[table].datas[i].clear();
	_tables[table].datas.clear();
	_tables[table].cursor = 0;
	_tables[table].rows = 0;
	_tables[table].cols = cols;
}

/// ȡ�����
/// ִ�гɹ���ɾ���ñ��������������
/// \param table �������
void Template::unset_table( const string &table ) {
	if ( table != "" )
		_tables.erase( table );
}

/// ���һ�����ݵ����
/// �����ȵ���Template::table()��ʼ������ֶζ���,������ֹ
/// \param table �������
/// \param value_0 value_0��value_0֮��Ϊ�ֶ������б�,���һ������������NULL
/// \param ... �ֶ������б�,���һ������������NULL
void Template::set_row( const string &table, const char* value_0, ... ) {
	// table must exist
	if ( _tables.find(table) == _tables.end() ) {
		this->error_log( 0, "Error: Call table() to init $"+table+" first, in set_row()" );
		return;
	}
	
	// get values
	va_list ap;
	const char *p;
	string value;
	strings values;
	int cols = 0;
	
	va_start( ap, value_0 );
	for ( p=value_0; p; p=va_arg(ap,const char*) ) {
		value = p;
		values.push_back( value );
		++cols;
		
		// enough now
		if ( cols >= _tables[table].cols )
			break;
	}
	va_end( ap );

	// fill blank if not enough
	if( cols < _tables[table].cols ) {
		for ( int i=cols; i<_tables[table].cols; ++i )
			values.push_back( "" );
	}
	
	// insert into table
	_tables[table].datas.push_back( values );
	++_tables[table].rows;
}

/// ���һ��ָ����ʽ�����ݵ����
/// �����ȵ���Template::table()��ʼ������ֶζ���,������ֹ
/// \param table �������
/// \param format �ֶ��б��ʽ����,"%d,%s,..."��ʽ
/// \param ... ������������Ϊ�ֶ�ֵ�б�,
/// �ֶ�ֵ���������������ڸ�ʽ�������format��ָ���ĸ���
void Template::format_row( const string &table, const char* format, ... ) {
	// table must exist
	if ( _tables.find(table) == _tables.end() ) {
		this->error_log( 0, "Error: Call table() to init $"+table+" first, in format_row()" );
		return;
	}
	
	// split format string
	String fmtstr = format;
	vector<String> fmtlist = fmtstr.split( TEMPLATE_SPLIT );
	
	// get values
	va_list ap;
	string value;
	strings values;
	int cols = 0;
	
	va_start( ap, format );
	for ( size_t i=0; i<fmtlist.size(); ++i ) {
		// read
		fmtlist[i].trim();
		if ( fmtlist[i] == TEMPLATE_FMTSTR )
			value = va_arg( ap, const char* ); // %s
		else
			value = itos( va_arg(ap,long) ); // %d or other
			
		// push data
		values.push_back( value );
		++cols;

		// enough now
		if ( cols >= _tables[table].cols )
			break;
	}
	va_end( ap );

	// fill blank if not enough
	if( cols < _tables[table].cols ) {
		for ( int i=cols; i<_tables[table].cols; ++i )
			values.push_back( "" );
	}
	
	// insert into table
	_tables[table].datas.push_back( values );
	++_tables[table].rows;
}

/// ���ñ��ָ��λ��ֵ
/// \param table �������
/// \param row λ��,���ڼ���
/// \param field �ֶ�����,���ڼ���
/// \param value Ҫ����Ϊ��ֵ
void Template::set( const string &table, const int row, 
			   const string &field, const string &value ) {
	// table must exist
	if ( _tables.find(table) == _tables.end() ) {
		this->error_log( 0, "Error: Call table() to init $"+table+" first, in set()" );
		return;
	}

	// row out of range
	if ( row > _tables[table].rows ) {
		this->error_log( 0, "Error: $"+table+" row out of range, in set()" );
		return;
	}

	int col = this->field_pos( table, field );
	if ( col != -1 )
		_tables[table].datas[row][col] = value;
}

/// ȡ�����ָ����
/// \param table �������
/// \param row ��λ��
void Template::unset_row( const string &table, const int row ) {
	// table must exist
	if ( _tables.find(table) == _tables.end() ) {
		this->error_log( 0, "Error: Call table() to init $"+table+" first, in unset_row()" );
		return;
	}
	
	if ( row <= _tables[table].rows ) {
		// delete row
		vector<strings>::iterator pos = _tables[table].datas.begin();
		std::advance( pos, row-1 );
		_tables[table].datas.erase( pos );
		--_tables[table].rows;
	}
}

/// ���������
/// \param table �������
/// \param field �����ֶ�
/// \param mode ����ģʽ
/// - Template::TEMPLATE_SORT_ASCE ����
/// - Template::TEMPLATE_SORT_DESC ����
/// - Ĭ��Ϊ����
/// \param cmp ����Ƚ�ģʽ,
/// ����Ϊ�ַ����Ƚ�(Template::TEMPLATE_CMP_STR)���������Ƚ�(Template::TEMPLATE_CMP_INT),Ĭ��Ϊ�ַ����Ƚ�
void Template::sort_table( const string &table, const string &field, 
					  const sort_mode mode, const cmp_mode cmp ) {
	// table must exist
	if ( _tables.find(table) == _tables.end() ) {
		this->error_log( 0, "Error: Call table() to init $"+table+
					  	 " first, in sort_table()" );
		return;
	}

	// get field position
	int col = this->field_pos( table, field );
	if ( col == -1 ) {
		this->error_log( 0, "Warning: field .$"+field+" not defined in tabel \""+
					  		table+"\", in sort_table()" );
		return;
	}

	// sort
	bool swaped;
	bool needswap;
	string curr, next;
	int curr_int, next_int;
	
	do {
		swaped = false;
		for ( int i=0; i<_tables[table].rows-1; ++i ) {
			curr = _tables[table].datas[i][col];
			next = _tables[table].datas[i+1][col];

			if ( cmp == TEMPLATE_CMP_STR ) {
				// string compare
				if ( mode == TEMPLATE_SORT_ASCE )
					needswap = ( curr>next ) ? true : false;
				else
					needswap = ( curr<next ) ? true : false;
			} else {
				// integer compare
				curr_int = stoi( curr );
				next_int = stoi( next );
				
				if ( mode == TEMPLATE_SORT_ASCE )
					needswap = ( curr_int>next_int ) ? true : false;
				else
					needswap = ( curr_int<next_int ) ? true : false;
			}
				
			// swap
			if ( needswap ) {
				std::swap( _tables[table].datas[i], _tables[table].datas[i+1] );
				swaped = true;
			}
		}
	} while ( swaped == true );
}

/// ��������滻����
/// �������б���滻����
void Template::clear_set() {
	_sets.clear();
	_tables.clear();
}

////////////////////////////////////////////////////////////////////////////
// parse functions

/// �����ֶ�λ��
/// \param table �������
/// \param field �ֶ�����
/// \return �ҵ������ֶ�λ��,���򷵻�-1
int Template::field_pos( const string &table, const string &field ) {
	if ( _tables[table].fieldspos.find(field) == _tables[table].fieldspos.end() )
		return -1;
	return _tables[table].fieldspos[field];
}

/// ��ȡָ��λ�õ�ģ��ű����ͼ����ʽ
/// \param tmpl ģ���ַ���
/// \param pos ��ʼ������λ��
/// \param exp ��ȡ���ı��ʽ�ַ���
/// \param type �������Ľű��������
/// \return ����ֵΪ���η������ַ�������,��������-1
int Template::parse_script( const string &tmpl, const size_t pos, 
					   string &exp, int &type ) {
	// find TEMPLATE_END
	size_t begin = pos + TEMPLATE_BEGIN_LEN;
	size_t end;
	if ( (end=tmpl.find(TEMPLATE_END,begin)) == tmpl.npos )
		return -1;	// can not find TEMPLATE_END

	// script type and content
	String content = tmpl.substr( begin, end-begin );
	content.trim();

	if ( strncmp(content.c_str(),TEMPLATE_VALUE,TEMPLATE_VALUE_LEN) == 0 ) {
		// simple value: $xxx
		type = TEMPLATE_S_VALUE;
		
	} else if ( strncmp(content.c_str(),TEMPLATE_TBLVALUE,TEMPLATE_TBLVALUE_LEN) == 0 ) {
		// current value in table: .$xxx
		type = TEMPLATE_S_TBLVALUE;
		
	} else if ( strncmp(content.c_str(),TEMPLATE_FOR,TEMPLATE_FOR_LEN) == 0 ) {
		// for begin: #FOR xxx
		type = TEMPLATE_S_FOR;
		content = tmpl.substr( begin+TEMPLATE_FOR_LEN, end-begin-TEMPLATE_FOR_LEN );
		content.trim();
		
	} else if ( strcmp(content.c_str(),TEMPLATE_ENDFOR) == 0 ) {
		// for end: #ENDFOR
		type = TEMPLATE_S_ENDFOR;
		
	} else if ( strncmp(content.c_str(),TEMPLATE_IF,TEMPLATE_IF_LEN) == 0 ) {
		// if begin: #IF xxx
		type = TEMPLATE_S_IF;
		content = tmpl.substr( begin+TEMPLATE_IF_LEN, end-begin-TEMPLATE_IF_LEN );
		content.trim();
	
	} else if ( strncmp(content.c_str(),TEMPLATE_ELSIF,TEMPLATE_ELSIF_LEN) == 0 ) {
		// elseif: #ELSIF xxx
		type = TEMPLATE_S_ELSIF;
		content = tmpl.substr( begin+TEMPLATE_ELSIF_LEN, end-begin-TEMPLATE_ELSIF_LEN );
		content.trim();
	
	} else if ( strcmp(content.c_str(),TEMPLATE_ELSE) == 0 ) {
		// else: #ELSE
		type = TEMPLATE_S_ELSE;
	
	} else if ( strcmp(content.c_str(),TEMPLATE_ENDIF) == 0 ) {
		// if end: #ENDIF
		type = TEMPLATE_S_ENDIF;
	
	} else if ( strncmp(content.c_str(),TEMPLATE_CURSOR,TEMPLATE_CURSOR_LEN) == 0 ) {
		// current table cursor: %CURSOR
		type = TEMPLATE_S_CURSOR;
	
	} else if ( strncmp(content.c_str(),TEMPLATE_ROWS,TEMPLATE_ROWS_LEN) == 0 ) {
		// current table cursor: %ROWS
		type = TEMPLATE_S_ROWS;

	} else if ( strcmp(content.c_str(),TEMPLATE_DATE) == 0 ) {
		// date: %DATE
		type = TEMPLATE_S_DATE;
	
	} else if ( strcmp(content.c_str(),TEMPLATE_TIME) == 0 ) {
		// time: %TIME
		type = TEMPLATE_S_TIME;
	
	} else if ( strcmp(content.c_str(),TEMPLATE_SPACE) == 0 ) {
		// space char: %SPACE
		type = TEMPLATE_S_SPACE;
	
	} else if ( strcmp(content.c_str(),TEMPLATE_BLANK) == 0 ) {
		// blank string: %BLANK
		type = TEMPLATE_S_BLANK;
	
	} else  {
		type = TEMPLATE_S_UNKNOWN;
	}
	
	// return parsed length
	exp = content;
	return ( end-pos+TEMPLATE_END_LEN );
}

/// �������ʽ��ֵ
/// \param exp ���ʽ�ַ���
/// \return ����ֵΪ�ñ��ʽ��ֵ,�����ʽ�Ƿ��򷵻ر��ʽ�ַ���
string Template::exp_value( const string &exp ) {
	if ( strncmp(exp.c_str(),TEMPLATE_VALUE,TEMPLATE_VALUE_LEN) == 0 ) {
		// simple value: $xxx
		string val = exp.substr( TEMPLATE_VALUE_LEN );
		return _sets[val];
		
	} else if ( strncmp(exp.c_str(),TEMPLATE_TBLVALUE,TEMPLATE_TBLVALUE_LEN) == 0 ) {
		// current value in table: .$xxx
		string val = exp.substr( TEMPLATE_TBLVALUE_LEN );
		return this->table_value( val );
		
	} else if ( strncmp(exp.c_str(),TEMPLATE_CURSOR,TEMPLATE_CURSOR_LEN) == 0 ) {
		// current table cursor: %CURSOR
		size_t pos = exp.find( TEMPLATE_TBLSCOPE );
		if ( pos != exp.npos ) {
			string table_name = this->exp_value( exp.substr(pos+TEMPLATE_TBLSCOPE_LEN) );
			int cursor = _tables[table_name].cursor;
			return itos(cursor+1);
		} else {
			return itos(_cursor+1);
		}
	
	} else if ( strncmp(exp.c_str(),TEMPLATE_ROWS,TEMPLATE_ROWS_LEN) == 0 ) {
		// current table cursor: %ROWS
		size_t pos = exp.find( TEMPLATE_TBLSCOPE );
		if ( pos != exp.npos ) {
			string table_name = this->exp_value( exp.substr(pos+TEMPLATE_TBLSCOPE_LEN) );
			return itos( _tables[table_name].rows );
		} else {
			return itos( _tables[_table].rows );
		}

	} else if ( strcmp(exp.c_str(),TEMPLATE_DATE) == 0 ) {
		// date: %DATE
		return _date;
	
	} else if ( strcmp(exp.c_str(),TEMPLATE_TIME) == 0 ) {
		// time: %TIME
		return _time;
	
	} else if ( strcmp(exp.c_str(),TEMPLATE_SPACE) == 0 ) {
		// space char: %SPACE
		return " ";
	
	} else if ( strcmp(exp.c_str(),TEMPLATE_BLANK) == 0 ) {
		// blank string: %BLANK
		return "";
	
	} else  {
		// string
		return exp;
	}
}

/// ��������ģ��
/// \param tmpl ģ���ַ���
/// \param output ���������������
void Template::parse( const string &tmpl, ostream &output ) {
	// init datetime
	struct tm stm;
	time_t tt = time( 0 );
	localtime_r( &tt, &stm );
	snprintf( _date, 15, "%d-%d-%d", stm.tm_year+1900, stm.tm_mon+1, stm.tm_mday );
	snprintf( _time, 15, "%d:%d:%d", stm.tm_hour, stm.tm_min, stm.tm_sec );

	// confirm if inited
	if ( _tmpl == "" ) {
		this->error_log( 0, "Error: Templet not initialized" );
		return;
	}
	
	// parse init
	_table = "";
	_cursor = 0;
	_lines = 0;
	
	size_t lastpos = 0;
	size_t currpos = 0;

	// for parse_script()
	string exp;
	int type;
	int parsed;
	
	// search TEMPLATE_BEGIN in tmpl
	while( (currpos=tmpl.find(TEMPLATE_BEGIN,lastpos)) != tmpl.npos ) {
		// output html before TEMPLATE_BEGIN
		output << tmpl.substr( lastpos, currpos-lastpos );
		
		// log current position
		if ( _debug == TEMPLATE_OUTPUT_DEBUG ) {
			String orightml = tmpl.substr( lastpos, currpos-lastpos );
			_lines += orightml.count( TEMPLATE_NEWLINE );
		}
		
		// get script content between TEMPLATE_BEGIN and TEMPLATE_END
		parsed = this->parse_script( tmpl, currpos, exp, type );
		
		if ( parsed < 0 ) {
			// can't find TEMPLATE_END
			lastpos = currpos;
			this->error_log( _lines, "Error: Can't find TEMPLATE_END" );
			break;
		}
		
		// parse by script type
		switch ( type ) {
			case TEMPLATE_S_VALUE:
				// replace
			case TEMPLATE_S_DATE:
				// replace with date
			case TEMPLATE_S_TIME:
				// replace with time
			case TEMPLATE_S_SPACE:
				// replace with space char
			case TEMPLATE_S_BLANK:
				// replace with blank string
				output << this->exp_value( exp );
				break;

			case TEMPLATE_S_IF:
				// condition replace
				parsed = this->parse_if( tmpl.substr(currpos), output, true, exp, parsed );
				break;
				
			case TEMPLATE_S_FOR:
				// cycle replace
				parsed = this->parse_for( tmpl.substr(currpos), output, true, exp, parsed );
				// restore table status
				_table = "";
				_cursor = 0;
				break;

			case TEMPLATE_S_UNKNOWN:
				// unknown script, maybe html code
				this->error_log( _lines, "Warning: Unknown script, in parse()" );
				
				// for syntax error
				size_t backlen;
				if ( (backlen=exp.find(TEMPLATE_BEGIN)) != exp.npos )
					parsed = backlen+TEMPLATE_BEGIN_LEN;
					
				output << tmpl.substr( currpos, parsed );
				break;
				
			default:
				// syntax error
				this->error_log( _lines, "Error: Unexpected script, in parse()" );
		}

		// location to next position
		lastpos = currpos + parsed;
	}
	
	// output tail html
	output << tmpl.substr( lastpos );
}

/// ������������ʽ�Ƿ����
/// \param exp ����Ϊ�������ʽ,
/// �����ʽΪ�ַ���,��ֵ��Ϊ""���Ҳ�Ϊ"0"ʱ����true,���򷵻�false,
/// ��Ϊ�Ƚϱ��ʽ,��������true,���򷵻�false
/// \retval true �������ʽ����
/// \retval false �������ʽ������
bool Template::compare( const string &exp ) {
	// read compare type
	// supported: ==,!=,<=,<,>=,>
	string cmpop;
	size_t oppos;
	template_cmptype optype;
	
	if ( (oppos=exp.find(TEMPLATE_EQ)) != exp.npos ) {
		// ==
		cmpop = TEMPLATE_EQ;
		optype = TEMPLATE_C_EQ;
	
	} else if ( (oppos=exp.find(TEMPLATE_NE)) != exp.npos ) {
		// !=
		cmpop = TEMPLATE_NE;
		optype = TEMPLATE_C_NE;
	
	} else if ( (oppos=exp.find(TEMPLATE_LE)) != exp.npos ) {
		// <=
		cmpop = TEMPLATE_LE;
		optype = TEMPLATE_C_LE;
	
	} else if ( (oppos=exp.find(TEMPLATE_LT)) != exp.npos ) {
		// <
		cmpop = TEMPLATE_LT;
		optype = TEMPLATE_C_LT;
	
	} else if ( (oppos=exp.find(TEMPLATE_GE)) != exp.npos ) {
		// >=
		cmpop = TEMPLATE_GE;
		optype = TEMPLATE_C_GE;
	
	} else if ( (oppos=exp.find(TEMPLATE_GT)) != exp.npos ) {
		// >
		cmpop = TEMPLATE_GT;
		optype = TEMPLATE_C_GT;
	
	} else {
		// read value, compare and return
		string val = this->exp_value( exp );
		if ( val!="" && val!="0" )
			return true;
		else
			return false;
	}

	// split exp by compare operator
	String lexp = exp.substr( 0, oppos );
	String rexp = exp.substr( oppos+cmpop.length() );
	
	// read value
	lexp.trim(); rexp.trim();
	lexp = this->exp_value( lexp );
	rexp = this->exp_value( rexp );

	// compare
	int cmp;
	if ( lexp.isnum() && rexp.isnum() ) {
		int lv = atoi( lexp.c_str() );
		int rv = atoi( rexp.c_str() );
		cmp = ( lv>rv ) ? 1 : ( lv==rv ) ? 0 : -1;
	} else {
		cmp = strcmp( lexp.c_str(), rexp.c_str() );
	}

	// return
	switch ( optype ) {
		case TEMPLATE_C_EQ:
			return ( cmp==0 ) ? true : false;
		case TEMPLATE_C_NE:
			return ( cmp!=0 ) ? true : false;
		case TEMPLATE_C_LE:
			return ( cmp<=0 ) ? true : false;
		case TEMPLATE_C_LT:
			return ( cmp<0 ) ? true : false;
		case TEMPLATE_C_GE:
			return ( cmp>=0 ) ? true : false;
		case TEMPLATE_C_GT:
			return ( cmp>0 ) ? true : false;
		default:
			return false;
	}
}

/// ��������Ƿ����	
/// \param exp ����Ϊ�������ʽ�������
/// \retval true �������ʽ����
/// \retval false �������ʽ������
bool Template::check_if( const string &exp ) {
	template_logictype exp_type = TEMPLATE_L_NONE;
	String exps;

	// check expression type
	if ( strncmp(exp.c_str(),TEMPLATE_AND,TEMPLATE_AND_LEN) == 0 ) {
		exp_type = TEMPLATE_L_AND;
		exps = exp.substr( TEMPLATE_AND_LEN );
	} else if ( strncmp(exp.c_str(),TEMPLATE_OR,TEMPLATE_OR_LEN) == 0 ) {
		exp_type = TEMPLATE_L_OR;
		exps = exp.substr( TEMPLATE_OR_LEN );
	}

	// none logic expression
	if ( exp_type == TEMPLATE_L_NONE )
		return this->compare( exp );

	// check TEMPLATE_SUBBEGIN/TEMPLATE_SUBEND
	exps.trim();
	size_t explen = exps.length();
	if ( exps.substr(0,TEMPLATE_SUBBEGIN_LEN)!=TEMPLATE_SUBBEGIN ||
		 exps.substr(explen-TEMPLATE_SUBEND_LEN)!=TEMPLATE_SUBEND ) {
		this->error_log( _lines, "Warning: Maybe wrong TEMPLATE_AND or TEMPLATE_OR script" );
		return this->compare( exp );
	}
		
	// split expressions list
	exps = exps.substr( TEMPLATE_SUBBEGIN_LEN, explen-TEMPLATE_SUBBEGIN_LEN-TEMPLATE_SUBEND_LEN );
	vector<String> explist = exps.split( TEMPLATE_SPLIT );

	// judge
	if ( exp_type == TEMPLATE_L_AND ) {
		// TEMPLATE_AND
		for ( unsigned int i=0; i<explist.size(); i++ ) {
			explist[i].trim();
			if ( !this->compare(explist[i]) )
				return false;
		}
		return true;
		
	} else {
		// TEMPLATE_OR
		for ( size_t i=0; i<explist.size(); i++ ) {
			explist[i].trim();
			if ( this->compare(explist[i]) )
				return true;
		}
		return false;
	}
	
	return false; // for warning
}

/// ������������ģ��
/// \param tmpl ģ���ַ���
/// \param output ���������������
/// \param parent_state ���øú���ʱ������״̬
/// \param parsed_exp �ѷ����������ű����ʽ
/// \param parsed_length �ѷ����������ű����ʽ����
/// \return ����ֵΪ���η������ַ�������
size_t Template::parse_if( const string &tmpl, ostream &output, 
					  const bool parent_state, const string &parsed_exp,
					  const int parsed_length ) {
	// parsed length
	size_t length = parsed_length;
		
	// check status
	bool status;
	bool effected;
	if ( parent_state == false ) {
		status = false;
		effected = true;
	} else if ( this->check_if(parsed_exp) ) {
		status = true;
		effected = true;
	} else {
		status = false;
		effected = false;
	}
		
	// parse
	size_t lastpos = parsed_length;
	size_t currpos = parsed_length;

	// for parse_script()
	string exp;
	int type;
	int parsed;
	
	while( (currpos=tmpl.find(TEMPLATE_BEGIN,lastpos)) != tmpl.npos ) {
		// output html before TEMPLATE_BEGIN if status valid
		if ( status )
			output << tmpl.substr( lastpos, currpos-lastpos );
		length += ( currpos-lastpos );
		
		// log current position
		if ( _debug == TEMPLATE_OUTPUT_DEBUG ) {
			String orightml = tmpl.substr( lastpos, currpos-lastpos );
			_lines += orightml.count( TEMPLATE_NEWLINE );
		}
		
		// get script content between TEMPLATE_BEGIN and TEMPLATE_END
		parsed = this->parse_script( tmpl, currpos, exp, type );
		
		if ( parsed < 0 ) {
			// can't find TEMPLATE_END
			this->error_log( _lines, "Error: Can't find TEMPLATE_END" );
			lastpos = currpos;
			break;
		}
		
		// parse by script type
		switch ( type ) {
			case TEMPLATE_S_VALUE:
				// replace if status is true
			case TEMPLATE_S_TBLVALUE:
				// replace with table value if status is true
			case TEMPLATE_S_CURSOR:
				// replace with cursor
			case TEMPLATE_S_ROWS:
				// replace with rows
			case TEMPLATE_S_DATE:
				// replace with date
			case TEMPLATE_S_TIME:
				// replace with time
			case TEMPLATE_S_SPACE:
				// replace with space char
			case TEMPLATE_S_BLANK:
				// replace with blank string
				if ( status )
					output << this->exp_value( exp );
				break;

			case TEMPLATE_S_ELSIF:
				// check and set status
				if ( effected ) {
					// something effected before
					status = false;
				} else if ( this->check_if(exp) ) {
					// nothing effected and status is true now
					status = true;
					effected = true;
				} else {
					// nothing effected, status is false now
					status = false;
				}
				break;

			case TEMPLATE_S_ELSE:
				// check and set status
				if ( effected ) {
					// something effected before
					status = false;
				} else {
					// nothing effected, then status is true
					status = true;
					effected = true;
				}
				break;

			case TEMPLATE_S_ENDIF:
				// parsed length
				length += parsed;
				// exit function
				return length;
					
			case TEMPLATE_S_IF:
				// sub condition replace
				parsed = this->parse_if( tmpl.substr(currpos), output, status, exp, parsed );
				break;
				
			case TEMPLATE_S_FOR: { // make compiler happy
				// backup current table status
				string parent_table = _table;
				int parent_cursor = _cursor;
				
				// sub cycle replace
				parsed = this->parse_for( tmpl.substr(currpos), output, status, exp, parsed );
				
				// restore table status
				_table = parent_table;
				_cursor = parent_cursor;
				_tables[_table].cursor = _cursor;
				}
				break;

			case TEMPLATE_S_UNKNOWN:
				// unknown script, maybe html code
				this->error_log( _lines, "Warning: Unknown script, in parse_if()" );

				// for syntax error
				size_t backlen;
				if ( (backlen=exp.find(TEMPLATE_BEGIN)) != exp.npos )
					parsed = backlen+TEMPLATE_BEGIN_LEN;

				if ( status )
					output << tmpl.substr( currpos, parsed );
				break;

			default:
				// syntax error
				error_log( _lines, "Error: Unexpected script, in parse_if()" );
		}

		// parsed length
		length += parsed;
		// location to next position
		lastpos = currpos + parsed;
	}
	
	// can not find TEMPLATE_ENDIF
	this->error_log( _lines, "Error: Can't find TEMPLATE_ENDIF" );

	// output tail html
	if ( status )
		output << tmpl.substr( lastpos );
	length += ( tmpl.size()-lastpos );
	
	return length;
}

/// ���ѭ������Ƿ���Ч
/// \param table ѭ���������
/// \retval true ����Ѷ���
/// \retval false δ����
bool Template::check_for( const string &tablename ) {
	string table = this->exp_value( tablename );
	
	if ( _tables.find(table) != _tables.end() && _tables[table].rows > 0 ) {
		return true;
	} else {
		this->error_log( _lines, "Warning: table " + tablename + " \""+table+
						   		 "\" not defined or not set data" );
		return false;
	}
}

/// ���ر����ָ��λ���ֶε�ֵ
/// \param fleid �������ֶ���
/// \return ����ȡ�ɹ�����ֵ�ַ���,���򷵻ؿ��ַ���
string Template::table_value( const string &field ) {
	// get table info
	size_t pos = field.find( TEMPLATE_TBLSCOPE );
	if ( pos != field.npos ) {
		string table_name = this->exp_value( field.substr(pos+TEMPLATE_TBLSCOPE_LEN) );
		string field_name = field.substr( 0, pos );
		int cursor = _tables[table_name].cursor;

		// return value
		int col = this->field_pos( table_name, field_name );
		if ( col!=-1 && cursor<_tables[table_name].rows )
			return _tables[table_name].datas[cursor][col];
		else
			return string( "" );
	} else {
		// return value
		int col = this->field_pos( _table, field );
		if ( col!=-1 && _cursor<_tables[_table].rows )
			return _tables[_table].datas[_cursor][col];
		else
			return string( "" );
	}
}

/// ����ѭ������ģ��
/// \param tmpl ģ���ַ���
/// \param output ���������������
/// \param parent_state ���øú���ʱ������״̬
/// \param parsed_exp �ѷ�����ѭ���ű����ʽ
/// \param parsed_length �ѷ�����ѭ���ű����ʽ����
/// \return ����ֵΪ���η������ַ�������
size_t Template::parse_for( const string &tmpl, ostream &output, 
					   const bool parent_state, const string &parsed_exp,
					   const int parsed_length ) {
	// parsed length
	size_t length = parsed_length;
		
	// check status
	bool status;
	if ( parent_state == false )
		status = false;
	else if ( this->check_for(parsed_exp) )
		status = true;
	else
		status = false;
		
	// init
	bool cycled = false;
	int cursor = 0;
	size_t start_pos = parsed_length;
	size_t start_len = parsed_length;
	size_t lastpos = parsed_length;
	size_t currpos = parsed_length;
	
	// current table name
	string table = this->exp_value( parsed_exp );
	_table = table;
	_tables[_table].cursor = 0;

	// for parse_script()		
	string exp;
	int type;
	int parsed;

	while( (currpos=tmpl.find(TEMPLATE_BEGIN,lastpos)) != tmpl.npos ) {
		// output html before TEMPLATE_BEGIN if status valid
		if ( status )
			output << tmpl.substr( lastpos, currpos-lastpos );
		length += ( currpos-lastpos );
		
		// log current position
		if ( !cycled && _debug==TEMPLATE_OUTPUT_DEBUG ) {
			String orightml = tmpl.substr( lastpos, currpos-lastpos );
			_lines += orightml.count( TEMPLATE_NEWLINE );
		}
		
		// get script content between TEMPLATE_BEGIN and TEMPLATE_END
		parsed = this->parse_script( tmpl, currpos, exp, type );
		
		if ( parsed < 0 ) {
			// can't find TEMPLATE_END
			if ( !cycled )
				this->error_log( _lines, "Error: Can't find TEMPLATE_END" );
			lastpos = currpos;
			break;
		}
		
		// current table cursor
		_cursor = cursor;

		// parse by script type
		switch ( type ) {
			case TEMPLATE_S_VALUE:
				// replace
			case TEMPLATE_S_TBLVALUE:
				// replace with table value in table
			case TEMPLATE_S_CURSOR:
				// replace with cursor
			case TEMPLATE_S_ROWS:
				// replace with rows				
			case TEMPLATE_S_DATE:
				// replace with date
			case TEMPLATE_S_TIME:
				// replace with time
			case TEMPLATE_S_SPACE:
				// replace with space char
			case TEMPLATE_S_BLANK:
				// replace with blank string
				if ( status )
					output << this->exp_value( exp );
				break;

			case TEMPLATE_S_ENDFOR:
				// at the end of this cycle
				++cursor; // data cursor
				_tables[_table].cursor = cursor;
				if ( status && cursor<_tables[table].rows ) {
					// next cycle
					length = start_len;		// reset parsed length
					lastpos = start_pos;	// reset start position
					cycled = true;			// do not add current position value
					continue;
				} else {
					// parsed length
					length += parsed;
					// exit function
					return length;
				}
					
			case TEMPLATE_S_IF:
				// sub condition replace
				parsed = this->parse_if( tmpl.substr(currpos), output, status, exp, parsed );
				break;
				
			case TEMPLATE_S_FOR:
				// sub cycle replace
				parsed = this->parse_for( tmpl.substr(currpos), output, status, exp, parsed );
				// restore table status
				_table = table;
				_cursor = cursor;
				_tables[_table].cursor = _cursor;
				break;

			case TEMPLATE_S_UNKNOWN:
				// unknown script, maybe html code
				if ( !cycled )
					this->error_log( _lines, "Warning: Unknown script, in parse_for()" );

				// for syntax error
				size_t backlen;
				if ( (backlen=exp.find(TEMPLATE_BEGIN)) != exp.npos )
					parsed = backlen+TEMPLATE_BEGIN_LEN;

				if ( status )
					output << tmpl.substr( currpos, parsed );
				break;

			default:
				// syntax error
				if ( !cycled )
					this->error_log( _lines, "Error: Unexpected script, in parse_for()" );
		}

		// parsed length
		length += parsed;
		// location to next position
		lastpos = currpos + parsed;
	}
	
	// can not find TEMPLATE_ENDFOR
	this->error_log( _lines, "Error: Can't find TEMPLATE_ENDFOR" );

	// output tail html
	if ( status )
		output << tmpl.substr( lastpos );
	length += ( tmpl.size()-lastpos );

	return length;
}

////////////////////////////////////////////////////////////////////////////
// output functions

/// ģ����������¼
/// \param pos ģ�������λ��
/// \param error ��������
void Template::error_log( const size_t lines, const string &error ) {
	if ( error != "" )
		_errlog.insert( multimap<int,string>::value_type(lines,error) );
}

/// ����ģ�������¼
/// \param output ���������������
void Template::parse_log( ostream &output ) {
	output << endl;
	output << "<!-- Generated by waTemplate " << _date << " " << _time << endl
		   << "  Templet source: " << _tmplfile << endl
		   << "  Tables: " << _tables.size() << endl;

	for ( map<string,template_table>::const_iterator i=_tables.begin(); i!=_tables.end(); ++i ) {
		if ( i->first != "" ) {
			output << "    Table " << i->first
				   << "\t\t" << (i->second).cursor << " rows" << endl;
		}
	}

	output << "  Errors: " << _errlog.size() << endl;
	for ( multimap<int,string>::const_iterator i=_errlog.begin(); i!=_errlog.end(); ++i ) {
		output << "    Line " << i->first+1
			   << "\t\t" << i->second << endl;
	}
			   
	output << "-->";
	_errlog.clear();
}

/// ����HTML�ַ���
/// \return ����ģ�����������
string Template::html() {
	ostringstream result;
	this->parse( _tmpl, result );
	result << ends;
	return result.str();
}

/// ���HTML��stdout
/// \param mode �Ƿ����������Ϣ
/// - Template::TEMPLATE_OUTPUT_DEBUG ���������Ϣ
/// - Template::TEMPLATE_OUTPUT_RELEASE �����������Ϣ
/// - Ĭ��Ϊ�����������Ϣ
void Template::print( const output_mode mode ) {
	_debug = mode;
	this->parse( _tmpl, std::cout );
	if ( _debug == TEMPLATE_OUTPUT_DEBUG ) 
		this->parse_log( std::cout );
}

/// ���HTML���ļ�
/// \param file ����ļ���
/// \param mode �Ƿ����������Ϣ
/// - Template::TEMPLATE_OUTPUT_DEBUG ���������Ϣ
/// - Template::TEMPLATE_OUTPUT_RELEASE �����������Ϣ
/// - Ĭ��Ϊ�����������Ϣ
/// \param permission �ļ����Բ�����Ĭ��Ϊ0666
/// \retval true �ļ�����ɹ�
/// \retval false ʧ��
bool Template::print( const string &file, const output_mode mode,
				 const mode_t permission ) {
	ofstream outfile( file.c_str(), ios::trunc|ios::out );
	if ( outfile ) {
		// parse
		_debug = mode;
		this->parse( _tmpl, outfile );
		if ( _debug == TEMPLATE_OUTPUT_DEBUG ) 
			this->parse_log( outfile );
		outfile.close();
		
		// chmod
		mode_t mask = umask( 0 );
		chmod( file.c_str(), permission );
		umask( mask );

		return true;
	}
	
	return false;
}

} // namespace


