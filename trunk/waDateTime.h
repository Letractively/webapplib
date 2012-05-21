/// \file waDateTime.h
/// DateTime��ͷ�ļ�
/// ����ʱ������

#ifndef _WEBAPPLIB_DATETIME_H_
#define _WEBAPPLIB_DATETIME_H_ 

#include <string>
#include <ctime>

using namespace std;

// WEB Application Library namaspace
namespace webapp {

/// \defgroup waDateTime waDateTime�������������ȫ�ֺ���

/// \ingroup waDateTime
/// \def TIME_ONE_SEC 
/// ʱ������һ����
#define	TIME_ONE_SEC	1

/// \ingroup waDateTime
/// \def TIME_ONE_MIN 
/// ʱ������һ����
#define	TIME_ONE_MIN	60

/// \ingroup waDateTime
/// \def TIME_ONE_HOUR 
/// ʱ������һСʱ
#define	TIME_ONE_HOUR	3600

/// \ingroup waDateTime
/// \def TIME_ONE_DAY 
/// ʱ������һ��
#define	TIME_ONE_DAY	86400

/// \ingroup waDateTime
/// \def TIME_ONE_WEEK 
/// ʱ������һ��
#define	TIME_ONE_WEEK	604800

/// DateTime����ʱ��������
class DateTime {
	public:
	
	/// Ĭ�Ϲ��캯��,�Ե�ǰʱ�乹�����
	DateTime() {
		this->set();
	}
	
	/// ����Ϊ time_t �Ĺ��캯��
	DateTime( const time_t &tt ) {
		this->set( tt );
	}
	
	/// ����Ϊָ��ʱ��Ĺ��캯��
	DateTime( const int year, const int mon, const int mday, 
		  const int hour=0, const int min=0, const int sec=0 ) {
		this->set( year, mon, mday, hour, min, sec );
	}
	
	/// ����Ϊ tm �ṹ�Ĺ��캯��
	DateTime( const tm &st ) {
		this->set( st );
	}
	
	/// ����Ϊ"YYYY-MM-DD HH:MM:SS"��ʽ�ַ����Ĺ��캯��
	DateTime( const string &datetime, const string &datemark = "-", 
		  const string &dtmark = " ", const string &timemark = ":" ) {
		this->set( datetime, datemark, dtmark, timemark );
	}

	/// �������캯��
	DateTime ( const DateTime &date ) {
		this->set( date );
	}
		  
	/// ��������
	virtual ~DateTime() {}
	
	/// ��ֵ����
	DateTime& operator=( const DateTime &date );
	/// ��ֵ����
	DateTime& operator=( const time_t &tt );

	/// ��������
	DateTime& operator+=( const DateTime &date );
	/// ��������
	DateTime& operator+=( const time_t &tt );
	
	/// �ݼ�����
	DateTime& operator-=( const DateTime &date );
	/// �ݼ�����
	DateTime& operator-=( const time_t &tt );

	/// ������λ�����
	inline int year() const {return _tm.tm_year+1900;}
	/// �����·�
	inline int month() const {return _tm.tm_mon+1;}
	/// ���ص�������
	inline int m_day() const {return _tm.tm_mday;}
	/// ���ص�������
	/// ��һ����������1~6�����շ���0
	inline int w_day() const {return _tm.tm_wday;}
	/// ���ص�������
	inline int y_day() const {return _tm.tm_yday;}
	/// ����Сʱ
	inline int hour() const {return _tm.tm_hour;}
	/// ���ط���
	inline int min() const {return _tm.tm_min;}
	/// ��������
	inline int sec() const {return _tm.tm_sec;}
	
	/// ���� 1970-1-1 0:0:0 ����������
	inline long secs() const {return _time;}
	/// ���� 1970-1-1 0:0:0 �����ķ�����
	inline long mins() const {return ( _time/TIME_ONE_MIN );}
	/// ���� 1970-1-1 0:0:0 ������Сʱ��
	inline long hours() const {return ( _time/TIME_ONE_HOUR );}
	/// ���� 1970-1-1 0:0:0 ����������
	inline long days() const {return ( _time/TIME_ONE_DAY );}
	/// ���� 1970-1-1 0:0:0 ����������
	inline long weeks() const {return ( _time/TIME_ONE_WEEK );}
	
	/// �Ե�ǰʱ�����ö���
	void set();
	/// �� time_t �������ö���
	void set( const time_t &tt );
	/// �� tm �ṹ�������ö���
	void set( const tm &st );
	/// ��ָ��ʱ�����ö���
	void set( const int year, const int mon, const int mday, 
			  const int hour=0, const int min=0, const int sec=0 );
	/// �� DateTime �������ö���
	void set( const DateTime &date );
	/// ��"YYYY-MM-DD HH:MM:SS"��ʽ�ַ������ö���
	void set( const string &datetime, const string &datemark = "-", 
			  const string &dtmark = " ", const string &timemark = ":" );
	
	/// ���� time_t ���͵Ķ���ֵ
	inline time_t value() const {return this->secs();}
	/// ���� struct tm ���͵Ķ���ֵ
	inline tm struct_tm() const {return _tm;}
	
	/// ��������ַ���
	string date( const string &datemark = "-", 
				 const bool leadingzero = true ) const;
				 
	/// ���ʱ���ַ���
	string time( const string &timemark = ":", 
				 const bool leadingzero = true ) const;
				 
	/// �������ʱ���ַ���
	string datetime( const string &datemark = "-", const string &dtmark = " ",
					 const string &timemark = ":", const bool leadingzero = true ) const;
					 
	/// ��� GMT ��ʽ����ʱ���ַ���
	string gmt_datetime() const;
	
	////////////////////////////////////////////////////////////////////////////
	private:
	time_t _time;
	struct tm _tm;	
};

// operators

/// \ingroup waDateTime
/// \fn DateTime operator+( const DateTime &date1, const DateTime &date2 )
/// ʱ�����
DateTime operator+( const DateTime &date1, const DateTime &date2 );

/// \ingroup waDateTime
/// \fn DateTime operator+( const DateTime &date, const time_t &tt )
/// ʱ�����
DateTime operator+( const DateTime &date, const time_t &tt );

/// \ingroup waDateTime
/// \fn DateTime operator-( const DateTime &date1, const DateTime &date2 )
/// ʱ�����
DateTime operator-( const DateTime &date1, const DateTime &date2 );

/// \ingroup waDateTime
/// \fn DateTime operator-( const DateTime &date, const time_t &tt )
/// ʱ�����
DateTime operator-( const DateTime &date, const time_t &tt );

/// \ingroup waDateTime
/// \fn bool operator==( const DateTime &left, const DateTime &right )
/// ʱ����ȱȽ�
inline bool operator==( const DateTime &left, const DateTime &right ) {
	return ( left.value() == right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator==( const DateTime &left, const time_t &right )
/// ʱ����ȱȽ�
inline bool operator==( const DateTime &left, const time_t &right ) {
	return ( left.value() == right );
}

/// \ingroup waDateTime
/// \fn bool operator!=( const DateTime &left, const DateTime &right )
/// ʱ�䲻��ȱȽ�
inline bool operator!=( const DateTime &left, const DateTime &right ) {
	return ( left.value() != right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator!=( const DateTime &left, const time_t &right )
/// ʱ�䲻��ȱȽ�
inline bool operator!=( const DateTime &left, const time_t &right ) {
	return ( left.value() != right );
}

/// \ingroup waDateTime
/// \fn bool operator>( const DateTime &left, const DateTime &right )
/// ʱ����ڱȽ�
inline bool operator>( const DateTime &left, const DateTime &right ) {
	return ( left.value() > right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator>( const DateTime &left, const time_t &right )
/// ʱ����ڱȽ�
inline bool operator>( const DateTime &left, const time_t &right ) {
	return ( left.value() > right );
}

/// \ingroup waDateTime
/// \fn bool operator<( const DateTime &left, const DateTime &right )
/// ʱ��С�ڱȽ�
inline bool operator<( const DateTime &left, const DateTime &right ) {
	return ( left.value() < right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator<( const DateTime &left, const time_t &right )
/// ʱ��С�ڱȽ�
inline bool operator<( const DateTime &left, const time_t &right ) {
	return ( left.value() < right );
}

/// \ingroup waDateTime
/// \fn bool operator>=( const DateTime &left, const DateTime &right )
/// ʱ�䲻С�ڱȽ�
inline bool operator>=( const DateTime &left, const DateTime &right ) {
	return ( left.value() >= right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator>=( const DateTime &left, const time_t &right )
/// ʱ�䲻С�ڱȽ�
inline bool operator>=( const DateTime &left, const time_t &right ) {
	return ( left.value() >= right );
}

/// \ingroup waDateTime
/// \fn bool operator<=( const DateTime &left, const DateTime &right )
/// ʱ�䲻���ڱȽ�
inline bool operator<=( const DateTime &left, const DateTime &right ) {
	return ( left.value() <= right.value() );
}

/// \ingroup waDateTime
/// \fn bool operator<=( const DateTime &left, const time_t &right ) 
/// ʱ�䲻���ڱȽ�
inline bool operator<=( const DateTime &left, const time_t &right ) {
	return ( left.value() <= right );
}

} // namespace

#endif //_WEBAPPLIB_DATETIME_H_

